/************************************************************************************
 * 	
 * 	@file     cross_correlate.c
 *  @author   Tej Vyas, Kayshi Shankar, Sam Burke, Andrew Bristow
 * 	@brief    Performs a cross correlation on the PPG data and returns the index
 *            with the highest correlation
 *  
************************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <math.h>

int i, j, correlation_index, delay, sum_early, sum_late;
float mean_early, mean_late, standard_deviation_1, standard_deviation_2, numerator, denominator, correlation_coefficient, max_correlation;

/**
 * @brief Cross correlate the early_array with the late_array
 * @param early_array The array that would be from the proximal sensor
 * @param late_array The array that would be from the distal sensor
 * @param size The size of the smaller of the 2 arrays
 * @param max_reasonable_ptt The maximum reasonable pulse transit time
 * @return max_index
 */
int cross_correlate(
    int early_array[],
    int late_array[],
    int size,
    int max_reasonable_ptt)
{
  mean_early = 0;
  mean_late = 0;
  max_correlation = -1;

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
  for (delay = 0; delay < max_reasonable_ptt; delay++)
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
    if (correlation_coefficient > max_correlation)
    {
      max_correlation = correlation_coefficient;
      correlation_index = delay;
    }
  }
  return correlation_index;
}