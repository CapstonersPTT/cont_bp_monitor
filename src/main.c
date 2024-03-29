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
#include "bpm.h"
#include "../lib/cross_correlate.h"
#include "../lib/highest_correlation.h"

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/settings/settings.h>

//Initialize Logger
LOG_MODULE_REGISTER(bp, LOG_LEVEL_DBG);

//Stacksize for threads
#define STACKSIZE 2048

//Size of proximal and distal arrays, number of samples taken per cycle
#define PPG_ARRAY_SIZE 2000
//Sampling rate of PPG sensors (in Hz)
#define PPG_SAMPLE_RATE 1000
//Time between sensor read cycles
#define SENSOR_SLEEP_MS 5000

//Thread priorities
#define READ_THREAD_PRIORITY 1
#define CALC_THREAD_PRIORITY 2

#define CS_NODE DT_ALIAS(ppgcs)
#define CS_NODE2 DT_ALIAS(ppgcs2)

#define SPI_PPG_OP SPI_OP_MODE_MASTER | SPI_MODE_CPOL | SPI_MODE_CPHA | SPI_WORD_SET(8) | SPI_LINES_SINGLE | SPI_TRANSFER_MSB 

static const struct spi_dt_spec spi_ppg_dist = SPI_DT_SPEC_GET(DT_NODELABEL(adpd1801), SPI_PPG_OP, 0);
static const struct spi_dt_spec spi_ppg_prox = SPI_DT_SPEC_GET(DT_NODELABEL(adpd18012), SPI_PPG_OP, 0);

static const struct gpio_dt_spec ppg_csd = GPIO_DT_SPEC_GET(CS_NODE, gpios);
static const struct gpio_dt_spec ppg_csp = GPIO_DT_SPEC_GET(CS_NODE2, gpios);

static uint32_t proximal[PPG_ARRAY_SIZE];
static uint32_t distal[PPG_ARRAY_SIZE];
static double coefficients[PPG_ARRAY_SIZE * 2];

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL,
		      BT_UUID_16_ENCODE(BT_UUID_BPS_VAL)),
};

void mtu_updated(struct bt_conn *conn, uint16_t tx, uint16_t rx)
{
	printk("Updated MTU: TX: %d RX: %d bytes\n", tx, rx);
}

static struct bt_gatt_cb gatt_callbacks = {
	.att_mtu_updated = mtu_updated
};

static void bt_ready(void)
{
	int err;

	printk("Bluetooth initialized\n");

	if (IS_ENABLED(CONFIG_SETTINGS)) {
		settings_load();
	}

	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}

	printk("Advertising successfully started\n");
}

static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printk("Passkey for %s: %06u\n", addr, passkey);
}

static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printk("Pairing cancelled: %s\n", addr);
}

static struct bt_conn_auth_cb auth_cb_display = {
	.passkey_display = auth_passkey_display,
	.passkey_entry = NULL,
	.cancel = auth_cancel,
};

void read_thread(void) {
	LOG_INF("Entering read_thread\n");
	int err;

	//Reset PPG sensor
	err = ppg_software_reset(spi_ppg_dist, ppg_csd);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	err = ppg_software_reset(spi_ppg_prox, ppg_csp);
	if (err < 0) {
            LOG_ERR("SPI2 Write failed (%d)\n", err);
    }
	//Set up PPG sensor
	err = ppg_start_config(spi_ppg_dist, ppg_csd);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	err = ppg_start_config(spi_ppg_prox, ppg_csp);
	if (err < 0) {
            LOG_ERR("SPI2 Write failed (%d)\n", err);
    }
	//Config num channels
	err = ppg_config_num_channels(spi_ppg_dist, ppg_csd);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	err = ppg_config_num_channels(spi_ppg_prox, ppg_csp);
	if (err < 0) {
            LOG_ERR("SPI2 Write failed (%d)\n", err);
    }
	//Config LED settings and time slots
	err = ppg_config_leds(spi_ppg_dist, ppg_csd);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	err = ppg_config_leds(spi_ppg_prox, ppg_csp);
	if (err < 0) {
            LOG_ERR("SPI2 Write failed (%d)\n", err);
    }
	//Config FIFO queue
	err = ppg_config_fifo(spi_ppg_dist, ppg_csd);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	err = ppg_config_fifo(spi_ppg_prox, ppg_csp);
	if (err < 0) {
            LOG_ERR("SPI2 Write failed (%d)\n", err);
    }
	//Config sampling freq
	err = ppg_config_sampling_freq(spi_ppg_dist, PPG_SAMPLE_RATE, ppg_csd);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	err = ppg_config_sampling_freq(spi_ppg_prox, PPG_SAMPLE_RATE, ppg_csp);
	if (err < 0) {
            LOG_ERR("SPI2 Write failed (%d)\n", err);
    }
	//Config GPIO outputs (mostly for debugging)
	err = ppg_config_gpios(spi_ppg_dist, ppg_csd);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	err = ppg_config_gpios(spi_ppg_prox, ppg_csp);
	if (err < 0) {
            LOG_ERR("SPI2 Write failed (%d)\n", err);
    }
	//Extra configs for sensor tuning
	//TODO: add calibration loop for LED drive
	err = ppg_set_LED_drive(spi_ppg_dist, ppg_csd, 0, 3);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	err = ppg_set_LED_drive(spi_ppg_prox, ppg_csp, 0, 3);
	if (err < 0) {
            LOG_ERR("SPI2 Write failed (%d)\n", err);
    }
	err = ppg_set_slot_A(spi_ppg_dist, ppg_csd, 0x04, 0x20, 0x02, 0x14);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	err = ppg_set_slot_A(spi_ppg_prox, ppg_csp, 0x04, 0x20, 0x02, 0x14);
	if (err < 0) {
            LOG_ERR("SPI2 Write failed (%d)\n", err);
    }
	//Exit program mode
	err = ppg_exit_config(spi_ppg_dist, ppg_csd);
	if (err < 0) {
            LOG_ERR("SPI Write failed (%d)\n", err);
    }
	err = ppg_exit_config(spi_ppg_prox, ppg_csp);
	if (err < 0) {
            LOG_ERR("SPI2 Write failed (%d)\n", err);
    }

	LOG_INF("Starting read loop\n");
	//Read from PPG Sensor
	while (err == 0) {
		err = ppg_read_sensors(spi_ppg_dist, spi_ppg_prox, ppg_csd, ppg_csp, proximal, distal , PPG_ARRAY_SIZE);
		if (err < 0) {
            LOG_ERR("SPI Read failed (%d)\n", err);
    	}

		for (int i = 0; i < PPG_ARRAY_SIZE; i++) {
			printf("%d,", proximal[i]);
		}
		printf("\n.\n");
		for (int i = 0; i < PPG_ARRAY_SIZE; i++) {
			printf("%d,", distal[i]);
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
	//TODO: Finish Algorithm
	uint16_t samples_delayed;
	double ptt;
	while (1) {
		//Cross correlate the proximal and distal data
		cross_correlate(proximal, distal, coefficients, PPG_ARRAY_SIZE, false);

		//find how many samples later the pulse wave arrived at distal location
		samples_delayed = highest_correlation(coefficients, PPG_ARRAY_SIZE, true, true);

		//calculate the ptt based on the sample rate and peak location
		ptt = (double)samples_delayed / PPG_SAMPLE_RATE;

		LOG_INF("\n\nPTT: %f\n", ptt);

		bt_bps_notify(samples_delayed, 1);
		k_msleep(SENSOR_SLEEP_MS);
	}
}

int main(void)
{
	int err;
	//Configure GPIOS
	LOG_INF("Configuring GPIO pins\n");
	if (!device_is_ready(ppg_csd.port)) {
		LOG_ERR("GPIO device is not ready");
		return 0;
	}
	err = gpio_pin_configure_dt(&ppg_csd, GPIO_OUTPUT_INACTIVE);
	if (!device_is_ready(ppg_csp.port)) {
		LOG_ERR("GPIO device is not ready");
		return 0;
	}
	err = gpio_pin_configure_dt(&ppg_csp, GPIO_OUTPUT_INACTIVE);

	//Configure SPI
	LOG_INF("Configuring SPI\n");
    if (!spi_is_ready_dt(&spi_ppg_dist)) {
        LOG_ERR("SPI device not ready, aborting\n");
		return 0;
	}
	if (!spi_is_ready_dt(&spi_ppg_prox)) {
        LOG_ERR("SPI device not ready, aborting\n");
		return 0;
	}

	//Start BLE advertising
	err = bt_enable(NULL);
	if (err) {
		LOG_ERR("Bluetooth init failed (err %d)\n", err);
		return 0;
	}

	bt_ready();
	bt_gatt_cb_register(&gatt_callbacks);
	bt_conn_auth_cb_register(&auth_cb_display);
}

//Initialize the threads
K_THREAD_DEFINE(rd_thread, STACKSIZE, read_thread, NULL, NULL, NULL, 
                READ_THREAD_PRIORITY, 0, 0); 
K_THREAD_DEFINE(cal_thread, STACKSIZE, calc_thread, NULL, NULL, NULL, 
                CALC_THREAD_PRIORITY, 0, 0);


