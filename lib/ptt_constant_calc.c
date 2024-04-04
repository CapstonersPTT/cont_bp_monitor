
#include <stdio.h>
#include <math.h>

float find_Ka(float PTT, float Sys, float Dia){

    float Ka = pow(PTT, 2) *(Sys - Dia);
    
    return Ka;
}

float find_Kb(float Kc, float PTT, float Sys, float Dia){

    float K_a = find_Ka(PTT, Sys, Dia);
    
    float Kb = Dia - 2/0.031*log(Kc/PTT) + (K_a/pow(PTT, 2))/3;

    return Kb;
}

