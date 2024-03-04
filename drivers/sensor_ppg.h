#ifndef SENSOR_PPG_H_
#define SENSOR_PPG_H_

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>

int ppg_start_config(const struct spi_dt_spec spi, const struct gpio_dt_spec cs); //Start clock, enter program mode, config GPIOS
int ppg_config_sampling_freq(const struct spi_dt_spec spi, uint16_t freq, const struct gpio_dt_spec cs);
int ppg_config_num_channels(const struct spi_dt_spec spi, const struct gpio_dt_spec cs);
int ppg_config_fifo(const struct spi_dt_spec spi, const struct gpio_dt_spec cs);
int ppg_config_leds(const struct spi_dt_spec spi, const struct gpio_dt_spec cs);
int ppg_config_gpios(const struct spi_dt_spec spi, uint16_t gpios, const struct gpio_dt_spec cs);
int ppg_exit_config(const struct spi_dt_spec spi, const struct gpio_dt_spec cs); //Exit program mode
int ppg_read_sensors(const struct spi_dt_spec spi, const struct spi_dt_spec spi2, uint16_t num_samples, const struct gpio_dt_spec cs);
int ppg_software_reset(const struct spi_dt_spec spi, const struct gpio_dt_spec cs);

#endif