#ifndef CROSS_CORRELATE_H
#define CROSS_CORRELATE_H

#include <stdbool.h>

void cross_correlate(
    double early_array[],
    double late_array[],
    double return_array[],
    int size,
    bool should_log_result);

#endif