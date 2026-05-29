# GooseEye hardware assembly guide

> **Start on GitHub?** Open the [README start-here path](../README.md#the-build-path--follow-in-order) first, then return here for detail.

This guide walks you through building the GooseEye truck-bed distance sensor from scratch. **You will solder wires** — there are no pre-made harness connectors in the bill of materials. Take your time; bad joints cause intermittent power and UART failures.

The companion mobile app is **not** in this repository. This repo is firmware + hardware only.

### Guide map (in order)

| § | Topic | Jump |
|---|--------|------|
| 1 | What you are building | [Below](#1-what-you-are-building) |
| 2 | Safety | [§2](#2-safety) |
| 3 | Tools | [§3](#3-tools-and-supplies) |
| 4 | Parts to buy | [§4](#4-bill-of-materials) |
| 5 | Voltages | [§5](#5-voltage-and-current) |
| 6 | Wiring overview | [§6](#6-wiring-overview) → [full diagram](wiring-diagram.md) |
| 7 | Soldering | [§7](#7-soldering-the-harness) |
| 8 | Bench test | [§8](#8-bench-test-before-truck-install) |
| 9 | 3D print & case | [§9](#9-enclosure-assembly-v3-case) |
| 10 | Flash firmware | [§10](#10-flash-firmware) |
| 11 | Test Bluetooth | [§11](#11-verify-ble) |
| 12 | Truck install | [§12](#12-truck-installation) |
| 13 | Troubleshooting | [§13](#13-troubleshooting) |

---

## 1. What you are building

A waterproof Benewake TFmini-class LiDAR talks to a Seeed XIAO ESP32-S3 over UART. The ESP32 advertises distance over Bluetooth Low Energy (BLE) as **GooseEye**. A 12 V → 5 V buck converter powers the board from the truck; a MOSFET module switches LiDAR power off when the firmware sleeps to save energy.

Electronics live inside the **v3 printed case** ([`v3 Case.step`](v3%20Case.step)): the LiDAR bolts on with **2 mm screws**, the buck and MOSFET are fitted loosely inside, and the truck harness exits the **bottom**. The **lid** is the face that mounts **against the truck bed**.

**Next:** [§2 Safety →](#2-safety)

---

## 2. Safety

- **12 V automotive wiring** can supply high current. Use an **inline fuse** on the 12 V feed (5–10 A blade fuse is a reasonable starting point; size to your tap and wire gauge).
- Work with the vehicle **off** or the circuit **unplugged** while soldering and doing first power-up on the bench.
- The LiDAR is a Class 1 eye-safe laser product when used as specified; do not stare into the aperture at close range.
- **ESD:** touch a grounded metal surface before handling the XIAO or LiDAR PCB.
- **5 V only** on the XIAO power pad and LiDAR VCC. **Never connect 12 V** to the ESP32 or sensor.

**Next:** [§3 Tools & supplies →](#3-tools-and-supplies)

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
| **2 mm** screwdriver or hex driver | LiDAR screws into case |
| Calipers / CAD viewer | Check printed case against STEP model |

**Next:** [§4 Parts list (order these) →](#4-bill-of-materials)

---

## 4. Bill of materials

| Part | Notes | Link |
|------|-------|------|
| Benewake waterproof TFmini LiDAR | TFmini-S / Plus class; 4-wire UART | [Amazon B08D3D63QZ](https://www.amazon.com/Benewake-Distance-Waterproof-Anti-dust-Raspberry/dp/B08D3D63QZ) |
| Seeed XIAO ESP32-S3 | 2.4 GHz, USB-C; includes **U.FL WiFi antenna** | [Amazon B0BYSB66S5](https://www.amazon.com/ESP32S3-2-4GHz-Dual-core-Supported-Efficiency-Interface/dp/B0BYSB66S5) |
| DC buck converter (12 V → 5 V) | **One** module; adjust trim to **5.00 V** before load | [Amazon B0F1WB3LJ5](https://www.amazon.com/dp/B0F1WB3LJ5) |
| MOSFET switch module | High-side LiDAR VCC; control on D1 | [Amazon B0DZP27C2N](https://www.amazon.com/dp/B0DZP27C2N) |
| Wire | Main harness gauge per your run length | [Amazon B0C5T5G2NH](https://www.amazon.com/dp/B0C5T5G2NH) |
| Printed enclosure (v3 case) | STEP model in this repo; print or machine from CAD | [`docs/v3 Case.step`](v3%20Case.step) |
| **2 mm screws** (LiDAR to case) | Fasten LiDAR to enclosure mounting bosses | Length to suit your print (typically 6–12 mm) |

### Recommended (not in links above)

- Inline **blade fuse holder** + fuse on 12 V
- **USB-C cable** for development flashing
- **Ferrules** or crimp connectors for the 12 V tap (if not soldering directly)

Add photos of your finished build in `docs/images/` when you have them.

**Next:** [§5 Voltages →](#5-voltage-and-current) · [Wiring diagram (read before soldering) →](wiring-diagram.md)

---

## 5. Voltage and current

| Rail | Nominal | Notes |
|------|---------|-------|
| Vehicle input | 12 V DC (9–16 V typical) | Accessory circuit or battery; fused |
| Buck output | **5.0 V** | Adjust with no/minimal load, then recheck under load |
| XIAO | 5 V on **5V / VBUS** pad (see below) | Do not power from **3V3** — that is output only |
| TFmini VCC | 4.5–6 V | Fed from MOSFET switched 5 V |
| XIAO / LiDAR logic UART | 3.3 V tolerant | XIAO IO is 3.3 V; TFmini UART is 3.3 V CMOS |

**Current budget (planning):** ESP32-S3 + BLE ~100–250 mA peaks; TFmini ~100–300 mA peaks. Size the buck for **≥ 1 A** continuous on the 5 V output with headroom.

**Next:** [§6 Wiring overview →](#6-wiring-overview)

---

## 6. Wiring overview

See [wiring-diagram.md](wiring-diagram.md) for diagrams and tables.

Summary:

- **One buck:** 12 V in → 5 V out → XIAO **5V / VBUS** pad + MOSFET high-side input.
- **Shared ground:** buck, XIAO, MOSFET, LiDAR black.
- **LiDAR red** only to MOSFET switched output (not direct to buck).
- **UART:** XIAO D9 → white, D10 ← green.
- **Control:** XIAO D1 → MOSFET `IN`.

**Next:** Open [wiring-diagram.md](wiring-diagram.md), then [§7 Soldering →](#7-soldering-the-harness)

---

## 7. Soldering the harness

**You must solder** every connection below. Use heat shrink on each joint.

### Step 7.0 — XIAO WiFi antenna (before wiring)

The XIAO ESP32-S3 ships with a small **U.FL antenna** on a short coax pigtail. BLE range is poor without it — install it on the board **before** you solder power wires or stuff the board into the case.

1. Find the tiny **U.FL / IPEX** RF connector on the edge of the XIAO (metal snap fitting, smaller than a grain of rice).
2. Align the antenna connector squarely on top of the board connector.
3. **Press straight down** until it **snaps** into place. It is normal for this to feel **firm and a bit resistant** — use steady pressure with your fingernail or a plastic spudger, not sideways force.
4. Confirm both connectors are fully seated (no gap, antenna cable can move but the joint does not twist off easily).
5. Route the antenna **away from the LiDAR UART wires and buck wiring** so nothing yanks the U.FL joint when you close the case. Let the antenna sit loosely inside the body — do not kink or crush the cable.

**Do not** power the board with a metal tool touching the U.FL contact. If the connector does not snap after several tries, check alignment; forcing it at an angle can damage the port.

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

On the Seeed XIAO ESP32-S3, the **5 V input pad is not always silkscreened as “5V”**. Look for **`5V`**, **`VBUS`**, or similar wording on the pin next to **`GND`** (same rail as USB-C 5 V). Do **not** wire the buck to **`3V3`** — that pin is regulated **output** only.

1. Solder buck 5 V+ → XIAO **5V / VBUS** pad.
2. Solder buck GND → XIAO **`GND`** pad.

If you will have **both** USB-C and the buck connected at once while bench-testing, Seeed recommends a **Schottky diode** from the buck (+) to this pad (anode toward buck, cathode toward the board) so the buck cannot back-feed the USB port. For truck install with USB unplugged, many builds omit the diode.

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

**Next:** [§8 Bench test →](#8-bench-test-before-truck-install) · or print case first: [§9 Print enclosure →](#3d-printing-the-case)

---

## 8. Bench test (before truck install)

1. **Power off.** Connect USB-C to the XIAO for flashing only; truck 12 V can stay disconnected for first flash.
2. Flash firmware — [§10](ASSEMBLY.md#10-flash-firmware) or [README script walkthrough](../README.md#running-the-flash-script-first-timers).
3. Open serial monitor at **115200** baud. Expect:
   - `[GooseEye] boot`
   - `[GooseEye] advertising as GooseEye`
4. With 12 V bench power to the buck (LiDAR wired):
   - Measure **~5 V** between the XIAO **5V / VBUS** pad and **`GND`**.
   - Connect a phone BLE scanner; see **GooseEye** advertising.
5. **Sleep test:** disconnect BLE or wait ~2 s without session keep-alive. Serial should log `lidar power off`; multimeter on LiDAR red–black should drop to ~0 V. Reconnect with session `0x01` written to wake.

Flash firmware (Section 10) **before** closing the enclosure if you need USB access to the XIAO.

**Next:** [§9 Print & assemble case →](#9-enclosure-assembly-v3-case) · [§10 Flash firmware →](#10-flash-firmware)

---

## 9. Enclosure assembly (v3 case)

The truck-bed housing is provided as a STEP model: **[`docs/v3 Case.step`](v3%20Case.step)** (model name **v3 Case**).

### 3D printing the case

1. Open your slicer (PrusaSlicer, Bambu Studio, Cura, etc.).
2. **Drag `v3 Case.step` into the slicer window** (or use File → Import). Most slicers accept STEP directly; if yours does not, open the file in a CAD tool and export STL first.
3. **Print orientation:** place the case so the **opening faces down onto the build plate** (bottom/wire-exit opening on the plate, LiDAR end pointing up). This minimizes overhangs on the rim and gives a flat, strong base for the lid seat.
4. Use your normal filament settings for an outdoor truck-bed part (e.g. PETG or ASA for heat and UV). Add brim or arachne on the opening lip if your slicer shows curling.
5. Remove supports, deburr the LiDAR screw bosses and wire exit, then dry-fit the LiDAR before soldering the harness.

### Orientation (installed on the truck)

| Part | Role |
|------|------|
| **Lid** | **Back** of the enclosure — this face mounts **against the truck bed** |
| **Body** | Front / outer side — LiDAR looks out toward the trailer hookup area |
| **Bottom opening** | Main vehicle harness (12 V power + any slack) exits here |

```text
        [ LiDAR window ]
              |
    +-------------------+
    |      BODY         |  ---> toward trailer
    +-------------------+
    |       LID         |  ---> flat against truck bed
    +-------------------+
              |
         [ wire exit ]
```

### LiDAR mounting

1. Seat the LiDAR in the case front so the lens has a clear field of view.
2. Fasten with **2 mm screws** through the case bosses into the LiDAR mounting holes (do not overtighten into plastic threads).
3. Confirm the UART and power pigtails still reach the XIAO inside the body with gentle bends — no sharp kinks.

### Internal layout (buck, MOSFET, XIAO)

There are **no fixed mounting points** for the XIAO, buck converter, or MOSFET module in v3 — they are placed inside the body by hand:

- Confirm the **U.FL WiFi antenna** is snapped on and the cable is not pinched by the lid.
- Keep **12 V and 5 V wiring** away from the LiDAR window and UART wires.
- Avoid metal hardware touching bare converter or module pads.
- Leave enough slack on the **bottom exit** harness to service the unit without desoldering.
- Optional: secure loose boards with foam, zip ties to internal ribs, or double-sided tape on flat areas — do not block ventilation or the lid seal.

Route the soldered harness so the **main wire exits through the bottom** of the case before installing the lid.

### Lid install

1. Tuck electronics so nothing interferes with the lid perimeter.
2. Close the **lid** (truck-bed side) and fasten per your print design (screws, clips, or sealant — follow whatever features are in your manufactured part).
3. Mount the assembly to the bed with the **lid against the truck** and the LiDAR facing the trailer.

**Next:** [§10 Flash firmware →](#10-flash-firmware) (if not done yet) · [README script help →](../README.md#running-the-flash-script-first-timers)

---

## 10. Flash firmware

This step **puts the GooseEye program on the XIAO**. You are not writing code — you install one free tool, then run the included script (or copy the commands from the [README](../README.md#running-the-flash-script-first-timers)).

**Before you start:** XIAO plugged in with a **USB-C data cable** (chargers-only cables will not work).

### Install PlatformIO

**PlatformIO** is a command-line tool that compiles and uploads firmware. Install it once per computer.

| Computer | What to do |
|----------|------------|
| **Mac** | Open Terminal. If you have [Homebrew](https://brew.sh): run `brew install platformio`. If not, use the [PlatformIO installer](https://platformio.org/install/cli). |
| **Windows** | Download and run the installer from [platformio.org/install/cli](https://platformio.org/install/cli). Close and reopen PowerShell after install. |

**Check it worked:** open Terminal or PowerShell and type:

```bash
pio --version
```

You should see a version number, not “command not found.”

### Upload

**Easiest path:** follow [README — Running the flash script](../README.md#running-the-flash-script-first-timers) (copy-paste friendly).

**Or**, from the **repository root** folder (the one that contains `scripts/` and `docs/`):

```bash
chmod +x scripts/flash-firmware.sh
./scripts/flash-firmware.sh
```

(`chmod` is Mac/Linux only — tells the computer the script is allowed to run.)

macOS port is usually `/dev/cu.usbmodem*`. Override if needed:

```bash
UPLOAD_PORT=/dev/cu.usbmodem143201 ./scripts/flash-firmware.sh
```

Windows: open **Device Manager** → **Ports (COM & LPT)** → note **COM5** (example), then in PowerShell from the repo folder:

```powershell
cd firmware
pio run -t upload --upload-port COM5
```

### Serial monitor

Optional — shows text logs from the board:

```bash
cd firmware
pio device monitor
```

Press `Ctrl+C` to exit. You should see `[GooseEye] boot`.

### Bootloader mode

If upload fails: double-tap the XIAO **RESET** button quickly; the board enters USB bootloader mode. Retry upload.

**Next:** [§11 Verify BLE on your phone →](#11-verify-ble)

---

## 11. Verify BLE

1. Install **nRF Connect** (iOS/Android) or **LightBlue**.
2. Scan and connect to **GooseEye**.
3. Subscribe to the Distance characteristic (`...0001...`).
4. Write **`0x01`** to the Session characteristic (`...0007...`) to keep the device awake (see [ble-protocol.md](ble-protocol.md)).
5. Wave a hand in front of the LiDAR; distance notifications should update.

**Next:** [§9 Enclosure →](#9-enclosure-assembly-v3-case) if not built yet · [§12 Truck install →](#12-truck-installation)

---

## 12. Truck installation

1. Mount the **lid (back) flush against the truck bed**; the LiDAR window should face the trailer hitch area with clear line of sight.
2. Strain-relief the harness where it exits the **bottom** of the case and at the 12 V tap.
3. Route wiring away from exhaust, heat, and moving parts.
4. Re-check **5 V** under load with the engine on (alternator may raise rail voltage).
5. After sealing the case, firmware updates require USB access to the XIAO — plan ahead or open the enclosure.

**Next:** [§13 Troubleshooting →](#13-troubleshooting) · [Back to README start →](../README.md)

---

## 13. Troubleshooting

| Symptom | Likely cause | Fix |
|---------|--------------|-----|
| No USB port for flash | Charge-only cable | Use data-capable USB-C |
| Upload fails | Wrong port / not in bootloader | Set `UPLOAD_PORT`; double-tap RESET |
| No BLE advertisement | Bad flash, power, missing antenna | Reflash; check 5 V; snap U.FL antenna onto RF port |
| BLE very weak / drops at short range | Antenna not seated or crushed in case | Reseat U.FL connector; reroute antenna cable |
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

## 14. Firmware pin reference

| XIAO label | GPIO | Function |
|------------|------|----------|
| D9 | 8 | UART TX → LiDAR RX |
| D10 | 9 | UART RX ← LiDAR TX |
| D1 | 2 | LiDAR power MOSFET control |
| 5V / VBUS + GND | — | 5 V power in (not 12 V; not 3V3) |

Firmware version is in `firmware/platformio.ini` (`FIRMWARE_VERSION_*`).

---

## 15. Next steps

- Add photos of your printed case and bed mount to `docs/images/`.
- Integrate with the GooseEye mobile app (distributed separately).
- Report hardware issues on the [GitHub issue tracker](https://github.com/cjevolutions/GooseEye/issues).

**Back to top:** [README — start here →](../README.md)
