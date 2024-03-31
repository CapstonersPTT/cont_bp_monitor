#include <stdio.h>
#include <math.h>
#include "highest_correlation.h"
#include <zephyr/logging/log.h>

//Initialize Logger
LOG_MODULE_REGISTER(hc, LOG_LEVEL_INF);

static int max_index;
static float max_element;

/**
 * @brief Find how many samples later the late_array is compared to the early_array based on the highest correlation coefficient
 * @param coefficients The array containing the correlation series
 * @param size The size of the coefficients array
 * @return The number of samples delayed from zero index. Negative means the late_array is earlier than the early_array
 */
int highest_correlation(
    float coefficients[],
    uint16_t size)
{
  /** determine index with highest coefficient */
  max_element = coefficients[0];
  max_index = 0;
  for (int j = 1; j < size; j++)
  {
    if (coefficients[j] > max_element)
    {
      max_element = coefficients[j];
      max_index = j;
    }
  }

  /** log the value at the max */
  LOG_DBG("\nThis is the Max_Coefficient: %f", max_element);
  LOG_DBG("\nThis is the Max_Index: %i", max_index);
  
  /** return mindex with max correlation */
  return max_index;
}