# Goose Eye Firmware

PlatformIO firmware for the **Seeed Studio XIAO ESP32-S3** + **Benewake TFmini-S / TFmini Plus** LiDAR sensor.

Hardware assembly and wiring: see the [GooseEye hardware repo](https://github.com/cjevolutions/GooseEye) `docs/ASSEMBLY.md` (or `gooseeye-hardware/docs/` in the monorepo).

## Hardware pins

| XIAO pin | GPIO | Connection |
|----------|------|------------|
| 5V / VBUS | — | Buck 5 V (USB rail; silkscreen may say VBUS — not 3V3) |
| GND | — | Common ground with buck, LiDAR, MOSFET |
| D9 | 8 | TFmini white (RX) — ESP UART TX |
| D10 | 9 | TFmini green (TX) — ESP UART RX |
| D1 | 2 | MOSFET module IN (LiDAR VCC high-side switch) |
| LED | 21 | Built-in status LED (breathe in sleep) |

LiDAR **red (VCC)** is fed from the MOSFET switched output, not directly from the buck.

## Build & flash

```bash
cd firmware
pio run                 # build
pio run -t upload       # flash (USB)
pio device monitor      # serial console @ 115200
```

From the monorepo root: `./scripts/flash-firmware.sh`

## BLE protocol

Service UUID `4e6f7365-4579-6500-0000-000000000000`. Full definitions in [`include/protocol.h`](include/protocol.h).

| Char | Suffix | Properties | Purpose |
|------|--------|------------|---------|
| Distance | `...0001...` | Notify / Read | 6-byte `ge_distance_payload_t` @ 20 Hz |
| Device Info | `...0002...` | Read | JSON: name, firmware, mac, uptime |
| Device Config | `...0003...` | Read / Write | JSON: writable name, frame rate |
| OTA Control | `...0004...` | Write / Notify | Update lifecycle + status |
| OTA Data | `...0005...` | Write w/o Resp. | Firmware bytes in ≤240 B chunks |
| Factory Reset | `...0006...` | Write | Magic `DE AD BE EF` clears NVS |
| Session | `...0007...` | Write | `0x01` keep awake while app foreground |

## Power behavior

- **Awake:** BLE connected, app session keep-alive within 5 s, OTA idle → LiDAR rail on, UART active, distance notify @ 20 Hz.
- **Sleep:** disconnect or no keep-alive for 2 s → LiDAR rail off (MOSFET), UART off, status LED breathes, BLE still advertises.
- No ESP deep sleep; the radio stays up for reconnect.

## Versioning

Set in [`platformio.ini`](platformio.ini) via `FIRMWARE_VERSION_*` build flags. Exposed on the Device Info characteristic.
