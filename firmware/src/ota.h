// BLE OTA firmware update receiver.
//
// Update protocol (see protocol.h):
//   1. App writes GE_OTA_CMD_START + [u32 total_size][u32 expected_crc32] to
//      OTA_CONTROL. Firmware allocates an update partition and replies READY.
//   2. App streams raw firmware bytes to OTA_DATA in <= 240-byte chunks.
//      Firmware writes them sequentially to the inactive OTA partition and
//      notifies PROGRESS every ~4KB.
//   3. App writes GE_OTA_CMD_VERIFY. Firmware checks CRC, marks the new
//      partition as boot target, replies SUCCESS, then reboots.
//   4. Either side can write GE_OTA_CMD_ABORT to cancel.

#pragma once

#include <Arduino.h>

namespace ge {

class Ota {
public:
    static void begin();

    // Called when a START command is written.
    static void handleStart(const uint8_t* data, size_t len);

    // Called when a data chunk is written.
    static void handleData(const uint8_t* data, size_t len);

    // Called when a VERIFY command is written.
    static void handleVerify();

    // Called when an ABORT command is written (or BLE disconnects mid-update).
    static void handleAbort();

    // True while an update is in progress.
    static bool inProgress();
};

} // namespace ge
