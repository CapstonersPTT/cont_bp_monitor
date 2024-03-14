#ifndef HIGH_CORRELATION_H
#define HIGH_CORRELATION_H

#include <stdbool.h>

int highest_correlation(
    double coefficients[],
    int size,
    bool should_log_max_value,
    bool should_log_max_index);

#endif