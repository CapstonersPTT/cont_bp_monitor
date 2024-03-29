#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "cross_correlate.h"
#include <zephyr/logging/log.h>

//Initialize Logger
LOG_MODULE_REGISTER(cc, LOG_LEVEL_DBG);

int i, j, max_delay, coefficient_index, delay;
double mean_early, mean_late, standard_deviation_1, standard_deviation_2, numerator, denominator, correlation_coefficient;

/**
 * @brief Cross correlate the early_array with the late_array
 * @param early_array The array that would be from the proximal sensor
 * @param late_array The array that would be from the distal sensor
 * @param return_array The array that will be filled with the correlation coefficients
 * @param size The size of the smaller of the 2 arrays
 * @param should_log_result Whether or not to log the sxy, result, and delay
 * @return void because the return_array is passed by reference
 */
void cross_correlate(
    uint32_t early_array[],
    uint32_t late_array[],
    double return_array[],
    uint16_t size,
    bool should_log_result)
{
  mean_early = 0;
  mean_late = 0;

  for (i = 0; i < size; i++)
  {
    mean_early += ((double) early_array[i]);
    mean_late += ((double) late_array[i]);
  }
  mean_early /= size;
  mean_late /= size;

  /* Calculate the denominator */
  standard_deviation_1 = 0;
  standard_deviation_2 = 0;
  for (i = 0; i < size; i++)
  {
    standard_deviation_1 += ((double) early_array[i] - mean_early) * ((double) early_array[i] - mean_early);
    standard_deviation_2 += ((double) late_array[i] - mean_late) * ((double) late_array[i] - mean_late);
  }
  denominator = sqrt(standard_deviation_1 * standard_deviation_2);

  /* Calculate the correlation series */
  max_delay = size;
  coefficient_index = 0;
  for (delay = -max_delay; delay < max_delay; delay++)
  {
    numerator = 0;
    for (i = 0; i < size; i++)
    {
      int j = i + delay;
      if (j < 0 || j >= size) {
        continue;
      }
      else {
        numerator += ((double) early_array[i] - mean_early) * ((double) late_array[j] - mean_late);
      }
    }

    correlation_coefficient = numerator / denominator;
    return_array[coefficient_index] = correlation_coefficient;
    coefficient_index++;

    /** optionally log the sxy, result, and delay */
    if (should_log_result)
    {
      LOG_DBG("sxy %10f ", numerator);
      LOG_DBG("\tresult %10f", correlation_coefficient);
      LOG_DBG("\tdelay %i\n", delay);
    }
  }
}