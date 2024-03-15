/** @file
 *  @brief bp Service sample
 */

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

static uint8_t bps;
static uint8_t bp_update;

static void bp_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value) {
	ARG_UNUSED(attr);

	bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

	LOG_INF("BPS notifications %s", notif_enabled ? "enabled" : "disabled");
}

static ssize_t read_bp(struct bt_conn *conn, const struct bt_gatt_attr *attr,
		       void *buf, uint16_t len, uint16_t offset) {
	const char *value = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 sizeof(bps));
}

static ssize_t write_bp(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			const void *buf, uint16_t len, uint16_t offset,
			uint8_t flags) {
	uint8_t *value = attr->user_data;

	if (offset + len > sizeof(bps)) {
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	memcpy(value + offset, buf, len);
	bp_update = 1U;

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
	bps = 0x01;

	return 0;
}

int bt_bps_notify(uint16_t pressure) {	
	int err;
	if (!bp_update) {
		return 0;
	}

	bp_update = 0U;
	bt_gatt_notify(NULL, &bp_service.attrs[1], &bps, sizeof(bps));

	return err == -ENOTCONN ? 0 : err;
}

SYS_INIT(bp_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
