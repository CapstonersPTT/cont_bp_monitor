#include "sensor_ppg.h"

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

//Start clock, enter program mode, config GPIOS
int ppg_start_config(const struct device *dev) {
    int err;
    spi_cmd = 0xcb40; //enable clock
	err = spi_write(dev, &spi_cfg, &tx_bufs);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}
	spi_cmd = 0x0109; //enter program mode
	err = spi_write(dev, &spi_cfg, &tx_bufs);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}
    return err;
}

int ppg_config_sampling_freq(const struct device *dev, uint16_t freq) {
    int err;
    //freq = 32KHz/(value * 4)
    uint16_t value = 32000 / (4 * freq);
    //Register 0x12, write, value
    spi_cmd = 0x92 + (value << 8);
    err = spi_write(dev, &spi_cfg, &tx_bufs);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}
    return err;
}

int ppg_config_num_channels(const struct device *dev) {
    int err;
    return err;
}

int ppg_config_fifo(const struct device *dev) { 
    int err;
    return err;
}

int ppg_config_leds(const struct device *dev) {
    int err;
    return err;
}

int ppg_config_gpios(const struct device *dev, uint16_t gpios) {
    int err;
    return err;
}

//Exit program mode
int ppg_exit_config(const struct device *dev) {
    int err;
    spi_cmd = 0x0209; //start normal operation
	err = spi_write(dev, &spi_cfg, &tx_bufs);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}
    return err;
}

int ppg_read_sensors(const struct device *dev, const struct device *dev2, uint16_t num_samples) {
    int err;
    err = spi_read(dev, &spi_cfg, &rx_bufs);
    if (err < 0) {
			printk("SPI Read failed (%d)\n", err);
			return err;
	}
    //err = spi_read(dev2, &spi_cfg, &rx_bufs);
    return err;
}