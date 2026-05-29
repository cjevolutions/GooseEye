// LiDAR supply rail control via MOSFET switch module (high-side on VCC).
//
// Sleep mode cuts sensor power; wake restores 5V, waits for boot, then UART.

#pragma once

#include <Arduino.h>

namespace ge {

class LidarPower {
public:
    static void begin();
    static void setPowered(bool on);
    static bool isPowered();
};

} // namespace ge
