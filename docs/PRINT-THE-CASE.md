# How to print the v3 case (read this if the link looked like “code”)

If you clicked **`v3 Case.step`** on GitHub and saw **thousands of lines of text** — nothing is broken. That page is the raw 3D model data. **Do not try to read, edit, or copy it.** You are not supposed to understand that text.

**What you actually do:** save the file to your computer, open a **3D printer slicer** app, drag the file in, and print the plastic box (or send the file to a print shop).

**Previous:** [Shopping list](../README.md#parts-to-order-shopping-list) · **Next:** [Soldering](ASSEMBLY.md#7-soldering-the-harness)

---

## What is this file?

| | |
|---|---|
| **File name** | `v3 Case.step` |
| **Type** | STEP — a standard **3D shape** file (like a digital blueprint) |
| **Used for** | 3D printing the enclosure that holds the LiDAR and ESP32 |
| **Not used for** | Opening in a text editor, the firmware, or the phone app |

---

## Step 1 — Get the file on your computer

Pick **one** method.

### Method A — Download ZIP of the whole project (easiest on GitHub)

1. Go to [github.com/cjevolutions/GooseEye](https://github.com/cjevolutions/GooseEye).
2. Click green **Code** → **Download ZIP**.
3. Unzip the folder on your computer.
4. Open the folder → `docs` → you will see **`v3 Case.step`** (a single file, a few MB).

### Method B — Direct download (one file only)

1. Click this link (it should **download**, not show text in the browser):  
   **[Download `v3 Case.step`](https://github.com/cjevolutions/GooseEye/raw/main/docs/v3%20Case.step)**
2. If your browser still opens a text page: press **Ctrl+S** (Windows) or **Cmd+S** (Mac) and save as `v3 Case.step`, **or** right-click the link → **Save link as…**.

### Method C — You already cloned or downloaded the repo

The file is at:

```text
GooseEye/docs/v3 Case.step
```

(on your machine, inside whatever folder you unzipped or cloned)

---

## Step 2 — Open a slicer (printer software)

A **slicer** turns the 3D model into instructions your printer understands (G-code). You need **one** of these on a computer — install free if you do not have one:

| Slicer | Typical printers |
|--------|------------------|
| [Bambu Studio](https://bambulab.com/en/download/studio) | Bambu Lab |
| [PrusaSlicer](https://www.prusa3d.com/prusaslicer/) | Prusa, many others |
| [Cura](https://ultimaker.com/software/ultimaker-cura) | Creality, many others |
| MakerWorld / printer brand app | Varies |

**No 3D printer?** Skip to [Step 5 — Print service](#step-5--no-printer-use-a-print-service) below.

---

## Step 3 — Import the model (do not open in Notepad)

1. Launch your slicer.
2. **Drag `v3 Case.step` from your file manager into the slicer window.**  
   — or use **File → Import / Open** and choose `v3 Case.step`.
3. After a few seconds you should see a **3D view of a plastic box** — that is correct.

If the slicer says it cannot open STEP:

- Update the slicer to the latest version, **or**
- Open the file in [FreeCAD](https://www.freecad.org/) or [Tinkercad](https://www.tinkercad.com/), export as **STL**, then import the STL into your slicer.

---

## Step 4 — Set print orientation (important)

Place the model so the **large opening faces down onto the build plate** (the hole where wires exit points **down**; the LiDAR end points **up**).

```text
   build plate ═══════════════
        [ opening down ]
              ↑
         [ case body ]
```

This gives a flat base and fewer fragile overhangs. Your slicer may call this “lay flat on bed” — rotate the model until the open rim touches the plate.

**Material:** PETG or ASA recommended for sun/heat in a truck bed. PLA can work but may soften in hot weather.

Slice and print. Remove supports, clean the LiDAR screw holes and wire exit with a small knife or file if needed.

---

## Step 5 — No printer? Use a print service

1. Download `v3 Case.step` using [Step 1](#step-1--get-the-file-on-your-computer) above.
2. Upload the file to an online 3D printing service, for example:
   - [Craftcloud](https://craftcloud3d.com/)
   - [Shapeways](https://www.shapeways.com/)
   - Local makerspace / library with a printer
3. Choose **PETG** or **ASA** if offered for outdoor use.
4. Mention in notes: **opening must sit on the build plate** (same orientation as Step 4).

---

## Step 6 — After the print

Continue the main guide:

1. [Mount LiDAR with 2 mm screws](ASSEMBLY.md#lidar-mounting)
2. [Seal gaps with silicone / hot glue](ASSEMBLY.md#weather-sealing-silicone-andor-hot-glue)
3. [Full enclosure section](ASSEMBLY.md#9-enclosure-assembly-v3-case)

**Next in build order:** [§7 Soldering](ASSEMBLY.md#7-soldering-the-harness) (you can print the case while waiting for Amazon parts).

---

## Quick troubleshooting

| Problem | Fix |
|---------|-----|
| GitHub shows endless text | Normal — [download the file](#step-1--get-the-file-on-your-computer), do not read it in the browser |
| Slicer is empty after import | Zoom to fit; check you imported `.step` not a shortcut |
| Model is tiny or huge | Use slicer **scale** only if you know you need it — default should be mm |
| Ugly overhangs | Re-check orientation — **opening on the plate** |

**Back to start:** [README](../README.md)
