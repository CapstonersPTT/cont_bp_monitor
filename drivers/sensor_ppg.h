#ifndef SENSOR_PPG_H_
#define SENSOR_PPG_H_

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/drivers/spi.h>

int ppg_start_config(const struct spi_dt_spec spi); //Start clock, enter program mode, config GPIOS
int ppg_config_sampling_freq(const struct spi_dt_spec spi, uint16_t freq);
int ppg_config_num_channels(const struct spi_dt_spec spi);
int ppg_config_fifo(const struct spi_dt_spec spi);
int ppg_config_leds(const struct spi_dt_spec spi);
int ppg_config_gpios(const struct spi_dt_spec spi, uint16_t gpios);
int ppg_exit_config(const struct spi_dt_spec spi); //Exit program mode
int ppg_read_sensors(const struct spi_dt_spec spi, const struct spi_dt_spec spi2, uint16_t num_samples);

#endif