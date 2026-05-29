# GooseEye hardware assembly guide

This guide walks you through building the GooseEye truck-bed distance sensor from scratch. **You will solder wires** — there are no pre-made harness connectors in the bill of materials. Take your time; bad joints cause intermittent power and UART failures.

The companion mobile app is **not** in this repository. This repo is firmware + hardware only.

---

## 1. What you are building

A waterproof Benewake TFmini-class LiDAR talks to a Seeed XIAO ESP32-S3 over UART. The ESP32 advertises distance over Bluetooth Low Energy (BLE) as **GooseEye**. A 12 V → 5 V buck converter powers the board from the truck; a MOSFET module switches LiDAR power off when the firmware sleeps to save energy.

---

## 2. Safety

- **12 V automotive wiring** can supply high current. Use an **inline fuse** on the 12 V feed (5–10 A blade fuse is a reasonable starting point; size to your tap and wire gauge).
- Work with the vehicle **off** or the circuit **unplugged** while soldering and doing first power-up on the bench.
- The LiDAR is a Class 1 eye-safe laser product when used as specified; do not stare into the aperture at close range.
- **ESD:** touch a grounded metal surface before handling the XIAO or LiDAR PCB.
- **5 V only** on the XIAO `5V` pin and LiDAR VCC. **Never connect 12 V** to the ESP32 or sensor.

---

## 3. Tools and supplies

| Tool / supply | Purpose |
|---------------|---------|
| Soldering iron (temperature-controlled, ~350°C / 660°F for lead-free) | Joints |
| Solder (rosin core) | Joints |
| Heat-shrink tubing (assorted) | Insulate joints |
| Wire stripper | Prepare wire ends |
| Multimeter | Verify 5 V, continuity, polarity |
| USB-C **data** cable | Flash firmware and serial debug |
| PC with PlatformIO | Build and upload firmware |
| Optional: helping hands, flux, isopropyl alcohol | Easier soldering and cleanup |

---

## 4. Bill of materials

| Part | Notes | Link |
|------|-------|------|
| Benewake waterproof TFmini LiDAR | TFmini-S / Plus class; 4-wire UART | [Amazon B08D3D63QZ](https://www.amazon.com/Benewake-Distance-Waterproof-Anti-dust-Raspberry/dp/B08D3D63QZ) |
| Seeed XIAO ESP32-S3 | 2.4 GHz, USB-C | [Amazon B0BYSB66S5](https://www.amazon.com/ESP32S3-2-4GHz-Dual-core-Supported-Efficiency-Interface/dp/B0BYSB66S5) |
| DC buck converter (12 V → 5 V) | **One** module; adjust trim to **5.00 V** before load | [Amazon B0F1WB3LJ5](https://www.amazon.com/dp/B0F1WB3LJ5) |
| MOSFET switch module | High-side LiDAR VCC; control on D1 | [Amazon B0DZP27C2N](https://www.amazon.com/dp/B0DZP27C2N) |
| Wire | Main harness gauge per your run length | [Amazon B0C5T5G2NH](https://www.amazon.com/dp/B0C5T5G2NH) |

### Recommended (not in links above)

- Inline **blade fuse holder** + fuse on 12 V
- **USB-C cable** for development flashing
- **Enclosure** / strain relief for truck-bed mounting
- **Ferrules** or crimp connectors for the 12 V tap (if not soldering directly)

Add photos, STL mounts, or fuse part numbers in `docs/images/` or the repo root when you have them.

---

## 5. Voltage and current

| Rail | Nominal | Notes |
|------|---------|-------|
| Vehicle input | 12 V DC (9–16 V typical) | Accessory circuit or battery; fused |
| Buck output | **5.0 V** | Adjust with no/minimal load, then recheck under load |
| XIAO | 5 V on `5V` pin | Do not use 3.3 V pin for power |
| TFmini VCC | 4.5–6 V | Fed from MOSFET switched 5 V |
| XIAO / LiDAR logic UART | 3.3 V tolerant | XIAO IO is 3.3 V; TFmini UART is 3.3 V CMOS |

**Current budget (planning):** ESP32-S3 + BLE ~100–250 mA peaks; TFmini ~100–300 mA peaks. Size the buck for **≥ 1 A** continuous on the 5 V output with headroom.

---

## 6. Wiring overview

See [wiring-diagram.md](wiring-diagram.md) for diagrams and tables.

Summary:

- **One buck:** 12 V in → 5 V out → XIAO `5V` + MOSFET high-side input.
- **Shared ground:** buck, XIAO, MOSFET, LiDAR black.
- **LiDAR red** only to MOSFET switched output (not direct to buck).
- **UART:** XIAO D9 → white, D10 ← green.
- **Control:** XIAO D1 → MOSFET `IN`.

---

## 7. Soldering the harness

**You must solder** every connection below. Use heat shrink on each joint.

### Step 7.1 — Prepare wires

1. Cut lengths with slack for mounting (bench-test short lengths first).
2. Strip ~3–5 mm (1/8–3/16 in) of insulation.
3. Tin the stripped ends with a small amount of solder.

### Step 7.2 — Buck converter

1. **Do not connect 12 V yet.**
2. Solder 12 V+ and 12 V− to the buck input pads (observe module silkscreen).
3. Solder 5 V+ and GND leads to the buck output pads.
4. On the bench, apply 12 V from a current-limited supply or vehicle tap (fused).
5. Turn the trim pot until the multimeter reads **5.00 V** at the output (no load or light load per module datasheet).

### Step 7.3 — XIAO power

1. Solder buck 5 V+ → XIAO **`5V`** pad.
2. Solder buck GND → XIAO **`GND`** pad.

### Step 7.4 — MOSFET module

1. Buck 5 V+ → MOSFET **VIN+** (high-side in).
2. Buck GND → MOSFET **GND**.
3. MOSFET **OUT+** → LiDAR **red** (VCC).
4. LiDAR **black** → buck GND (same star ground as XIAO).
5. XIAO **D1** → MOSFET **IN** / control.

Confirm silkscreen on [your module](wiring-diagram.md#mosfet-module-labels); labels vary by seller.

### Step 7.5 — UART

1. XIAO **D9** → LiDAR **white** (RX on sensor).
2. XIAO **D10** → LiDAR **green** (TX on sensor).

Do not swap D9 and D10 — distance readings will fail.

### Step 7.6 — Quality check

- Tug-test each joint gently.
- Heat-shrink every exposed conductor.
- Verify **no short** between 5 V and GND with multimeter beep test before applying power.

---

## 8. Bench test (before truck install)

1. **Power off.** Connect USB-C to the XIAO for flashing only; truck 12 V can stay disconnected for first flash.
2. Flash firmware (Section 9).
3. Open serial monitor at **115200** baud. Expect:
   - `[GooseEye] boot`
   - `[GooseEye] advertising as GooseEye`
4. With 12 V bench power to the buck (LiDAR wired):
   - Measure **~5 V** on XIAO `5V`–`GND`.
   - Connect a phone BLE scanner; see **GooseEye** advertising.
5. **Sleep test:** disconnect BLE or wait ~2 s without session keep-alive. Serial should log `lidar power off`; multimeter on LiDAR red–black should drop to ~0 V. Reconnect with session `0x01` written to wake.

---

## 9. Flash firmware

### Install PlatformIO

- macOS: `brew install platformio`
- Other: [PlatformIO CLI install](https://platformio.org/install/cli)

### Upload

From the **repository root** (this folder):

```bash
chmod +x scripts/flash-firmware.sh
./scripts/flash-firmware.sh
```

macOS port is usually `/dev/cu.usbmodem*`. Override if needed:

```bash
UPLOAD_PORT=/dev/cu.usbmodem143201 ./scripts/flash-firmware.sh
```

Windows: use Device Manager to find `COMx`, then:

```cmd
set UPLOAD_PORT=COM5
scripts\flash-firmware.sh
```

### Serial monitor

```bash
cd firmware
pio device monitor
```

### Bootloader mode

If upload fails: double-tap the XIAO **RESET** button quickly; the board enters USB bootloader mode. Retry upload.

---

## 10. Verify BLE

1. Install **nRF Connect** (iOS/Android) or **LightBlue**.
2. Scan and connect to **GooseEye**.
3. Subscribe to the Distance characteristic (`...0001...`).
4. Write **`0x01`** to the Session characteristic (`...0007...`) to keep the device awake (see [ble-protocol.md](ble-protocol.md)).
5. Wave a hand in front of the LiDAR; distance notifications should update.

---

## 11. Truck installation

1. Secure the buck and MOSFET so they cannot short against metal.
2. Route the LiDAR cable away from exhaust and moving parts.
3. Mount the LiDAR with a clear line of sight toward the trailer target area.
4. Strain-relief the 12 V tap and all solder joints.
5. Re-check 5 V under load with the engine on (alternator may raise rail voltage).

---

## 12. Troubleshooting

| Symptom | Likely cause | Fix |
|---------|--------------|-----|
| No USB port for flash | Charge-only cable | Use data-capable USB-C |
| Upload fails | Wrong port / not in bootloader | Set `UPLOAD_PORT`; double-tap RESET |
| No BLE advertisement | Bad flash, power | Reflash; check 5 V on XIAO |
| BLE connects, no distance | Sleep mode, no session | Write `0x01` to Session char |
| Distance always invalid | D9/D10 swapped | Swap white/green UART |
| LiDAR always off | D1 wrong; active-low module | Rewire D1; rebuild with inverted flag (below) |
| LiDAR always on in sleep | MOSFET wired on wrong terminals | Review high-side diagram |
| 5 V low under load | Buck underrated / bad trim | Re-trim at load; larger buck |

### Inverted MOSFET control

If your module turns the load **on** when D1 is LOW, add to `firmware/platformio.ini` under `build_flags`:

```
-DGE_LIDAR_POWER_ACTIVE_HIGH=0
```

Rebuild and reflash.

---

## 13. Firmware pin reference

| XIAO label | GPIO | Function |
|------------|------|----------|
| D9 | 8 | UART TX → LiDAR RX |
| D10 | 9 | UART RX ← LiDAR TX |
| D1 | 2 | LiDAR power MOSFET control |
| 5V / GND | — | 5 V power (not 12 V) |

Firmware version is in `firmware/platformio.ini` (`FIRMWARE_VERSION_*`).

---

## 14. Next steps

- Add enclosure photos and mount templates to this repo.
- Integrate with the GooseEye mobile app (distributed separately).
- Report hardware issues on the project GitHub tracker.
