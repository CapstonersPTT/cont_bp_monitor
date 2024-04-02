#include <stdio.h>
#include <math.h>

void rolling_window(int array[], int return_array[], int window_size, int size)
{
  int i, j;
  int sum = 0;
  for (i = 0; i < window_size; i++)
  {
    sum += array[i];
  }
  return_array[0] = sum;
  for (i = 1; i < size - window_size; i++)
  {
    sum = sum - array[i - 1] + array[i + window_size - 1];
    return_array[i] = sum;
  }
  return;
}