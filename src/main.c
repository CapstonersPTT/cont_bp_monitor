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
//Thread priorities
#define READ_THREAD_PRIORITY 1
#define CALC_THREAD_PRIORITY 2
#define BLE_THREAD_PRIORITY 3

#define CS_NODE DT_ALIAS(ppgcs)

//Time between sensor reads
#define SENSOR_SLEEP_MS 93

#define SPI_PPG_OP SPI_OP_MODE_MASTER | SPI_MODE_CPOL | SPI_MODE_CPHA | SPI_WORD_SET(8) | SPI_LINES_SINGLE | SPI_TRANSFER_MSB

static const struct spi_dt_spec spi_ppg = SPI_DT_SPEC_GET(DT_NODELABEL(adpd1801), SPI_PPG_OP, 0);
static const struct spi_dt_spec spi_ppg2 = SPI_DT_SPEC_GET(DT_NODELABEL(adpd1801_2), SPI_PPG_OP, 0);

static const struct gpio_dt_spec ppg_cs = GPIO_DT_SPEC_GET(CS_NODE, gpios);

//TODO: define array for holding PPG sensor readings

void read_thread(void) {
    int err;
	//Reset PPG sensor
	err = ppg_software_reset(spi_ppg, ppg_cs);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	//Set up PPG sensor
	err = ppg_start_config(spi_ppg, ppg_cs);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	//Config num channels
	err = ppg_config_num_channels(spi_ppg, ppg_cs);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	//Config LED settings and time slots
	err = ppg_config_leds(spi_ppg, ppg_cs);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	//Config FIFO queue
	err = ppg_config_fifo(spi_ppg, ppg_cs);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	//Config sampling freq
	err = ppg_config_sampling_freq(spi_ppg, 10, ppg_cs);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	//Config GPIO outputs (mostly for debugging)
	err = ppg_config_gpios(spi_ppg, ppg_cs);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	//Exit program mode
	err = ppg_exit_config(spi_ppg, ppg_cs);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }

	//Read from PPG Sensor
	while (err == 0) {
		err = ppg_clear_fifo(spi_ppg, ppg_cs);
		if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    	}
		k_msleep(10);
		err = ppg_read_sensors(spi_ppg, spi_ppg, 20, ppg_cs);
		if (err < 0) {
            LOG_ERR("SPI Read failed (%d)\n", err);
    	}
		k_msleep(SENSOR_SLEEP_MS);
	}
	//If an error occurs, break loop
	LOG_ERR("err = %d\n", err);

	while (1) {
		k_msleep(SENSOR_SLEEP_MS);
	}
}

void calc_thread(void) {
	//Put Algorithm Here
	//TODO: Add method of storing data from PPG sensors to be accessed here
	while (1) {
			k_msleep(60);
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
}

//Initialize the threads
K_THREAD_DEFINE(rd_thread, STACKSIZE, read_thread, NULL, NULL, NULL, 
                READ_THREAD_PRIORITY, 0, 0); 
/*K_THREAD_DEFINE(cal_thread, STACKSIZE, calc_thread, NULL, NULL, NULL, 
                CALC_THREAD_PRIORITY, 0, 0); */
/*K_THREAD_DEFINE(bt_thread, STACKSIZE, BLE_thread, NULL, NULL, NULL, 
                BLE_THREAD_PRIORITY, 0, 0); */
