#include <stdio.h>
#include <math.h>
#include "highest_correlation.h"
#include <zephyr/logging/log.h>

//Initialize Logger
LOG_MODULE_REGISTER(hc, LOG_LEVEL_DBG);

int max_index;
double max_element;

/**
 * @brief Find how many samples later the late_array is compared to the early_array based on the highest correlation coefficient
 * @param coefficients The array containing the correlation series
 * @param size The size of the smaller of the 2 arrays
 * @param should_log_max_value Whether or not to log the max value
 * @param should_log_max_index Whether or not to log the max index
 * @return The number of samples delayed from zero index. Negative means the late_array is earlier than the early_array
 */
int highest_correlation(
    double coefficients[],
    uint16_t size,
    bool should_log_max_value,
    bool should_log_max_index)
{
  /** dynamic memory allocation for the correlation series */

  /** determine index with highest coefficient */
  max_element = coefficients[0];
  max_index = 0;
  for (int j = 1; j < 2 * size; j++)
  {
    if (coefficients[j] > max_element)
    {
      max_element = coefficients[j];
      max_index = j;
    }
  }

  /** optionally log the value at the max */
  if (should_log_max_value)
  {
    LOG_DBG("\nThis is the Max_Coefficient: %f", max_element);
  }

  if (should_log_max_index)
  {
    LOG_DBG("\nThis is the Max_Index: %i", max_index - size);
  }

  /** return with zero index correction */
  return max_index - size;
}