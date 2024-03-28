/************************************************************************************
 * 	
 * 	@file     sensor_ppg.c
 *  @author   Mario Lanzilotta
 * 	@brief    PPG sensor driver
 *  
************************************************************************************/
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


/************************************************************************************
 *   
 * @brief Enters program mode and starts the sample clock
 * @param spi devicetree spec of the ppg sensor
 * @param cs devicetree spec of the chip select pin 
 * @returns spi error code
 *  
************************************************************************************/
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

    //enable clock
    spi_reg = 0x4B;
    spi_rw = 0x1;
    spi_value = 0x2692;
    spi_cmd[0] = spi_rw + (spi_reg << 1);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    gpio_pin_set_dt(&cs, 1);
	err = spi_write_dt(&spi, &tx_bufs);
    gpio_pin_set_dt(&cs, 0);

    return err;
}

/************************************************************************************
 *   
 * @brief Sets the sampling frequency of the PPG sensor
 * @param spi devicetree spec of the ppg sensor
 * @param freq desired sampling frequency
 * @param cs devicetree spec of the chip select pin 
 * @returns spi error code
 *  
************************************************************************************/
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
    return err;
}

/************************************************************************************
 *   
 * @brief Sets the ppg sensor to single channel mode
 * @param spi devicetree spec of the ppg sensor
 * @param cs devicetree spec of the chip select pin 
 * @returns spi error code
 *  
************************************************************************************/
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
    return err;
}

/************************************************************************************
 *   
 * @brief Sets FIFO length, and enables time slot A
 * @param spi devicetree spec of the ppg sensor
 * @param cs devicetree spec of the chip select pin 
 * @returns spi error code
 *  
************************************************************************************/
int ppg_config_fifo(const struct spi_dt_spec spi, const struct gpio_dt_spec cs) { 
    int err = 0;
    //Set max FIFO length (in words)
    spi_reg = 0x06;
    spi_rw = 0x1;
    spi_value = 0x2000;
    spi_cmd[0] = spi_rw + (spi_reg << 1);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    gpio_pin_set_dt(&cs, 1);
	err = spi_write_dt(&spi, &tx_bufs);
    gpio_pin_set_dt(&cs, 0);

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
    
    return err;

}

/************************************************************************************
 *   
 * @brief Configures LED settings
 * @param spi devicetree spec of the ppg sensor
 * @param cs devicetree spec of the chip select pin 
 * @returns spi error code
 *  
************************************************************************************/
int ppg_config_leds(const struct spi_dt_spec spi, const struct gpio_dt_spec cs) {
    int err = 0;
    //TODO: add option to adjust timeslot time, pulses, averaging

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

    spi_reg = 0x54;
    spi_rw = 0x1;
    spi_value = 0xc020;
    spi_cmd[0] = spi_rw + (spi_reg << 1);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    gpio_pin_set_dt(&cs, 1);
	err = spi_write_dt(&spi, &tx_bufs);
    gpio_pin_set_dt(&cs, 0);

    return err;
}

/************************************************************************************
 *   
 * @brief Configures GPIOs to provide debug signals
 * @param spi devicetree spec of the ppg sensor
 * @param cs devicetree spec of the chip select pin 
 * @returns spi error code
 *  
************************************************************************************/
int ppg_config_gpios(const struct spi_dt_spec spi, const struct gpio_dt_spec cs) {
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

    spi_reg = 0x0B;
    spi_rw = 0x1;
    spi_value = 0x0501;
    spi_cmd[0] = spi_rw + (spi_reg << 1);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    gpio_pin_set_dt(&cs, 1);
	err = spi_write_dt(&spi, &tx_bufs);
    gpio_pin_set_dt(&cs, 0);
    return err;
}

/************************************************************************************
 *   
 * @brief Exits program mode and starts normal operation
 * @param spi devicetree spec of the ppg sensor
 * @param cs devicetree spec of the chip select pin 
 * @returns spi error code
 *  
************************************************************************************/
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
    return err;
}

/************************************************************************************
 *   
 * @brief Performs a software reset on the PPG sensor
 * @param spi devicetree spec of the ppg sensor
 * @param cs devicetree spec of the chip select pin 
 * @returns spi error code
 *  
************************************************************************************/
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
    return err;
}

/************************************************************************************
 *   
 * @brief Clears the FIFO queue on the PPG sensor
 * @param spi devicetree spec of the ppg sensor
 * @param cs devicetree spec of the chip select pin 
 * @returns spi error code
 *  
************************************************************************************/
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
    return err;
}

/************************************************************************************
 *   
 * @brief Reads data samples from both PPG sensors
 * @param spi devicetree spec of the first ppg sensor
 * @param spi2 devicetree spec of the second ppg sensor
 * @param cs devicetree spec of the chip select pin 
 * @param proximal array of size num_samples to store proximal readings
 * @param distal array of size num_samples to store distal readings
 * @param num_samples number of samples to take from each sensor (size of array to fill)
 * @returns spi error code
 *  
************************************************************************************/
int ppg_read_sensors(const struct spi_dt_spec spi_d, const struct spi_dt_spec spi_p, const struct gpio_dt_spec cs, const struct gpio_dt_spec cs2, uint32_t *distal, uint32_t *proximal, uint16_t num_samples) {
    int err = 0;
    uint16_t sample_count = 0;

    for (sample_count = 0; sample_count < num_samples; sample_count++) {
        //Read one sample from proximal
        spi_reg = 0x70;
        spi_rw = 0x0;
        tx_buf.len = 1; 
        spi_cmd[0] = spi_rw + (spi_reg << 1);
        gpio_pin_set_dt(&cs, 1);
        err = spi_write_dt(&spi_p, &tx_bufs);
        if (err < 0) {
            return err;
        }
        err = spi_read_dt(&spi_p, &rx_bufs);
        if (err < 0) {
            return err;
        }
        gpio_pin_set_dt(&cs, 0);
        proximal[sample_count] = spi_rd[1] + (spi_rd[0] << 8);

        spi_reg = 0x74;
        spi_rw = 0x0;
        tx_buf.len = 1; 
        spi_cmd[0] = spi_rw + (spi_reg << 1);
        gpio_pin_set_dt(&cs, 1);
        err = spi_write_dt(&spi_p, &tx_bufs);
        if (err < 0) {
            return err;
        }
        err = spi_read_dt(&spi_p, &rx_bufs);
        if (err < 0) {
            return err;
        }
        gpio_pin_set_dt(&cs, 0);
        proximal[sample_count] += (spi_rd[1] << 16) + (spi_rd[0] << 24);

        //Read one sample from distal
        spi_reg = 0x70;
        spi_rw = 0x0;
        tx_buf.len = 1; 
        spi_cmd[0] = spi_rw + (spi_reg << 1);
        gpio_pin_set_dt(&cs2, 1);
        err = spi_write_dt(&spi_d, &tx_bufs);
        if (err < 0) {
            return err;
        }
        err = spi_read_dt(&spi_d, &rx_bufs);
        if (err < 0) {
            return err;
        }
        gpio_pin_set_dt(&cs2, 0);
        distal[sample_count] = spi_rd[1] + (spi_rd[0] << 8);

        spi_reg = 0x74;
        spi_rw = 0x0;
        tx_buf.len = 1; 
        spi_cmd[0] = spi_rw + (spi_reg << 1);
        gpio_pin_set_dt(&cs2, 1);
        err = spi_write_dt(&spi_d, &tx_bufs);
        if (err < 0) {
            return err;
        }
        err = spi_read_dt(&spi_d, &rx_bufs);
        if (err < 0) {
            return err;
        }
        gpio_pin_set_dt(&cs2, 0);
        distal[sample_count] += (spi_rd[1] << 16) + (spi_rd[0] << 24);

        k_msleep(1); //TODO: read sample rate to determine sleep time
    }

    return err; 
}

/************************************************************************************
 *   
 * @brief Sets LED drive current
 * @param spi devicetree spec of the ppg sensor
 * @param cs devicetree spec of the chip select pin
 * @param drive LED scale 0 = 10%, 1 = 100%
 * @param coarse Coarse adjustment of LED current, 0-16
 * @returns spi error code
 *  
************************************************************************************/
int ppg_set_LED_drive(const struct spi_dt_spec spi, const struct gpio_dt_spec cs, uint8_t drive, uint8_t coarse) {
    int err = 0;
    tx_buf.len = 3; 
    spi_reg = 0x23;
    spi_rw = 0x1;
    spi_value = 0x1000 + (drive << 13) + coarse;
    spi_cmd[0] = spi_rw + (spi_reg << 1);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    gpio_pin_set_dt(&cs, 1);
    err = spi_write_dt(&spi, &tx_bufs);
    gpio_pin_set_dt(&cs, 0);
    return err;
}

int ppg_set_TIA_INT_gain(const struct spi_dt_spec spi, const struct gpio_dt_spec cs, uint16_t tia_gain, uint16_t int_gain) {
    int err = 0;
    tx_buf.len = 3; 
    spi_reg = 0x42;
    spi_rw = 0x1;
    spi_value = 0x3900 + tia_gain + (int_gain << 7);
    spi_cmd[0] = spi_rw + (spi_reg << 1);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    gpio_pin_set_dt(&cs, 1);
    err = spi_write_dt(&spi, &tx_bufs);
    gpio_pin_set_dt(&cs, 0);
    return err;
}

/************************************************************************************
 *   
 * @brief Adjusts settings for time slot A
 * @param spi devicetree spec of the ppg sensor
 * @param cs devicetree spec of the chip select pin 
 * @param width LED pulse width (us)
 * @param offset time between start of time slot and first pulse (us)
 * @param pulses number of LED pulses per time slot
 * @param period time between LED pulses (us)
 * @returns spi error code
 *  
************************************************************************************/
int ppg_set_slot_A(const struct spi_dt_spec spi, const struct gpio_dt_spec cs, uint8_t width, uint8_t offset, uint8_t pulses, uint8_t period) {
    int err = 0;

    tx_buf.len = 3; 
    spi_reg = 0x30;
    spi_rw = 0x1;
    spi_value = offset + ((width & 0x0F) << 8);
    spi_cmd[0] = spi_rw + (spi_reg << 1);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    gpio_pin_set_dt(&cs, 1);
    err = spi_write_dt(&spi, &tx_bufs);
    gpio_pin_set_dt(&cs, 0);

    spi_reg = 0x31;
    spi_rw = 0x1;
    spi_value = period + (pulses << 8);
    spi_cmd[0] = spi_rw + (spi_reg << 1);
    spi_cmd[1] = (uint8_t) (spi_value >> 8);
    spi_cmd[2] = (uint8_t) spi_value;
    gpio_pin_set_dt(&cs, 1);
    err = spi_write_dt(&spi, &tx_bufs);
    gpio_pin_set_dt(&cs, 0);

    return err;
}