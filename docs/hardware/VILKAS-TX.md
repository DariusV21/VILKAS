# VILKAS-TX — handheld remote

> Items marked **TBD** still need to be measured or confirmed before the repository goes public.

## Revisions at a glance

| Rev | Hardware | Firmware |
|---|---|---|
| B | **Current, shipping** | ✅ `TX_Rev_B_V4.22.bin` |

A next revision is in development. Its hardware isn't ready yet, so it isn't documented here.

## Overview

| | |
|---|---|
| MCU | ESP32-C3 "OLED Supermini" module (ESP32-C3 + 0.42" 72 × 40 SSD1306 OLED, USB-C) |
| Main PCB size | **75.0 × 95.0 mm**, 1.6 mm, 2-layer |
| Inputs | Analog thumb stick with push button (Joy-Con-style module, 5-pin 0.5 mm FPC), weapon potentiometer wheel, FLIP switch, MENU/OK and CANCEL/BIND buttons. |
| Output | Passive magnetic buzzer (12 × 8.5 mm, 16 Ω) |
| Battery | 1S Li-ion / LiPo |
| Charging | USB |
| Firmware update | USB-C on the ESP32-C3 module (native USB) |
| Enclosure | 3D printed. STL files are in [`mechanical/enclosure/`](../../mechanical/enclosure/) |
| Mechanical models | [`mechanical/VILKAS-TX/`](../../mechanical/VILKAS-TX/) |

## Electrical specification

| Parameter | Value | Notes |
|---|---|---|
| Battery | 1S, 3.0–4.2 V (LiPo) | Low-battery warning at 20 %, critical at 10 % |
| Regulator | AP2112K-3.3 LDO (600 mA) | |
| Battery sense | GPIO1, divider ≈ 1:10 | Below 3.40 V the remote shows "Charging…" (USB powered, switch off) |
| Battery capacity / runtime | TBD mAh / TBD h | |
| Current consumption | TBD mA | |
| Radio | ESP-NOW, odd Wi-Fi channels 1–13 | Range TBD m |

## Pinout

The firmware is built per board revision (`TX_Rev_<R>_…`). There is no hardware ID pin, so you must flash the image for your board.

| GPIO | Function (Rev B) | Notes |
|---|---|---|
| 0 | WEAPON_POT | Analog |
| 1 | V_SENSE | Analog, ≈ 1:10 |
| 2 | JOY_Y | Analog |
| 3 | JOY_X | Analog |
| 4 | BUZZER | Passive buzzer via NPN driver |
| 5 | LCD_SDA | On-module OLED, I²C |
| 6 | LCD_SCL | On-module OLED, I²C |
| 7 | BTN_CANCEL/BIND | Digital input |
| 8 | EXT_SDA | External I²C expansion. Strapping pin. |
| 9 | EXT_SCL | External I²C. Strapping pin (BOOT). |
| 10 | BTN_MENU/OK | Digital input |
| 20 | BTN_FLIP | Digital input |
| 21 | JOY_BTN | Digital input |
| 18 / 19 | USB D− / D+ | USB-C on the module |

## Controls

| Action | Result |
|---|---|
| Hold CANCEL/BIND while powering up | Pairing mode: lists robots that are offering (saved robots first) |
| Hold MENU/OK | Opens the menu (only while paired) |
| FLIP switch | Inverts forward/reverse for an upside-down robot |
| `RX OPTS > Weapon > Out:` | Which robot weapon channel the wheel controls: ESC → DC → SERVO (TX ≥ 4.21; SERVO asks for confirmation) |
| `RX OPTS > Weapon > W Limits` / `Q DC > Q Limits` | Per-channel weapon limits (`Q DC >` is shown only for Rev D robots) |
| `SYSTEM > RX AP-Mode` | Switches the paired robot to Wi-Fi mode, for [phone control](../phone-control.md) and firmware update. Shows the network name and address for that robot (TX ≥ 4.22). |

## Hardware revisions

| Rev | Status | Notes |
|---|---|---|
| B | **Current, shipping** | Discrete buttons |
| next | In development | Not released. Hardware not ready. |

## Expansion

EXT_SDA / EXT_SCL (GPIO8 / GPIO9) provide an I²C bus for add-ons (e.g. an IMU or a second display) when you run custom firmware. Both pins are strapping pins. Don't let an add-on pull GPIO9 LOW during reset, or the module boots into download mode.
