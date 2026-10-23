# VILKAS-RX — receiver / driver board

A combined receiver, dual brushed motor driver and weapon output board for antweight (150 g / 200 g) combat robots.

> Items marked **TBD** still need to be measured or confirmed before the repository goes public.

## Revisions at a glance

| Rev | Hardware | Firmware | Notes |
|---|---|---|---|
| B | A few units in the field | Supported by the source, **no longer in official releases** (from 4.31) | Ask for a build if you have one |
| C | **Current, shipping** | ✅ `RX_RevC_…` | 2 drive motors + weapon signal (ESC / SERVO) |
| D | ⏳ **Not built yet**: "triple DC motor" board | ✅ **Ready** (`RX_RevD_…`, since 4.10, W + Q at once since 4.30) | Rev C + a third DRV8231 for a DC weapon (channel Q) |

## Overview

| | |
|---|---|
| MCU | ESP32-C3-MINI-1 (on-module PCB antenna) |
| Board size | **20.0 × 30.0 mm**, 1.6 mm, 4-layer (Rev C; Rev D TBD) |
| Weight | TBD g (bare board) |
| Drive | 2 × brushed DC, H-bridge TI DRV8231 (one per motor) |
| Weapon channel **W** | GPIO10 signal: **ESC** (50 Hz 1000–2000 µs) or **SERVO** (1500 µs centre). All revisions. |
| Weapon channel **Q** | Third DRV8231 H-bridge for a brushed DC weapon. **Rev D only.** Works together with W. |
| Telemetry | Battery voltage, link quality, firmware/board info |
| Indicators | Status LED (red), power LED (yellow) |
| Controls | BIND button |
| Control sources | VILKAS-TX remote (ESP-NOW), or a **phone** in Wi-Fi mode (network `VILKAS-<name>`, http://4.3.2.1, RX ≥ 4.30; see [phone-control.md](../phone-control.md)) |
| Firmware update | USB via the programming board, or over Wi-Fi from a browser |
| Mechanical model | [`mechanical/VILKAS-RX/`](../../mechanical/VILKAS-RX/) |

## Electrical specification

| Parameter | Value | Notes |
|---|---|---|
| Input voltage | 2S–4S LiPo / LiHV | Logic buck SY8120 (abs. max input 18 V) |
| Logic supply | 3.3 V, on-board buck | |
| Drive motor channels | 2 | DRV8231 per channel, 3.7 A peak (IC rating) |
| DC weapon channel Q (Rev D) | 1 | DRV8231, 3.7 A peak (IC rating). 5 kHz 8-bit. Coast or brake. |
| Continuous current per channel | TBD A | Depends on cooling. Confirm by testing. |
| Drive PWM | 5 kHz, 8-bit, fast decay | |
| Weapon signal W | 50 Hz, 3.3 V logic. ESC 1000–2000 µs (stop = 1000 µs) or SERVO 1000–2000 µs (stop = 1500 µs, no pulses when disarmed) | |
| ESC BEC / 5 V output | TBD | Whether the weapon connector provides power |
| Battery sense | GPIO0, divider ratio ≈ 1:10 | Calibrated from the remote (`PARAM_VCAL`) |
| Fuse | 0805 fuse F1 (0.75 A hold / 1.5 A trip) | TBD: which rail it protects |
| Failsafe | Drive stops at 500 ms, weapons at 850 ms | Same with a phone. See protocol §5.3. |

## Pinout

| GPIO | Rev B | Rev C | Rev D *(planned)* | Notes |
|---|---|---|---|---|
| 0 | BATT_SENSE | BATT_SENSE | BATT_SENSE | Analog, ≈ 1:10 divider |
| 1 | BIND | BIND | BIND | 10 k pull-up to 3V3, button to GND (active LOW) |
| 2 | — | — | **Q_FWD** (DRV8231 IN1) | Strapping pin, 10 k pull-up kept |
| 3 | R_MOTOR_FWD | R_MOTOR_FWD | R_MOTOR_FWD | PWM |
| 4 | L_MOTOR_REV | L_MOTOR_REV | L_MOTOR_REV | PWM |
| 5 | L_MOTOR_FWD | L_MOTOR_FWD | L_MOTOR_FWD | PWM |
| 6 | R_MOTOR_REV | **STATUS_LED** | STATUS_LED | Swapped in Rev C (GPIO6 is held high during boot, which made the motor twitch) |
| 7 | STATUS_LED | **R_MOTOR_REV** | R_MOTOR_REV | |
| 8 | — | — | **Q_REV** (DRV8231 IN2) | Strapping pin, 10 k pull-up kept |
| 9 | BOOT | BOOT | BOOT | Strapping pin. LOW at reset = ROM download mode |
| 10 | WEAPON W | WEAPON W | WEAPON W | ESC / SERVO signal |
| 18 / 19 | USB D− / D+ | USB D− / D+ | USB D− / D+ | Native USB of the ESP32-C3. Programming pads: TBD |

**Notes for custom firmware**

- On Rev D, GPIO2 and GPIO8 are both pulled HIGH at boot, which puts the Q driver in **brake**. That is the safe state. Keep them HIGH (brake) or LOW (coast) until your firmware decides otherwise.
- Drive the motor pins LOW as early as possible in `setup()`. Call `gpio_reset_pin()` on the pins that default to JTAG/other functions to avoid twitching at power-up.
- The ESP32-C3 has only **6 LEDC channels**. The official firmware uses 0–3 for drive, 4 for W (50 Hz / 14-bit timer) and 5 for Q (5 kHz / 8-bit timer). Q uses **one** LEDC channel that is routed through the GPIO matrix to Q_FWD or Q_REV depending on direction. The other pin is held LOW, or both pins are set HIGH for brake.

## LED and button

| Action | Result |
|---|---|
| Hold BIND while powering up (≥ 150 ms) | Pairing mode. The stored remote is forgotten and the robot shows up on any remote for 90 s. |
| Hold BIND for ≥ 5 s while powering up | **Wi-Fi mode**: phone control and firmware update. The LED lights, then you release the button. Pairing is kept. |
| Short BIND press at power-up, with *Always start in Wi-Fi phone mode* on | Skips Wi-Fi mode and enters normal pairing |
| Status LED, normal mode | TBD (solid = connected, 2 Hz blink = failsafe / no link, triple blink = remote firmware too old) |
| Status LED, Wi-Fi mode | Double blink = waiting for a phone, on = phone driving, fast flicker = writing firmware |

## Hardware revisions

| Rev | Status | Changes |
|---|---|---|
| B | Few units in the field | First released revision |
| C | **Current** | GPIO6 ↔ GPIO7 swapped (STATUS_LED / R_MOTOR_REV) to stop motor twitch at boot |
| D | **No hardware yet. Firmware ready.** | Rev C + third DRV8231 (channel Q, DC weapon) on GPIO2 / GPIO8. W and Q usable together. Needs RX ≥ 4.10 (W+Q together: ≥ 4.30). Board size and pin assignment may still change before release. |

The board revision is compiled into the firmware. Always flash the image that matches your board (`RX_RevC_…` for Rev C). The Wi-Fi updater refuses images built for a different revision.
