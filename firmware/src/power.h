// Power management and sleep mode.
//
// Awake when BLE is connected, the app is foreground (session keep-alive), and
// OTA is not in progress. Otherwise the device enters sleep mode: status LED
// breathes, BLE keeps advertising, LiDAR rail and UART are off.

#pragma once

#include <Arduino.h>

namespace ge {

class Power {
public:
    static void begin();

    static void setBleConnected(bool connected);
    static void setAppKeepAwake(bool keepAwake);

    static bool appKeepAwake();
    static bool isAwake();
    static bool isSleeping();

    static void tick();
};

} // namespace ge
