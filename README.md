# GooseEye Hardware

Open-source **ESP32-S3 firmware and build guide** for the GooseEye truck-bed LiDAR hookup assistant. This repository contains everything needed to assemble, wire, and flash the sensor module. The mobile app is distributed separately and is not included here.

## Quick links

| Document | Description |
|----------|-------------|
| [docs/ASSEMBLY.md](docs/ASSEMBLY.md) | Full build guide — parts, soldering, voltages, bench test |
| [docs/wiring-diagram.md](docs/wiring-diagram.md) | Wiring tables and diagrams |
| [docs/ble-protocol.md](docs/ble-protocol.md) | BLE service reference for integrators |
| [firmware/](firmware/) | PlatformIO project (Seeed XIAO ESP32-S3) |

## You will need to solder

All harness connections between the buck converter, MOSFET module, XIAO, and LiDAR are soldered joints (plus heat shrink). There are no plug-and-play connectors in the BOM. See the assembly guide for step-by-step instructions.

## Flash firmware

1. Install [PlatformIO CLI](https://platformio.org/install/cli).
2. Connect the XIAO with a **USB-C data cable**.
3. From this repo root:

```bash
chmod +x scripts/flash-firmware.sh
./scripts/flash-firmware.sh
```

Serial monitor: `cd firmware && pio device monitor` (115200 baud).

## Updating firmware from the monorepo

If you develop in the private GooseEyeApp monorepo, run:

```bash
./scripts/prepare-publish.sh
```

from this directory to copy the latest `firmware/` sources before committing.

## License

Add your license file at the repo root if desired.
