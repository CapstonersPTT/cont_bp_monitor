/************************************************************************************
 * 	
 * 	@file     double_readings.c
 *  @author   Andrew Bristow
 * 	@brief    Interpolates a set of PPG data to increase data resolution
 *  
************************************************************************************/

#include <stdio.h>
#include <math.h>

int offset;

int cubic_interpolation(int y0, int y1, int y2, int y3)
{
  float a0, a1, a2, a3, interpolated_value;
  a0 = y3 - y2 - y0 + y1;
  a1 = y0 - y1 - a0;
  a2 = y2 - y0;
  a3 = y1;
  interpolated_value = a0 * 0.5 * 0.25 + a1 * 0.25 + a2 * 0.5 + a3;
  return (int) interpolated_value;
}

void double_readings(int array[], int return_array[], int size)
{
  int i;

  for (i = 1; i < size - 3; i++)
  {
    offset= (2*i) - 1;
    return_array[offset-1] = array[i];
    return_array[offset] = cubic_interpolation(array[i-1], array[i], array[i+1], array[i+2]);
  }
  return_array[2 * size - 2] = array[size - 1];
  return;
}

