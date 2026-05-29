// TFmini-S / TFmini Plus UART driver.
//
// Both sensors share the same 9-byte frame format at 115200 baud, so the same
// driver works for either. Frame layout:
//
//   [0] 0x59  (header)
//   [1] 0x59  (header)
//   [2] dist_low   (cm low byte)
//   [3] dist_high  (cm high byte)
//   [4] strength_low
//   [5] strength_high
//   [6] reserved / temperature_low (varies by model)
//   [7] reserved / temperature_high
//   [8] checksum = (sum of bytes 0..7) & 0xFF
//
// We poll the UART in a tight loop. A small state-machine parser tolerates
// partial reads and resync after corruption.

#pragma once

#include <Arduino.h>

namespace ge {

struct LidarFrame {
    uint16_t distance_cm;
    uint16_t strength;
    uint16_t temperature; // raw register value; not used for guidance
    uint32_t timestamp_ms;
    bool valid;           // true if checksum matched and within sensor spec
};

class Lidar {
public:
    static void begin(int rxPin, int txPin, uint32_t baud = 115200);

    // Enable or disable UART reads. When disabled the parser state is cleared.
    static void setEnabled(bool enabled);

    // Pump the UART parser. Call from loop() frequently. Returns true if a
    // new valid frame was just parsed (also populated into outFrame).
    static bool poll(LidarFrame& outFrame);

    // Returns the most recent valid frame, or {0,0,0,0,false} if none yet.
    static LidarFrame latest();
};

} // namespace ge
