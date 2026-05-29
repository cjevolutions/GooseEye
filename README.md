# GooseEye hardware — **start here**

You are in the right place. This repository is a **step-by-step kit** for building the GooseEye truck-bed distance sensor: a small box with a laser rangefinder and a Bluetooth chip that talks to the GooseEye phone app (the app is **not** in this repo).

**You do not need** prior experience with GitHub, soldering, programming, or running scripts. Follow the numbered path below in order. Each step links to the **next** one.

---

## What you are making (30 seconds)

A waterproof LiDAR measures distance to your trailer. A **Seeed XIAO ESP32-S3** board sends that distance over **Bluetooth** to your phone. Everything runs from your truck’s **12 V** power, converted to safe **5 V** inside the box. You will **solder wires**, **print a plastic case**, and **plug the board into your computer once** to load the firmware.

---

## Before you start — read this honestly

| You will need to… | Notes |
|-------------------|--------|
| **Order parts** | Amazon links in the [parts list](docs/ASSEMBLY.md#4-bill-of-materials) |
| **3D-print a case** | Free model file included; needs a printer or a print service |
| **Solder** | Every wire joint is soldered — first time is OK, go slow |
| **Use a computer once** | Mac or Windows, USB-C cable, internet to install one tool |
| **Run one command** | Copy-paste from this page; explained below |
| **Work with 12 V truck power** | Fuse strongly recommended; vehicle off while wiring |

Plan on **a weekend** if you are new to soldering. Stop anytime; pick up at the same step number.

---

## New to GitHub? You only need the files

You **do not** need a GitHub account to build this. You do **not** need to learn git.

**Option A — Download as ZIP (easiest)**

1. Open this page: [github.com/cjevolutions/GooseEye](https://github.com/cjevolutions/GooseEye)
2. Click the green **Code** button.
3. Click **Download ZIP**.
4. Double-click the ZIP on your computer to unpack it.
5. You now have a folder named `GooseEye-main` (or similar). **That folder is your project.** All paths below assume you are inside it.

**Option B — Clone (only if you already use git)**

```text
git clone git@github.com:cjevolutions/GooseEye.git
cd GooseEye
```

The rest of this README lives inside that folder.

---

## The build path — follow in order

Check off each step. **Next** links take you to the right document and section.

| Step | What you do | Open this next |
|:----:|-------------|----------------|
| **0** | Understand the project and risks | → [Safety & tools](docs/ASSEMBLY.md#2-safety) |
| **1** | Buy / gather all parts | → [Bill of materials](docs/ASSEMBLY.md#4-bill-of-materials) |
| **2** | See what connects to what (read before soldering) | → [Wiring diagram](docs/wiring-diagram.md) |
| **3** | Print the enclosure | → [Print the v3 case](docs/ASSEMBLY.md#3d-printing-the-case) |
| **4** | Snap on WiFi antenna, then solder the harness | → [Soldering steps](docs/ASSEMBLY.md#7-soldering-the-harness) |
| **5** | Power on at the bench (no truck install yet) | → [Bench test](docs/ASSEMBLY.md#8-bench-test-before-truck-install) |
| **6** | Install the flash tool on your computer | → [Install PlatformIO](docs/ASSEMBLY.md#install-platformio) |
| **7** | Load firmware onto the XIAO (one script) | → [Flash firmware](docs/ASSEMBLY.md#upload) + [Script help](#running-the-flash-script-first-timers) below |
| **8** | Confirm Bluetooth with your phone | → [Verify BLE](docs/ASSEMBLY.md#11-verify-ble) |
| **9** | Mount parts in the case | → [Enclosure assembly](docs/ASSEMBLY.md#9-enclosure-assembly-v3-case) |
| **10** | Install in the truck | → [Truck installation](docs/ASSEMBLY.md#12-truck-installation) |
| **11** | Something wrong? | → [Troubleshooting](docs/ASSEMBLY.md#13-troubleshooting) |

**Full single document (printable):** [docs/ASSEMBLY.md](docs/ASSEMBLY.md) — same content as the table above, with more detail in each section.

---

### Step 0 → Start the assembly guide

Open **[docs/ASSEMBLY.md — section 1](docs/ASSEMBLY.md#1-what-you-are-building)** and read sections **1 (what you are building)** and **2 (safety)**.

Then continue to **section 3 (tools)** and **section 4 (parts list)** — order anything you do not have.

**Next:** [Bill of materials →](docs/ASSEMBLY.md#4-bill-of-materials)

---

### Step 2 → Wiring (look at diagrams before you solder)

Open **[docs/wiring-diagram.md](docs/wiring-diagram.md)**. You do not need to memorize it; keep it open while you solder.

Key idea: truck **12 V** → **buck converter** → **5 V** to the XIAO **5V / VBUS** pad (board may say **VBUS**, not “5V”) and to a **MOSFET** that switches LiDAR power. UART wires go XIAO **D9/D10** ↔ LiDAR white/green.

**Next:** [Print the case →](docs/ASSEMBLY.md#3d-printing-the-case)

---

### Step 4 → Soldering & antenna

In **[docs/ASSEMBLY.md section 7](docs/ASSEMBLY.md#7-soldering-the-harness)**:

1. **[§7.0](docs/ASSEMBLY.md#step-70--xiao-wifi-antenna-before-wiring)** — Press the small **U.FL antenna** onto the tiny RF port on the XIAO (press straight down; it will feel stiff until it snaps).
2. **§7.1–7.6** — Solder buck, XIAO power, MOSFET, and LiDAR wires.

**Next:** [Bench test →](docs/ASSEMBLY.md#8-bench-test-before-truck-install)

---

### Steps 6–7 → Computer setup and firmware

You are **loading a program** onto the XIAO so it can speak Bluetooth. You are not “coding” — you run one prepared script.

1. Read **[Install PlatformIO](docs/ASSEMBLY.md#install-platformio)** in the assembly guide (Mac: Homebrew; Windows: installer from platformio.org).
2. Plug in the XIAO with a **USB-C data cable** (not charge-only).
3. Follow **[Upload / flash](docs/ASSEMBLY.md#upload)** or use the [script instructions below](#running-the-flash-script-first-timers).

**Next:** [Verify BLE on your phone →](docs/ASSEMBLY.md#11-verify-ble)

---

### Steps 9–10 → Case and truck

**[Enclosure assembly](docs/ASSEMBLY.md#9-enclosure-assembly-v3-case)** — LiDAR on **2 mm screws**, electronics inside, lid against the truck bed, wire out the bottom.

**[Truck installation](docs/ASSEMBLY.md#12-truck-installation)** — fuse, strain relief, clear line of sight to the trailer.

---

## Running the flash script (first timers)

A **script** is a small file of instructions for your computer. You open a **terminal** (text window), go to the project folder, and paste one command. The computer builds and uploads the firmware to the XIAO.

### Mac

1. Open **Terminal** (Spotlight: type `Terminal`, press Enter).
2. Go to your project folder (adjust the path if your folder is on the Desktop):

```bash
cd ~/Downloads/GooseEye-main
```

If you unpacked the ZIP somewhere else, drag the folder into the Terminal window after typing `cd ` — it will fill in the path.

3. Allow the script to run (only needed once):

```bash
chmod +x scripts/flash-firmware.sh
```

4. Run the flasher:

```bash
./scripts/flash-firmware.sh
```

5. Wait until it says **Done**. If it cannot find the board, unplug/replug USB or see [Troubleshooting](docs/ASSEMBLY.md#13-troubleshooting).

**What `chmod` means:** “This file is allowed to run.” **What `./scripts/...` means:** “Run the flash script in the `scripts` folder.”

### Windows

1. Install [PlatformIO](https://platformio.org/install/cli) first (see assembly guide).
2. Open **PowerShell** or **Command Prompt**.
3. Go to the folder:

```powershell
cd Downloads\GooseEye-main
```

4. Run PlatformIO directly (Windows does not require `chmod`):

```powershell
cd firmware
pio run -t upload
```

If upload fails, open Device Manager, note the **COM port** (e.g. `COM5`), then:

```powershell
pio run -t upload --upload-port COM5
```

### Success check

- No red error text at the end.
- Optional: `cd firmware` then `pio device monitor` — you should see `[GooseEye] boot` in the log.

**Next step:** [Verify BLE with your phone →](docs/ASSEMBLY.md#11-verify-ble)

---

## Words you might see (quick glossary)

| Term | Plain meaning |
|------|----------------|
| **GitHub** | Website where this project files are hosted |
| **Repository / repo** | This project’s folder of files |
| **Firmware** | Program inside the XIAO |
| **BLE / Bluetooth** | How the sensor talks to the phone |
| **Buck converter** | Lowers 12 V to 5 V |
| **MOSFET module** | Electronic switch that turns LiDAR power on/off |
| **U.FL antenna** | Small snap-on WiFi antenna for Bluetooth range |
| **PlatformIO** | Free tool that installs and uploads firmware |
| **Terminal** | Mac text window for typing commands |
| **D9, D10, D1** | Pin names printed on the XIAO board |
| **5V / VBUS** | Pad where regulated 5 V power goes in |

---

## Reference documents

| Document | When to use it |
|----------|----------------|
| [docs/ASSEMBLY.md](docs/ASSEMBLY.md) | Main build manual (all steps) |
| [docs/wiring-diagram.md](docs/wiring-diagram.md) | While soldering |
| [docs/v3 Case.step](docs/v3%20Case.step) | 3D print the enclosure |
| [docs/ble-protocol.md](docs/ble-protocol.md) | Only if you integrate without the official app |
| [firmware/](firmware/) | Source code (you do not need to edit this) |

---

## Help

- **Stuck on a step?** [Troubleshooting](docs/ASSEMBLY.md#13-troubleshooting)
- **Issue with this documentation?** Open an issue on [github.com/cjevolutions/GooseEye/issues](https://github.com/cjevolutions/GooseEye/issues)

---

## For maintainers (skip if you are building hardware)

Sync firmware from the private monorepo: `./scripts/prepare-publish.sh`  
Push notes: [PUSH.md](PUSH.md)
