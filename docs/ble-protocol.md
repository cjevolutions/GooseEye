# GooseEye BLE protocol

Human-readable summary of the firmware GATT service. Canonical definitions live in [`firmware/include/protocol.h`](../firmware/include/protocol.h).

## Service

| | UUID |
|---|------|
| Primary service | `4e6f7365-4579-6500-0000-000000000000` |

Advertised name defaults to **GooseEye** (NVS-overridable via Device Config).

## Characteristics

| Name | UUID suffix | Properties | Purpose |
|------|-------------|------------|---------|
| Distance | `...0001...` | Notify, Read | 6-byte distance payload @ 20 Hz when streaming |
| Device Info | `...0002...` | Read | JSON: name, firmware version, MAC, uptime |
| Device Config | `...0003...` | Read, Write | JSON: writable name, frame rate |
| OTA Control | `...0004...` | Write, Notify | Firmware update commands and status |
| OTA Data | `...0005...` | Write Without Response | Firmware binary chunks (max 240 B) |
| Factory Reset | `...0006...` | Write | Magic bytes `DE AD BE EF` → clear NVS, reboot |
| Session | `...0007...` | Write | `0x01` = keep awake, `0x00` = allow sleep |

Full UUID form: `4e6f7365-4579-6500-00XX-000000000000` where `XX` is the suffix above.

## Distance payload (6 bytes, little-endian)

| Offset | Field | Type | Description |
|--------|-------|------|-------------|
| 0–1 | distance_mm | uint16 | Millimeters; 0 = invalid |
| 2–3 | strength | uint16 | TFmini signal strength |
| 4 | quality | uint8 | Derived 0–100 |
| 5 | flags | uint8 | Bit0 out_of_range, Bit1 too_close, Bit2 smoothed |

## Session / sleep behavior

| Constant | Value | Meaning |
|----------|-------|---------|
| Keep awake | `0x01` | App foreground — stay awake |
| Allow sleep | `0x00` | App backgrounded |
| Session timeout | 5 s | No keep-alive write → treat as background |
| Sleep enter delay | 2 s | After disconnect/no session → sleep |

**Sleep:** LiDAR power off (MOSFET), UART off, status LED breathes, BLE still advertises.

**Awake:** BLE connected + session keep-alive within 5 s + OTA idle → LiDAR on, notifications at 20 Hz.

## OTA (optional integrators)

| Command | Byte | Payload |
|---------|------|---------|
| START | `0x01` | uint32 size, uint32 CRC32 |
| ABORT | `0x02` | — |
| VERIFY | `0x03` | — |

Status notifications: READY `0xA0`, PROGRESS `0xA1`, ERROR `0xA2`, SUCCESS `0xA3`.

## Testing without the mobile app

Use **nRF Connect** or **LightBlue**:

1. Scan for **GooseEye**.
2. Connect and subscribe to Distance notifications.
3. Write `0x01` to Session characteristic to simulate foreground (otherwise device sleeps after ~2 s when connected without keep-alive, depending on connection state).

For full integration, use the GooseEye mobile app (not in this repository).
