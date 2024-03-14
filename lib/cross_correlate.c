#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "cross_correlate.h"

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
    double early_array[],
    double late_array[],
    double return_array[],
    int size,
    bool should_log_result)
{
  mean_early = 0;
  mean_late = 0;

  for (i = 0; i < size; i++)
  {
    mean_early += (early_array[i]);
    mean_late += (late_array[i]);
  }
  mean_early /= size;
  mean_late /= size;

  /* Calculate the denominator */
  standard_deviation_1 = 0;
  standard_deviation_2 = 0;
  for (i = 0; i < size; i++)
  {
    standard_deviation_1 += (early_array[i] - mean_early) * (early_array[i] - mean_early);
    standard_deviation_2 += (late_array[i] - mean_late) * (late_array[i] - mean_late);
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
      if (j < 0 || j >= size)
        continue;
      else
        numerator += (early_array[i] - mean_early) * (late_array[j] - mean_late);
    }

    correlation_coefficient = numerator / denominator;
    return_array[coefficient_index] = correlation_coefficient;
    coefficient_index++;

    /** optionally log the sxy, result, and delay */
    if (should_log_result)
    {
      printf("sxy %10f ", numerator);
      printf("\tresult %10f", correlation_coefficient);
      printf("\tdelay %i\n", delay);
    }
  }
}