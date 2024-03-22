/************************************************************************************
 * 	
 * 	@file     sensor_ppg.h
 *  @author   Mario Lanzilotta
 * 	@brief    PPG sensor driver
 *  
************************************************************************************/
#ifndef SENSOR_PPG_H_
#define SENSOR_PPG_H_

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/gpio.h>


int ppg_start_config(const struct spi_dt_spec spi, const struct gpio_dt_spec cs);
int ppg_config_sampling_freq(const struct spi_dt_spec spi, uint16_t freq, const struct gpio_dt_spec cs);
int ppg_config_num_channels(const struct spi_dt_spec spi, const struct gpio_dt_spec cs);
int ppg_config_fifo(const struct spi_dt_spec spi, const struct gpio_dt_spec cs);
int ppg_config_leds(const struct spi_dt_spec spi, const struct gpio_dt_spec cs);
int ppg_config_gpios(const struct spi_dt_spec spi, const struct gpio_dt_spec cs);
int ppg_exit_config(const struct spi_dt_spec spi, const struct gpio_dt_spec cs);
int ppg_read_sensors(const struct spi_dt_spec spi, const struct spi_dt_spec spi2, const struct gpio_dt_spec cs, uint16_t *proximal, uint16_t *distal);
int ppg_software_reset(const struct spi_dt_spec spi, const struct gpio_dt_spec cs);
int ppg_clear_fifo(const struct spi_dt_spec spi, const struct gpio_dt_spec cs);

int ppg_set_LED_drive(const struct spi_dt_spec spi, const struct gpio_dt_spec cs, uint16_t drive);
int ppg_set_TIA_INT_gain(const struct spi_dt_spec spi, const struct gpio_dt_spec cs, uint16_t tia_gain, uint16_t int_gain);
int ppg_set_slot_A(const struct spi_dt_spec spi, const struct gpio_dt_spec cs, uint8_t width, uint8_t offset, uint8_t pulses, uint8_t period);

#endif