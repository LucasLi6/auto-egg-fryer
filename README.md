# Auto Egg Fryer

A fully automatic, unattended egg-frying machine. Drop in the eggs, press a button, and the
machine sprays oil, heats the pan, cracks the eggs, fries both sides, and flips the finished egg
onto your plate — all by itself.

The project integrates mechanical design, structural engineering, digital electronics, AC-motor
drive, induction heating, and embedded firmware written in Keil C for the **STM32F407ZG** MCU.

---

## Table of Contents

- [Features](#features)
- [System Architecture](#system-architecture)
- [Hardware Overview](#hardware-overview)
- [How It Works](#how-it-works)
- [Control Interface](#control-interface)
- [Repository Structure](#repository-structure)
- [Building the Firmware](#building-the-firmware)
- [Innovation Highlights](#innovation-highlights)
- [Future Work](#future-work)

---

## Features

- **Fully automatic** — completes nearly the entire frying process with no human intervention.
- **Precise motion control** — every motor reports its rotation angle back through Hall sensors,
  giving high accuracy and a low error rate.
- **High-efficiency heating** — dual induction cookers (upper continuous + lower intermittent)
  balance the heating so both sides of the egg are cooked evenly.
- **Solid-state relay (SSR) driven** — low-voltage MCU signals safely switch 220 VAC motors and
  induction cookers, enabling high power, high reliability, and low vibration.
- **Adjustable in the field** — a digital display and buttons let the operator tune heating time,
  oil-spray time, and per-unit run times for different egg sizes and types.
- **Robust construction** — the load-bearing frame is cut, bent, and welded steel plate.
- **Emergency stop** — a dedicated stop key interrupts and resets the machine for extreme cases.

---

## System Architecture

The machine is composed of several cooperating units:

| Unit | Function |
|------|----------|
| **Main control unit** | Self-designed CAM-16-13B MCU board with extra input/output interfaces and SSR driver boards, housed in a 20 × 12 cm metal box with a display and four buttons. |
| **Heating unit** | Two induction cookers above and below the pan. The lower cooker contacts the pan directly; the upper cooker heats through a suspended stainless-steel plate. |
| **Pan unit** | Horizontal- and vertical-flip AC motors position the pan across the oil, heating, and egg-opening stations, and later toss the cooked egg onto the plate. |
| **Egg-opening unit** | A motor-driven cracker pierces the shell, spreads it open to pour out the yolk and white, then flips 180° to discard the empty shell. |
| **Egg-placement unit** | Drops a single egg into the cracker on command, with a shutter plate that releases one egg and blocks the rest. |
| **Oil-spray unit** | A pump sprays a thin, even oil film into the pan to prevent sticking and to avoid oil build-up across repeated cycles. |

---

## Hardware Overview

- **MCU board:** CAM-16-13B (self-designed), based on the STM32F407ZG.
- **Power:** 220 VAC standard mains, single socket.
- **Heating:** two induction cookers switched by HK4100F-DC5V-SHC solid-state relays.
- **Motors:** 220 VAC / 50 Hz / 12 W AC gearmotors, switched through 8-channel G3MB-202P
  solid-state relays.
- **Position sensing:** A3144 Hall-effect sensors detect the neodymium magnet on each motor shaft,
  sending a low-level signal to the MCU when a rotation angle is reached. Every motor in the
  machine uses this principle.
- **Frame:** welded steel plate.
- **Dimensions:** base ≈ 30 × 30 cm, upper structure ≈ 40 × 45 cm, height ≈ 50 cm, weight ≈ 3 kg.

---

## How It Works

1. The machine is started.
2. The pan moves to the **oil station** and is sprayed with oil, then moves to the **heating
   station** to preheat.
3. The pan moves to the **egg-opening station**. The placement unit drops an egg into the cracker
   and resets.
4. The cracker opens the egg over the pan, discards the shell, and resets.
5. The pan returns to the heating station to fry the egg.
6. The pan moves back to the oil station, flips and tosses, and the finished egg drops onto the plate.

---

## Control Interface

Five buttons control the machine, laid out left to right:

| Button | Function |
|--------|----------|
| **Run** | Start operation. |
| **Stop** | Interrupt operation and reset the machine. |
| **Set** | Enter timing-adjustment mode. |
| **+ / −** | Increase / decrease the selected unit's run time (heating, oil-spray, etc.). |

---

## Repository Structure

```
.
├── CORE/        # Cortex-M4 core files + startup (STM32F407)
├── FWLIB/       # STM32F4 standard peripheral library (inc / src)
├── HARDWARE/    # Board-level drivers: KEY, LCD, LED, STMFLASH
├── SYSTEM/      # delay, sys, usart helpers
├── USER/        # main.c and the Keil µVision project (STMFLASH.uvprojx)
└── OBJ/         # Build outputs (ignored by git)
```

The firmware is a **Keil µVision (MDK-ARM)** project built on the STM32F4 standard peripheral
library. `USER/main.c` contains the machine's control logic, including 74HC595 shift-register
output driving for the relays/indicators and the per-unit state machine.

---

## Building the Firmware

1. Install **Keil MDK-ARM** (µVision 5) with the STM32F4 device support pack.
2. Open `USER/STMFLASH.uvprojx`.
3. Build the project, then flash the STM32F407ZG via your ST-Link / J-Link.

> The project file still carries the `STMFLASH` name from the original ALIENTEK template it was
> derived from; the application code inside has been rewritten for the egg fryer.

---

## Innovation Highlights

- Hall-sensor closed-loop angle control for stable, low-error motor positioning.
- Extensive use of solid-state relays to drive high-power loads efficiently and safely.
- Welded steel structure for strength and stability.
- Digital display + buttons for easy on-site tuning of operating parameters.
- Tightly integrated, interlocking units with high automation.
- Original two-sided heating and egg-loading mechanisms.
- Far higher frying throughput than manual operation, giving it strong market potential.

---

## Future Work

- Make the units more compact to reduce size and improve portability.
- Enlarge the egg-placement unit for greater capacity.
- Develop a next-generation machine with a rotary multi-pan design for uninterrupted heating and
  higher throughput, plus convenience features for mass adoption.
- Extend the platform toward an automatic egg-selling vending machine or a fully automatic pancake
  machine.
