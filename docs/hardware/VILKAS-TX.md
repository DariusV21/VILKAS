# VILKAS-TX — handheld remote

> Items marked **TBD** still need to be measured or confirmed.

## Revisions at a glance

| Rev | Hardware | Firmware |
|---|---|---|
| B | In the field | ✅ `TX_Rev_B_V5.36.bin` |
| C | ✅ **Released** | ✅ `TX_Rev_C_V5.36.bin` (from 5.36) |

The remote has no hardware ID pin, so you must flash the image for your board's revision. Rev B firmware on a Rev C board (or the reverse) won't read the buttons correctly.

## Overview

| | |
|---|---|
| MCU | ESP32-C3 "OLED Supermini" module (ESP32-C3 + 0.42" 72 × 40 SSD1306 OLED, USB-C) |
| Main PCB size | **75.0 × 95.0 mm**, 1.6 mm, 2-layer (Rev C) |
| Inputs | Analog thumb stick with push button (Joy-Con-style module, 5-pin 0.5 mm FPC), weapon potentiometer knob, FLIP switch, MENU/OK and CANCEL/BIND buttons |
| Rev C extras | All push buttons on one ADC pin (resistor ladder), optional rotary encoder footprint (not used by the firmware yet), on-board charging (TP4056 module) |
| Output | Passive magnetic buzzer (12 × 8.5 mm, 16 Ω) |
| Battery | 1S Li-ion / LiPo |
| Firmware update | USB-C on the ESP32-C3 module (native USB) |
| Enclosure | 3D printed. STL files are in [`mechanical/enclosure/`](../../mechanical/enclosure/) |
| Mechanical models | [`mechanical/VILKAS-TX/`](../../mechanical/VILKAS-TX/) (Rev B and Rev C) |

## Electrical specification

| Parameter | Value | Notes |
|---|---|---|
| Battery | 1S, 3.0–4.2 V (LiPo) | Low-battery warning at 20 %, critical at 10 % |
| Regulator | AP2112K-3.3 LDO (600 mA) | |
| Battery sense | GPIO1. Rev B: 100 k / 10 k divider. Rev C: 100 k / 100 k (1:2). | Calibrated with `REMOTE > TX Cal`. Below ~3.4 V on USB the remote shows `CH` (charging). |
| Charging | Rev C: USB through TP4056 module | TBD: charge current |
| Battery capacity / runtime | TBD mAh / TBD h | |
| Radio | ESP-NOW, channels 1 / 5 / 9 / 13, 100 control packets/s | Range TBD m |

## Pinout

| GPIO | Rev B | Rev C | Notes |
|---|---|---|---|
| 0 | WEAPON_POT | WEAPON_POT | Analog |
| 1 | V_SENSE | V_SENSE | Analog (see divider above) |
| 2 | JOY_Y | **BTN_LADDER** | Rev C: all push buttons on one ADC pin (see below) |
| 3 | JOY_X | **JOY_Y** | Analog |
| 4 | BUZZER | **JOY_X** | Analog (Rev C) |
| 5 | LCD_SDA | LCD_SDA | On-module OLED, I²C |
| 6 | LCD_SCL | LCD_SCL | On-module OLED, I²C |
| 7 | BTN_CANCEL/BIND | **BUZZER** | Rev C: passive buzzer via NPN driver, 100 Ω series |
| 8 | EXT_SDA | EXT_SDA | External I²C expansion (Rev C: 4k7 pull-up). Strapping pin. |
| 9 | EXT_SCL | EXT_SCL | External I²C (Rev C: 4k7 pull-up). Strapping pin (BOOT). |
| 10 | BTN_MENU/OK | **ENC_B** | Rev C: optional encoder (10 k pull-up + RC filter) |
| 20 | BTN_FLIP | BTN_FLIP | Digital input |
| 21 | JOY_BTN | **ENC_A** | Rev C: optional encoder (10 k pull-up + RC filter) |
| 18 / 19 | USB D− / D+ | USB D− / D+ | USB-C on the module |

Rev B buttons are active-low with internal pull-ups.

### Rev C button ladder (GPIO2, 10 k pull-up, 10 nF)

| Button | Resistor to GND | Approx. voltage | Firmware threshold |
|---|---|---|---|
| MENU/OK | 0 Ω | 0 V | < 0.40 V |
| CANCEL/BIND | 3k3 | ~0.82 V | < 1.23 V |
| Joystick push | 10 k | ~1.65 V | < 1.95 V |
| Encoder push | 22 k | ~2.27 V | < 2.60 V |
| (none) | — | 3.3 V | above |

Only one button can be read at a time. Simultaneous presses read as the lowest-voltage button. `SYSTEM > Diagnostic` shows the live ladder voltage.

## Controls

| Action | Result |
|---|---|
| Hold CANCEL/BIND while powering up | Robot list: pair a new robot or switch to a saved one |
| Hold MENU/OK 0.5 s | Open the menu |
| Hold joystick button 1 s (home screen) | Start / reset the match timer |
| FLIP switch | Inverts forward/reverse for an upside-down robot |

Full menu reference: [user guide](../user-guide.md#menu-reference).

## Hardware revisions

| Rev | Status | Changes |
|---|---|---|
| B | In the field | Discrete buttons |
| C | **Released** | Button ladder on GPIO2, optional rotary encoder, joystick on GPIO3/4, buzzer moved to GPIO7, 1:2 battery divider, TP4056 charging, programming via module USB only |

## Expansion

EXT_SDA / EXT_SCL (GPIO8 / GPIO9) provide an I²C bus for add-ons (e.g. an IMU or a second display) when you run custom firmware. Both pins are strapping pins. Don't let an add-on pull GPIO9 LOW during reset, or the module boots into download mode.
