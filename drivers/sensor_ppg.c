#include "sensor_ppg.h"

static uint8_t spi_cmd[3] = {0};
static uint8_t spi_rd[2] = {0};
static uint8_t spi_reg; //Register address
static uint8_t spi_rw; //Read (0) / Write (1) bit
static uint16_t spi_value; //Value to write to register
struct spi_buf tx_buf  = {.buf = &spi_cmd, .len = 3};
const struct spi_buf_set tx_bufs = {.buffers = &tx_buf, .count = 1};
struct spi_buf rx_buf  = {.buf = &spi_rd, .len = 2};
const struct spi_buf_set rx_bufs = {.buffers = &rx_buf, .count = 1};

//Start clock, enter program mode
int ppg_start_config(const struct spi_dt_spec spi, const struct gpio_dt_spec cs) {
    int err = 0;

    //enter program mode
    spi_reg = 0x10;
    spi_rw = 0x1;
    spi_value = 0x0001;
    spi_cmd[0] = spi_rw + (spi_reg << 1);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    gpio_pin_set_dt(&cs, 1);
	err = spi_write_dt(&spi, &tx_bufs);
    gpio_pin_set_dt(&cs, 0);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}
    //enable clock
    spi_reg = 0x4B;
    spi_rw = 0x1;
    spi_value = 0x2692; //10010111 0001 0011 1001 0010
    spi_cmd[0] = spi_rw + (spi_reg << 1);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    gpio_pin_set_dt(&cs, 1);
	err = spi_write_dt(&spi, &tx_bufs);
    gpio_pin_set_dt(&cs, 0);
    printk("Data sent: 0x%x 0x%x 0x%x\n", spi_cmd[0], spi_cmd[1],spi_cmd[2]);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
    }
    return err;
}

int ppg_config_sampling_freq(const struct spi_dt_spec spi, uint16_t freq, const struct gpio_dt_spec cs) {
    int err = 0;

    //Set sampling frequency
    spi_reg = 0x12;
    spi_rw = 0x1;
    spi_value = 32000 / (4 * freq); //freq = 32KHz/(reg value * 4)
    spi_cmd[0] = spi_rw + (spi_reg << 1);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    gpio_pin_set_dt(&cs, 1);
    err = spi_write_dt(&spi, &tx_bufs);
    gpio_pin_set_dt(&cs, 0);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}
    return err;
}

int ppg_config_num_channels(const struct spi_dt_spec spi, const struct gpio_dt_spec cs) {
    int err = 0;
    //Power down all channels except 1
    spi_reg = 0x3c;
    spi_rw = 0x1;
    spi_value = 0x31c6; //0011000111000110
    spi_cmd[0] = spi_rw + (spi_reg << 1);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    gpio_pin_set_dt(&cs, 1);
	err = spi_write_dt(&spi, &tx_bufs);
    gpio_pin_set_dt(&cs, 0);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}
    return err;
}

int ppg_config_fifo(const struct spi_dt_spec spi, const struct gpio_dt_spec cs) { 
    int err = 0;
    //Enable Data hold for slot A and B
    /*
    spi_reg = 0x5f;
    spi_rw = 0x1;
    spi_value = 0x0006;
    spi_cmd[0] = spi_rw + (spi_reg << 1);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    gpio_pin_set_dt(&cs, 1);
	err = spi_write_dt(&spi, &tx_bufs);
    gpio_pin_set_dt(&cs, 0);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}
    */
    //Set max FIFO length (in words)
    spi_reg = 0x06;
    spi_rw = 0x1;
    spi_value = 0x0800;
    spi_cmd[0] = spi_rw + (spi_reg << 1);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    gpio_pin_set_dt(&cs, 1);
	err = spi_write_dt(&spi, &tx_bufs);
    gpio_pin_set_dt(&cs, 0);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}
    //Determines how data is written to FIFO
    //Also enables time slot A
    spi_reg = 0x11;
    spi_rw = 0x1;
    spi_value = 0x1005;
    spi_cmd[0] = spi_rw + (spi_reg << 1);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    gpio_pin_set_dt(&cs, 1);
	err = spi_write_dt(&spi, &tx_bufs);
    gpio_pin_set_dt(&cs, 0);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}
    
    return err;

}

int ppg_config_leds(const struct spi_dt_spec spi, const struct gpio_dt_spec cs) {
    int err = 0;

    //Set which LEDs pulse
    spi_reg = 0x14;
    spi_rw = 0x1;
    spi_value = 0x0545;
    spi_cmd[0] = spi_rw + (spi_reg << 1);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    gpio_pin_set_dt(&cs, 1);
	err = spi_write_dt(&spi, &tx_bufs);
    gpio_pin_set_dt(&cs, 0);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}

    spi_reg = 0x54;
    spi_rw = 0x1;
    spi_value = 0xc020;
    spi_cmd[0] = spi_rw + (spi_reg << 1);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    gpio_pin_set_dt(&cs, 1);
	err = spi_write_dt(&spi, &tx_bufs);
    gpio_pin_set_dt(&cs, 0);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}

    return err;
}

int ppg_config_gpios(const struct spi_dt_spec spi, uint16_t gpios, const struct gpio_dt_spec cs) {
    int err = 0;

    //Config Interrupts
    spi_reg = 0x01;
    spi_rw = 0x1;
    spi_value = 0x00FF;
    spi_cmd[0] = spi_rw + (spi_reg << 1);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    gpio_pin_set_dt(&cs, 1);
	err = spi_write_dt(&spi, &tx_bufs);
    gpio_pin_set_dt(&cs, 0);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}
    //Config GPIO pins
    spi_reg = 0x02;
    spi_rw = 0x1;
    spi_value = 0x0004;
    spi_cmd[0] = spi_rw + (spi_reg << 1);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    gpio_pin_set_dt(&cs, 1);
	err = spi_write_dt(&spi, &tx_bufs);
    gpio_pin_set_dt(&cs, 0);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}

    spi_reg = 0x0B;
    spi_rw = 0x1;
    spi_value = 0x0513;
    spi_cmd[0] = spi_rw + (spi_reg << 1);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    gpio_pin_set_dt(&cs, 1);
	err = spi_write_dt(&spi, &tx_bufs);
    gpio_pin_set_dt(&cs, 0);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}
    return err;
}

//Exit program mode
int ppg_exit_config(const struct spi_dt_spec spi, const struct gpio_dt_spec cs) {
    int err = 0;
    spi_reg = 0x10;
    spi_rw = 0x1;
    spi_value = 0x0002;
    spi_cmd[0] = spi_rw + (spi_reg << 1);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    gpio_pin_set_dt(&cs, 1);
	err = spi_write_dt(&spi, &tx_bufs);
    gpio_pin_set_dt(&cs, 0);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}
    return err;
}

int ppg_software_reset(const struct spi_dt_spec spi, const struct gpio_dt_spec cs) {
    int err = 0;
    tx_buf.len = 3; 
    spi_reg = 0x0F;
    spi_rw = 0x1;
    spi_value = 0x0001;
    spi_cmd[0] = spi_rw + (spi_reg << 1);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    gpio_pin_set_dt(&cs, 1);
	err = spi_write_dt(&spi, &tx_bufs);
    gpio_pin_set_dt(&cs, 0);
    if (err < 0) {
			printk("SPI Write failed (%d)\n", err);
			return err;
	}
    return err;
}

int ppg_clear_fifo(const struct spi_dt_spec spi, const struct gpio_dt_spec cs) {
    int err = 0;
    tx_buf.len = 3; 
    spi_reg = 0x00;
    spi_rw = 0x1;
    spi_value = 0x801F;
    spi_cmd[0] = spi_rw + (spi_reg << 1);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    gpio_pin_set_dt(&cs, 1);
    err = spi_write_dt(&spi, &tx_bufs);
    gpio_pin_set_dt(&cs, 0);
    if (err < 0) {
            printk("SPI Write failed (%d)\n", err);
            return err;
    }
    return err;
}

int ppg_read_sensors(const struct spi_dt_spec spi, const struct spi_dt_spec spi2, uint16_t num_samples, const struct gpio_dt_spec cs) {
    int err = 0;
    uint16_t samples_in_queue = 0;
    uint16_t sample_count = 0;

    //Read num_samples samples
    while (sample_count < num_samples) {
        //check how much data is in queue
        spi_reg = 0x00;
        spi_rw = 0x0;
        tx_buf.len = 1; 
        spi_cmd[0] = spi_rw + (spi_reg << 1);
        gpio_pin_set_dt(&cs, 1);
        err = spi_write_dt(&spi, &tx_bufs);
        err = spi_read_dt(&spi, &rx_bufs);
        gpio_pin_set_dt(&cs, 0);
        if (err < 0) {
                printk("SPI Read failed (%d)\n", err);
                return err;
        }
        samples_in_queue = (spi_rd[0] / 2);
        printk("Num Samples in FIFO queue: 0x%d\n", num_samples);

        //If number of samples in queue is more than we need, only read enough samples to return num_samples
        if (samples_in_queue > (num_samples - sample_count)) {
            samples_in_queue = (num_samples - sample_count);
        }
        
        for (int i = 0; i < samples_in_queue; i++) {
            //Read one sample from FIFO queue
            spi_reg = 0x60;
            spi_rw = 0x0;
            tx_buf.len = 1; 
            spi_cmd[0] = spi_rw + (spi_reg << 1);
            gpio_pin_set_dt(&cs, 1);
            err = spi_write_dt(&spi, &tx_bufs);
            err = spi_read_dt(&spi, &rx_bufs);
            gpio_pin_set_dt(&cs, 0);
            if (err < 0) {
                    printk("SPI Read failed (%d)\n", err);
                    return err;
            }
            sample_count++;
            printk("Data from SPI read: 0x%02x%02x\n", spi_rd[0], spi_rd[1]);
        }
        k_msleep(1);
    }
    return err; 
}