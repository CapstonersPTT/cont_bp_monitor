#include "sensor_ppg.h"

static uint8_t spi_cmd[3] = {0};
static uint8_t spi_read[4] = {0};
static uint8_t spi_reg; //Register address
static uint8_t spi_rw; //Read (0) / Write (1) bit
static uint16_t spi_value; //Value to write to register
struct spi_buf tx_buf  = {.buf = &spi_cmd, .len = 3};
struct spi_buf_set tx_bufs = {.buffers = &tx_buf, .count = 1};
struct spi_buf rx_buf  = {.buf = &spi_read, .len = 4};
struct spi_buf_set rx_bufs = {.buffers = &rx_buf, .count = 1};

//Start clock, enter program mode
int ppg_start_config(const struct spi_dt_spec spi) {
    int err = 0;

    //enable clock
    spi_reg = 0x4B;
    spi_rw = 0x1;
    spi_value = 0x1340; 
    spi_cmd[0] = spi_reg + (spi_rw << 7);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
	err = spi_write_dt(&spi, &tx_bufs);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}

    //enter program mode
    spi_reg = 0x10;
    spi_rw = 0x1;
    spi_value = 0x0001;
    spi_cmd[0] = spi_reg + (spi_rw << 7);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
	err = spi_write_dt(&spi, &tx_bufs);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}
    return err;
}

int ppg_config_sampling_freq(const struct spi_dt_spec spi, uint16_t freq) {
    int err = 0;

    //Set sampling frequency
    spi_reg = 0x12;
    spi_rw = 0x1;
    spi_value = 32000 / (4 * freq); //freq = 32KHz/(reg value * 4)
    spi_cmd[0] = spi_reg + (spi_rw << 7);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    err = spi_write_dt(&spi, &tx_bufs);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}
    return err;
}

int ppg_config_num_channels(const struct spi_dt_spec spi) {
    int err = 0;

    spi_reg = 0x37;
    spi_rw = 0x1;
    spi_value = 0xfc00; 
    spi_cmd[0] = spi_reg + (spi_rw << 7);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
	err = spi_write_dt(&spi, &tx_bufs);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}
    return err;

    spi_reg = 0x3c;
    spi_rw = 0x1;
    spi_value = 0x31c6; 
    spi_cmd[0] = spi_reg + (spi_rw << 7);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
	err = spi_write_dt(&spi, &tx_bufs);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}
    return err;
}

int ppg_config_fifo(const struct spi_dt_spec spi) { 
    int err = 0;
    //Enable Data hold for slot A and B
    spi_reg = 0x5f;
    spi_rw = 0x1;
    spi_value = 0x0006;
    spi_cmd[0] = spi_reg + (spi_rw << 7);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
	err = spi_write_dt(&spi, &tx_bufs);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}
    return err;
}

int ppg_config_leds(const struct spi_dt_spec spi) {
    int err = 0;

    //Set which LEDs pulse
    spi_reg = 0x14;
    spi_rw = 0x1;
    spi_value = 0x054f;
    spi_cmd[0] = spi_reg + (spi_rw << 7);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
	err = spi_write_dt(&spi, &tx_bufs);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}


    return err;
}

int ppg_config_gpios(const struct spi_dt_spec spi, uint16_t gpios) {
    int err = 0;
    return err;
}

//Exit program mode
int ppg_exit_config(const struct spi_dt_spec spi) {
    int err = 0;
    spi_reg = 0x10;
    spi_rw = 0x1;
    spi_value = 0x0002;
    spi_cmd[0] = spi_reg + (spi_rw << 7);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
	err = spi_write_dt(&spi, &tx_bufs);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}
    return err;
}

int ppg_read_sensors(const struct spi_dt_spec spi, const struct spi_dt_spec spi2, uint16_t num_samples) {
    int err = 0;
    spi_reg = 0x60;
    spi_rw = 0x0;
    spi_value = 0x0000; 
    spi_cmd[0] = spi_reg + (spi_rw << 7);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    err = spi_write_dt(&spi, &tx_bufs);
    err = spi_read_dt(&spi, &rx_bufs);
    if (err < 0) {
			printk("SPI Read failed (%d)\n", err);
			return err;
	}
    //err = spi_read(dev2, &spi_cfg, &rx_bufs);
    return err;
    
}