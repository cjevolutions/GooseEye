// Goose Eye firmware entry point.
//
// XIAO ESP32-S3 + Benewake TFmini-S/Plus LiDAR.
// Reads the LiDAR at its native 100Hz, throttles BLE notifications to 20Hz.
// Sleep mode (breathing LED, LiDAR off) when disconnected or the app is not foreground.

#include <Arduino.h>

#include "ble_server.h"
#include "config.h"
#include "lidar.h"
#include "ota.h"
#include "pins.h"
#include "power.h"
#include "protocol.h"

namespace {

uint32_t g_lastNotifyMs = 0;
uint16_t g_smoothedMm   = 0;

// EMA smoothing — called on every valid LiDAR frame (~100 Hz) so the filter
// sees all samples, not just the 20 Hz notify subsample.
//
// alpha ≈ 0.2 at 100 Hz gives a time constant of ~50 ms, matching the old
// "alpha = 0.5 at 20 Hz" noise rejection while using 5× more data. The app
// (processor.ts) applies a second, user-configurable EMA on top.
//
// GE_FLAG_SMOOTHED is always set so the app knows the value has already been
// spike-filtered.
uint16_t smooth(uint16_t raw) {
    if (g_smoothedMm == 0) {
        g_smoothedMm = raw;
        return raw;
    }
    // Fixed-point alpha ≈ 0.2: new = (raw + 4 * old) / 5
    g_smoothedMm = (uint16_t)(((uint32_t)raw + 4u * (uint32_t)g_smoothedMm) / 5u);
    return g_smoothedMm;
}

uint8_t deriveQuality(uint16_t strength) {
    // TFmini strength: < 100 = unreliable, 100..2000 = good, > 2000 = great,
    // 65535 = saturated.
    if (strength == 0 || strength == 65535) return 0;
    if (strength < 100) return 10;
    if (strength < 500) return 40;
    if (strength < 2000) return 80;
    return 100;
}

} // namespace

void setup() {
    Serial.begin(115200);
    delay(200); // USB-CDC needs a moment
    Serial.println(F("[GooseEye] boot"));

    ge::Config::begin();
    ge::Lidar::begin(GE_LIDAR_RX_PIN, GE_LIDAR_TX_PIN);
    ge::Ota::begin();
    ge::BleServer::begin();
    ge::Power::begin();

    Serial.print(F("[GooseEye] advertising as "));
    Serial.println(ge::Config::get().name);
}

void loop() {
    ge::LidarFrame frame;
    bool newFrame = ge::Lidar::poll(frame);

    // Smooth every valid LiDAR frame (~100 Hz) so the filter sees all data.
    if (newFrame && frame.valid) {
        uint16_t mm = frame.distance_cm * 10;
        smooth(mm);
    }

    // Only push notifications while a central is subscribed.
    if (ge::BleServer::isStreaming() && !ge::Ota::inProgress()) {
        uint32_t now = millis();
        if (now - g_lastNotifyMs >= GE_DISTANCE_NOTIFY_MS) {
            g_lastNotifyMs = now;
            ge::LidarFrame latest = ge::Lidar::latest();

            const bool fresh = latest.valid &&
                               (now - latest.timestamp_ms) <= GE_LIDAR_MAX_AGE_MS;

            ge_distance_payload_t payload = {};
            if (fresh) {
                payload.distance_mm = g_smoothedMm;
                payload.strength    = latest.strength;
                payload.quality     = deriveQuality(latest.strength);
                payload.flags       = GE_FLAG_SMOOTHED;
                if (latest.distance_cm == 0 || latest.strength < 100 ||
                    latest.strength == 65535) {
                    payload.flags |= GE_FLAG_OUT_OF_RANGE;
                }
                if (latest.distance_cm < 10) {
                    payload.flags |= GE_FLAG_TOO_CLOSE;
                }
            } else {
                payload.flags = GE_FLAG_OUT_OF_RANGE;
                g_smoothedMm = 0;
            }
            ge::BleServer::notifyDistance(
                reinterpret_cast<uint8_t*>(&payload), sizeof(payload)
            );
        }
    } else {
        // Drop the smoothing state when nobody's listening so the next session
        // starts clean.
        g_smoothedMm = 0;
    }

    ge::Power::tick();
    ge::BleServer::loop();
    delay(2); // small yield -- TFmini frames arrive every 10ms at 100Hz
}
