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

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
	ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

//Initialize Logger
LOG_MODULE_REGISTER(bp, LOG_LEVEL_DBG);

//Stacksize for threads
#define STACKSIZE 2048
//Thread priorities
#define READ_THREAD_PRIORITY 1
#define CALC_THREAD_PRIORITY 2
#define BLE_THREAD_PRIORITY 3

#define CS_NODE DT_ALIAS(led0)

//Time between sensor reads
#define SENSOR_SLEEP_MS 100

//Data of ADC io-channels specified in devicetree
static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
			     DT_SPEC_AND_COMMA)
};

#define SPI_PPG_OP SPI_OP_MODE_MASTER | SPI_MODE_CPOL | SPI_MODE_CPHA | SPI_WORD_SET(8) | SPI_LINES_SINGLE | SPI_TRANSFER_MSB

static const struct spi_dt_spec spi_ppg = SPI_DT_SPEC_GET(DT_NODELABEL(adpd1801), SPI_PPG_OP, 0);

static const struct gpio_dt_spec cs_test = GPIO_DT_SPEC_GET(CS_NODE, gpios);

//TODO: define array for holding PPG sensor readings

void read_thread(void) {
    int err;
	//Set up PPG sensor
	err = ppg_start_config(spi_ppg, cs_test);
	//Config num channels
	err = ppg_config_num_channels(spi_ppg, cs_test);
	//Config LED settings and time slots
	err = ppg_config_leds(spi_ppg, cs_test);
	//Config FIFO queue
	err = ppg_config_fifo(spi_ppg, cs_test);
	//Config sampling freq
	err = ppg_config_sampling_freq(spi_ppg, 10, cs_test);
	//Exit program mode
	err = ppg_exit_config(spi_ppg, cs_test);

	//Read from PPG Sensor (probably in a loop)
	printk("entered while loop %d\n", err);
	while (err == 0) {
		err = ppg_read_sensors(spi_ppg, spi_ppg, 20, cs_test);
		k_msleep(SENSOR_SLEEP_MS);
	}
	printk("err = %d\n", err);
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
	if (!device_is_ready(cs_test.port)) {
		LOG_ERR("GPIO device is not ready");
		return 0;
	}
	err = gpio_pin_configure_dt(&cs_test, GPIO_OUTPUT_INACTIVE);
	//Configure ADC channels
	printk("Configuring ADC channels\n");
	for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
		if (!adc_is_ready_dt(&adc_channels[i])) {
			printk("ADC controller device %s not ready\n", adc_channels[i].dev->name);
			return 0;
		}

		err = adc_channel_setup_dt(&adc_channels[i]);
		if (err < 0) {
			printk("Could not setup channel #%d (%d)\n", i, err);
			return 0;
		}
	}

	printk("Configuring SPI\n");
    if (!spi_is_ready_dt(&spi_ppg)) {
        printk("SPI device not ready, aborting\n");
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
