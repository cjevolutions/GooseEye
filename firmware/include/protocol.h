// Goose Eye BLE Protocol Definitions
// Shared between firmware and (conceptually) the mobile app.
//
// All multi-byte values transmitted in little-endian byte order.

#pragma once

#include <stdint.h>

// ============================================================================
// BLE Service & Characteristic UUIDs
// ============================================================================
//
// Base UUID: 4e6f7365-4579-6500-XXXX-000000000000
//   ("Nose" "Eye" "_" in ASCII -> matches the GooseEye name)
//
// Characteristic UUIDs replace the "0000" portion of the third group.

#define GE_SERVICE_UUID         "4e6f7365-4579-6500-0000-000000000000"
#define GE_CHAR_DISTANCE_UUID   "4e6f7365-4579-6500-0001-000000000000"
#define GE_CHAR_DEVICE_INFO     "4e6f7365-4579-6500-0002-000000000000"
#define GE_CHAR_DEVICE_CONFIG   "4e6f7365-4579-6500-0003-000000000000"
#define GE_CHAR_OTA_CONTROL     "4e6f7365-4579-6500-0004-000000000000"
#define GE_CHAR_OTA_DATA        "4e6f7365-4579-6500-0005-000000000000"
#define GE_CHAR_FACTORY_RESET   "4e6f7365-4579-6500-0006-000000000000"
#define GE_CHAR_SESSION_UUID    "4e6f7365-4579-6500-0007-000000000000"

// ============================================================================
// Distance characteristic payload
// ============================================================================
//
// Layout (6 bytes total, little-endian):
//   [0..1]  distance_mm   uint16   Distance reading in millimeters. 0 = invalid.
//   [2..3]  strength      uint16   TFmini signal strength (0..65535).
//   [4]     quality       uint8    Derived quality 0..100 (100 = best).
//   [5]     flags         uint8    Bit0: out_of_range, Bit1: too_close,
//                                  Bit2: smoothed (firmware-side), reserved.

#pragma pack(push, 1)
typedef struct {
    uint16_t distance_mm;
    uint16_t strength;
    uint8_t  quality;
    uint8_t  flags;
} ge_distance_payload_t;
#pragma pack(pop)

#define GE_FLAG_OUT_OF_RANGE  0x01
#define GE_FLAG_TOO_CLOSE     0x02
// GE_FLAG_SMOOTHED: always set; indicates the firmware spike filter has run.
// The app may apply additional smoothing on top for latency control.
#define GE_FLAG_SMOOTHED      0x04

// ============================================================================
// OTA control commands
// ============================================================================
//
// Command byte sent on OTA_CONTROL characteristic.
// Multi-byte payloads use little-endian.
//
// Client -> Device commands:
//   0x01 START   payload: uint32 total_size, uint32 expected_crc32
//   0x02 ABORT   payload: (none)
//   0x03 VERIFY  payload: (none)         -- verify CRC and reboot
//
// Device -> Client status notifications (sent back via OTA_CONTROL notify):
//   0xA0 READY    payload: (none)
//   0xA1 PROGRESS payload: uint32 bytes_received
//   0xA2 ERROR    payload: uint8 error_code
//   0xA3 SUCCESS  payload: (none)         -- about to reboot

#define GE_OTA_CMD_START    0x01
#define GE_OTA_CMD_ABORT    0x02
#define GE_OTA_CMD_VERIFY   0x03

#define GE_OTA_STATUS_READY    0xA0
#define GE_OTA_STATUS_PROGRESS 0xA1
#define GE_OTA_STATUS_ERROR    0xA2
#define GE_OTA_STATUS_SUCCESS  0xA3

#define GE_OTA_ERR_BAD_STATE      0x01
#define GE_OTA_ERR_BAD_SIZE       0x02
#define GE_OTA_ERR_WRITE_FAILED   0x03
#define GE_OTA_ERR_CRC_MISMATCH   0x04
#define GE_OTA_ERR_NO_PARTITION   0x05

// Maximum OTA chunk size pushed via OTA_DATA write-without-response.
// The default ATT MTU on iOS is ~185; Android negotiates up to 247.
// We chunk to 240 bytes max to leave headroom.
#define GE_OTA_MAX_CHUNK 240

// ============================================================================
// Factory reset magic
// ============================================================================
// Writing exactly these 4 bytes [0xDE, 0xAD, 0xBE, 0xEF] to the factory reset
// characteristic clears NVS and reboots.
#define GE_FACTORY_RESET_MAGIC_0 0xDE
#define GE_FACTORY_RESET_MAGIC_1 0xAD
#define GE_FACTORY_RESET_MAGIC_2 0xBE
#define GE_FACTORY_RESET_MAGIC_3 0xEF

// ============================================================================
// Distance notification cadence
// ============================================================================
// Firmware reads the TFmini at its native 100Hz, but throttles BLE notifications
// to 20 Hz to balance responsiveness against BLE bandwidth + battery.
#define GE_DISTANCE_NOTIFY_HZ   20
#define GE_DISTANCE_NOTIFY_MS   (1000 / GE_DISTANCE_NOTIFY_HZ)

// Maximum age of a LiDAR frame before the firmware treats it as stale.
// ~10 LiDAR sample periods at 100 Hz; prevents retransmitting a frozen value
// when the sensor drops out temporarily.
#define GE_LIDAR_MAX_AGE_MS     100

// Session keep-alive: app writes GE_SESSION_KEEP_AWAKE while foreground+connected.
#define GE_SESSION_KEEP_AWAKE   0x01
#define GE_SESSION_ALLOW_SLEEP  0x00
#define GE_SESSION_TIMEOUT_MS   5000
#define GE_SLEEP_ENTER_MS       2000

// Default advertised device name (16 chars max, NVS-overridable).
#define GE_DEFAULT_DEVICE_NAME  "GooseEye"
