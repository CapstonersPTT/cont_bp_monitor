
#include <stdio.h>
#include <math.h>

//TODO: Can we name some of these interim variables better?


float delta_P(float PTT, float K_a) {
    float frac = pow(PTT, 2);
    float rfrac = 1/frac;
    float r = K_a * rfrac;
    return r;
}

float diastolic_BP(float PTT, float K_a, float K_b, float K_c) {
    float p1 = K_b;
    float p2 = (2/0.031)*log(K_c / PTT);
    float p3 = -1 * K_a / pow(PTT, 2) * 1/3;
    float diastolic = p1 + p2 + p3;
    return diastolic;
}

float systolic_BP(float PTT, float K_a, float K_b, float K_c) {
    float d1 = diastolic_BP(PTT, K_a, K_b, K_c);
    float d2 = delta_P(PTT, K_a);
    return d1 + d2;
}

