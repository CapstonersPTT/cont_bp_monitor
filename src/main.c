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

#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
	!DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

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

//Time between sensor reads
#define SENSOR_SLEEP_MS 100

//Data of ADC io-channels specified in devicetree
static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
			     DT_SPEC_AND_COMMA)
};

//SPI config values
static const struct spi_config spi_cfg = {
    .frequency = DT_PROP(DT_NODELABEL(spi0), clock_frequency),
    .operation = SPI_OP_MODE_MASTER | SPI_MODE_CPOL | SPI_MODE_CPHA | SPI_WORD_SET(8) | SPI_LINES_SINGLE,
    .cs = SPI_CS_CONTROL_INIT(DT_NODELABEL(adpd1801), 0),
};

static uint16_t spi_cmd = 0x0000;
struct spi_buf tx_buf  = {.buf = &spi_cmd, .len = 4};
struct spi_buf_set tx_bufs = {.buffers = &tx_buf, .count = 1};
struct spi_buf rx_buf  = {.buf = &spi_cmd, .len = 4};
struct spi_buf_set rx_bufs = {.buffers = &rx_buf, .count = 1};
const struct device *spi;

//TODO: define array for holding PPG sensor readings

void read_thread(void) {
    int err;
	//Set up PPG sensor
	spi_cmd = 0xcb40; //enable clock
	err = spi_write(spi, &spi_cfg, &tx_bufs);
	spi_cmd = 0x0109; //enter program mode
	err = spi_write(spi, &spi_cfg, &tx_bufs);
	//TODO: look at registers for configuration
	//Config time slots
	//Config num channels
	//Config LED settings?
	//Config FIFO queue
	//Config sampling freq
	
	spi_cmd = 0x0209; //start normal operation
	err = spi_write(spi, &spi_cfg, &tx_bufs);

	//Read from PPG Sensor (probably in a loop)
	while (1) {
		spi_read(spi, &spi_cfg, &rx_bufs);

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

	spi = device_get_binding("SPI_0");
    if (!device_is_ready(spi)) {
        printk("SPI device not ready, aborting test");
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
