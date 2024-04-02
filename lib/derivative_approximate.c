#include <stdio.h>
#include <math.h>

void derivative_approximate(int array[], int return_array[], int delta_samples, int size)
{
  int i;
  for (i = 0; i < size - delta_samples; i++)
  {
    return_array[i] = (array[i + delta_samples] - array[i]);
  }
  return;
}