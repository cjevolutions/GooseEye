// Built-in status LED (XIAO ESP32-S3 user LED on GPIO21).
//
// Awake: off. Sleeping: fade up, hold, fade down, hold off, repeat (0–100% PWM).

#pragma once

#include <Arduino.h>

namespace ge {

class StatusLed {
public:
    static void begin();
    static void setSleeping(bool sleeping);
    static void tick();
};

} // namespace ge
