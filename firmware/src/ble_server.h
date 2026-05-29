// BLE server: advertises the Goose Eye service, exposes characteristics,
// and pushes distance notifications when a client is subscribed.

#pragma once

#include <Arduino.h>
#include <stdint.h>

namespace ge {

class BleServer {
public:
    static void begin();
    static void loop();

    // Returns true if a central is connected and subscribed to distance notifies.
    static bool isStreaming();

    // Returns true if any central is connected.
    static bool isConnected();

    // Push a new distance notification. data is a packed ge_distance_payload_t.
    static void notifyDistance(const uint8_t* data, size_t len);

    // OTA status notifications.
    static void notifyOtaStatus(const uint8_t* data, size_t len);

    // Restart advertising (e.g. after a config change updates the device name).
    static void restartAdvertising();
};

} // namespace ge
