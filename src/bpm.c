/************************************************************************************
 * 	
 * 	@file     bpm.c
 *  @author   Mario Lanzilotta
 * 	@brief    BLE blood pressure service
 *  
************************************************************************************/

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(BLE_bp, LOG_LEVEL_DBG);

static uint8_t bps_blsc;
static uint16_t bp[2];;

static void bp_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value) {
	ARG_UNUSED(attr);

	bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

	LOG_INF("BPS notifications %s", notif_enabled ? "enabled" : "disabled");
}

static ssize_t read_bp(struct bt_conn *conn, const struct bt_gatt_attr *attr,
		       void *buf, uint16_t len, uint16_t offset) {
	
	return bt_gatt_attr_read(conn, attr, buf, len, offset, &bps_blsc,
				 sizeof(bps_blsc));
}

static ssize_t write_bp(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			const void *buf, uint16_t len, uint16_t offset, uint8_t flags) {

	uint8_t *value = attr->user_data;

	if (offset + len > sizeof(bp)) {
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	memcpy(value + offset, buf, len);

	return len;
}

/* Blood Pressure Service Declaration */
BT_GATT_SERVICE_DEFINE(bp_service,
	BT_GATT_PRIMARY_SERVICE(BT_UUID_BPS),
	BT_GATT_CHARACTERISTIC(BT_UUID_GATT_BPM, BT_GATT_CHRC_READ |
			       BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_WRITE,
			       BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
			       read_bp, write_bp, NULL),
	BT_GATT_CCC(bp_ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);


int bp_init(void) {
	bps_blsc = 0x01;

	return 0;
}

int bt_bps_notify(uint16_t sys, uint16_t dia) {	
	int err = 0;

	bp[0] = sys;
	bp[1] = dia;

	err = bt_gatt_notify(NULL, &bp_service.attrs[1], &bp, sizeof(bp));
	LOG_INF("Sent BP notification: %d,%d (%d)", sys, dia, err);

	return err == -ENOTCONN ? 0 : err;
}

SYS_INIT(bp_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
