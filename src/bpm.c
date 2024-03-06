/** @file
 *  @brief bp Service sample
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/kernel.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

static uint8_t bp[10];
static uint8_t bp_update;

static void bp_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value) {
	// TODO: Handle value
}

static ssize_t read_bp(struct bt_conn *conn, const struct bt_gatt_attr *attr,
		       void *buf, uint16_t len, uint16_t offset) {
	const char *value = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 sizeof(bp));
}

static ssize_t write_bp(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			const void *buf, uint16_t len, uint16_t offset,
			uint8_t flags) {
	uint8_t *value = attr->user_data;

	if (offset + len > sizeof(bp)) {
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
			       read_bp, write_bp, bp),
	BT_GATT_CCC(bp_ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);


void bp_init(void) {

}

void bp_notify(void) {	
	if (!bp_update) {
		return;
	}

	bp_update = 0U;
	bt_gatt_notify(NULL, &bp_service.attrs[1], &bp, sizeof(bp));
}
