#ifndef CROSS_CORRELATE_H
#define CROSS_CORRELATE_H

#include <stdbool.h>

void cross_correlate(
    uint32_t early_array[],
    uint32_t late_array[],
    double return_array[],
    uint16_t size,
    bool should_log_result);

#endif