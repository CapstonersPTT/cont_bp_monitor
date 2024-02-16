# Continuous Blood Pressure Monitor
Author  : Mario Lanzilotta

## Table of Contents
1. [About the Project](#about-the-project)
2. [Project Status](#project-status)
3. [Getting Started](#getting-started)

### About the Project
Continuous Blood Pressure Monitor firmware for nrf52840 DK. The firmware handles reading from external PPG sensors via SPI, calculating the users blood pressure, and sending the blood pressure data over BLE to a mobile device.

### Project Status
In progess

This branch contains code for communication with the external PPG sensor via SPI. Once algorithm development with the 
test signals is finished, this code will be merged into the main branch. 

### Getting Started
nRF Connect SDK Toolchain version: 2.5.2

Board: nrf52840dk_nrf52840

Configuration: prj.conf
