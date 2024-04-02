cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(cont_bp_monitor)

target_sources(app PRIVATE src/main.c src/bpm.c drivers/sensor_ppg.c lib/cross_correlate.c lib/convert_to_bp.c lib/derivative_approximate.c lib/double_readings.c lib/rolling_window.c)
target_include_directories(app PRIVATE drivers lib)
