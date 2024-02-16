#ifndef SENSOR_PPG_H_
#define SENSOR_PPG_H_

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/drivers/spi.h>

static int ppg_start_config(const struct device *dev); //Start clock, enter program mode, config GPIOS
static int ppg_config_sampling_freq(const struct device *dev, uint16_t freq);
static int ppg_config_num_channels(const struct device *dev);
static int ppg_config_fifo(const struct device *dev);
static int ppg_config_leds(const struct device *dev);
static int ppg_exit_config(const struct device *dev); //Exit program mode
static int ppg_read_sensors(const struct device *dev, const struct device *dev2, uint16_t num_samples);

#endif