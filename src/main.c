/************************************************************************************
 * 	
 * 	@file     main.c
 *  @author   Mario Lanzilotta
 * 	@brief    Continuous blood pressure monitor code for nrf52840dk
 *  
************************************************************************************/

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>
#include "../drivers/sensor_ppg.h"

//Initialize Logger
LOG_MODULE_REGISTER(bp, LOG_LEVEL_DBG);

//Stacksize for threads
#define STACKSIZE 2048

//Size of proximal and distal arrays, number of samples taken per cycle
#define PPG_ARRAY_SIZE 2000
//Sampling rate of PPG sensors (in Hz)
#define PPG_SAMPLE_RATE 1000

//Thread priorities
#define READ_THREAD_PRIORITY 1
#define CALC_THREAD_PRIORITY 2

#define CS_NODE DT_ALIAS(ppgcs)

//Time between sensor read cycles
#define SENSOR_SLEEP_MS 5000

#define SPI_PPG_OP SPI_OP_MODE_MASTER | SPI_MODE_CPOL | SPI_MODE_CPHA | SPI_WORD_SET(8) | SPI_LINES_SINGLE | SPI_TRANSFER_MSB 

static const struct spi_dt_spec spi_ppg = SPI_DT_SPEC_GET(DT_NODELABEL(adpd1801), SPI_PPG_OP, 0);
static const struct spi_dt_spec spi_ppg2 = SPI_DT_SPEC_GET(DT_NODELABEL(adpd1801_2), SPI_PPG_OP, 0);

static const struct gpio_dt_spec ppg_cs = GPIO_DT_SPEC_GET(CS_NODE, gpios);

static uint32_t proximal[PPG_ARRAY_SIZE];
static uint32_t distal[PPG_ARRAY_SIZE];

void read_thread(void) {
	LOG_INF("Entering read_thread\n");
	int err;

	//Reset PPG sensor
	err = ppg_software_reset(spi_ppg, ppg_cs);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	err = ppg_software_reset(spi_ppg2, ppg_cs);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	//Set up PPG sensor
	err = ppg_start_config(spi_ppg, ppg_cs);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	err = ppg_start_config(spi_ppg2, ppg_cs);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	//Config num channels
	err = ppg_config_num_channels(spi_ppg, ppg_cs);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	err = ppg_config_num_channels(spi_ppg2, ppg_cs);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	//Config LED settings and time slots
	err = ppg_config_leds(spi_ppg, ppg_cs);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	err = ppg_config_leds(spi_ppg2, ppg_cs);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	//Config FIFO queue
	err = ppg_config_fifo(spi_ppg, ppg_cs);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	err = ppg_config_fifo(spi_ppg2, ppg_cs);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	//Config sampling freq
	err = ppg_config_sampling_freq(spi_ppg, PPG_SAMPLE_RATE, ppg_cs);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	err = ppg_config_sampling_freq(spi_ppg2, PPG_SAMPLE_RATE, ppg_cs);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	//Config GPIO outputs (mostly for debugging)
	err = ppg_config_gpios(spi_ppg, ppg_cs);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	err = ppg_config_gpios(spi_ppg2, ppg_cs);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	//Extra configs for sensor tuning
	err = ppg_set_LED_drive(spi_ppg, ppg_cs, 0, 3);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	err = ppg_set_LED_drive(spi_ppg2, ppg_cs, 0, 3);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	err = ppg_set_slot_A(spi_ppg, ppg_cs, 0x04, 0x20, 0x02, 0x14);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	err = ppg_set_slot_A(spi_ppg2, ppg_cs, 0x04, 0x20, 0x02, 0x14);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	//Exit program mode
	err = ppg_exit_config(spi_ppg, ppg_cs);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	err = ppg_exit_config(spi_ppg2, ppg_cs);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }

	LOG_INF("Starting read loop\n");
	//Read from PPG Sensor
	while (err == 0) {
		/*
		//clear queue before reading values for this cycle
		err = ppg_clear_fifo(spi_ppg, ppg_cs);
		if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    	}
		k_msleep(10);*/
		err = ppg_read_sensors(spi_ppg, spi_ppg2, ppg_cs, proximal, distal , PPG_ARRAY_SIZE);
		if (err < 0) {
            LOG_ERR("SPI Read failed (%d)\n", err);
    	}

		for (int i = 0; i < PPG_ARRAY_SIZE; i++) {
			printf("%d,", proximal[i]);
		}
		printf("\n\n");

		k_msleep(SENSOR_SLEEP_MS);
	}

	//If an error occurs, break loop
	//TODO: add better error handling
	LOG_ERR("err = %d\n", err);

	while (1) {
		k_msleep(SENSOR_SLEEP_MS);
	}
}

void calc_thread(void) {
	//TODO: Put Algorithm Here
	while (1) {
		k_yield();
	}
}

int main(void)
{
	int err;
	//Configure GPIOS
	LOG_INF("Configuring GPIO pins\n");
	if (!device_is_ready(ppg_cs.port)) {
		LOG_ERR("GPIO device is not ready");
		return 0;
	}
	err = gpio_pin_configure_dt(&ppg_cs, GPIO_OUTPUT_INACTIVE);

	//Configure SPI
	LOG_INF("Configuring SPI\n");
    if (!spi_is_ready_dt(&spi_ppg)) {
        LOG_ERR("SPI device not ready, aborting\n");
		return 0;
	}
	if (!spi_is_ready_dt(&spi_ppg2)) {
        LOG_ERR("SPI device not ready, aborting\n");
		return 0;
	}

	//TODO: start BLE advertising here
}

//Initialize the threads
K_THREAD_DEFINE(rd_thread, STACKSIZE, read_thread, NULL, NULL, NULL, 
                READ_THREAD_PRIORITY, 0, 0); 
/*K_THREAD_DEFINE(cal_thread, STACKSIZE, calc_thread, NULL, NULL, NULL, 
                CALC_THREAD_PRIORITY, 0, 0); */
