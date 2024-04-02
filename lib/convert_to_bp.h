#ifndef CONVERT_TO_BP_H
#define CONVERT_TO_BP_H

float delta_P(float PTT, float K_a);
float diastolic_BP(float PTT, float K_a, float K_b, float K_c);
float systolic_BP(float PTT, float K_a, float K_b, float K_c);

#endif