// NVS-backed device configuration (device name, frame rate, etc.)

#pragma once

#include <Arduino.h>

namespace ge {

struct DeviceConfig {
    String name;          // BLE advertised name, max 24 chars
    uint16_t frameRateHz; // TFmini sampling rate (1..1000), default 100
};

class Config {
public:
    static void begin();
    static void load();
    static void save();
    static void resetToDefaults();

    static const DeviceConfig& get();
    static void setName(const String& name);
    static void setFrameRate(uint16_t hz);

    // Returns JSON string for the Device Info characteristic.
    static String deviceInfoJson();
    static String deviceConfigJson();

    // Parse a config JSON blob written by the app. Returns true on success.
    static bool applyConfigJson(const String& json);
};

} // namespace ge
