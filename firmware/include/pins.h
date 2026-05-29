// Goose Eye hardware pin assignments (Seeed XIAO ESP32-S3).
//
// Use silkscreen labels (D1, D9, …) in code — raw GPIO numbers do not match
// the printed pin names on this board.

#pragma once

#include <Arduino.h>

// TFmini UART (HardwareSerial 1 @ 115200)
#define GE_LIDAR_TX_PIN D9   // GPIO8 — ESP TX -> sensor RX (white)
#define GE_LIDAR_RX_PIN D10  // GPIO9 — ESP RX <- sensor TX (green)

// High-side MOSFET module IN / control (active-high: HIGH = LiDAR powered)
#ifndef GE_LIDAR_POWER_PIN
#define GE_LIDAR_POWER_PIN D1  // GPIO2
#endif

#ifndef GE_LIDAR_POWER_ACTIVE_HIGH
#define GE_LIDAR_POWER_ACTIVE_HIGH 1
#endif

// Delay after enabling LiDAR rail before UART starts (TFmini boot time)
#define GE_LIDAR_POWER_ON_MS  500
#define GE_LIDAR_POWER_OFF_MS 10
