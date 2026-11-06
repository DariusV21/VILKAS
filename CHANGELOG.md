# Changelog

User-facing changes in official VILKAS firmware. Protocol changes are marked **[protocol]**.

## TX 5.36 / RX 5.36 — 2026-11-06

First public 5.x release. It contains everything from 5.00 to 5.36. Update the remote and the robots together for every feature; mixed 4.x / 5.x devices still work together. **TX Rev C is supported from this release**, and RX Rev B images are back.

### Radio and reliability
- **100 Hz control** (was 50 Hz). The display is drawn in slices, so it never delays a packet. Wi-Fi power save is off on both sides.
- **Failsafe always stops** (the S0/S1 option is gone). The time is configurable in `ROBOT > Failsafe`: 250 / 500 / 1000 ms; the weapon stops 350 ms later.
- The robot stays connected while you're in menus. A stuck button can't freeze the remote.
- Link alerts: a rising chime when the robot connects, a falling tone and `LINK LOST!` after 0.5 s without it.
- Faster reconnect: the remote starts searching after 0.7 s. Switching or pairing a robot no longer restarts the remote.
- **[protocol 4.2]** Real latency (the robot reports how long it held the packet), two-way signal bars, confirmed channel changes.
- **[protocol 4.3]** Fast recovery: after a brown-out, watchdog or crash the robot is back in ~0.2 s, and the remote shows `BROWNOUT!` / `RX WDOG!` / `RX CRASH!`. A 1 s watchdog guards the robot's control loop.
- Settings are written to flash only while the robot is stopped and the battery is healthy.
- `RADIO > Ch Scan` shows how busy channels 1 / 5 / 9 / 13 are. `Channel` offers only these four non-overlapping channels.

### Driving feel
- **Quiet, linear motors:** drive PWM is 20 kHz / 10-bit with slow decay (was 5 kHz / 8-bit).
- New robot settings: `Min Start` (power at the smallest stick movement), `Turn` (steering sensitivity), `Stop` (Coast / Brake), `Rev Ramp` (soft direction changes, protects against brown-outs). `Speed` replaces `Max Rate` + `Motr Spd`; RX 5.15 converts the old values once.
- Smoother sticks (4-sample averaging, jump-free deadzone). Easier stick calibration (centre → circle → up → right); an uncalibrated remote offers calibration by itself.

### Weapon
- `W Ramp` (ESC spin-up time, or OFF), `W Rate` (ESC signal 50 / 100 / 200 / 400 Hz).
- **`ESC Cal`:** ESC throttle calibration from the remote at the next power-up. Skipped on USB or without a battery.
- DC weapon (channel Q) uses slow decay. Time-based weapon input smoothing.

### Battery
- `Cut-off`: below the set voltage per cell, power is limited to 50 % until restart (`LOW V LIM!`). The limit survives a brown-out reset.
- More accurate robot voltage (factory-calibrated ADC). **Check `Volt Cal` once after updating from 4.x.**

### Remote
- New home screen: name, signal bars, battery icons, **match timer**, ARMED / SAFE, FLIP, latency.
- New flat menu `ROBOT` / `REMOTE` / `RADIO` / `SYSTEM`; values change directly in the list.
- `Sound` OFF / LOW / MED / HIGH, `Idle` reminder, weapon-armed chirp, timer beeps.
- Removed: `FIND BOT`, `WF Scanner`, the separate Buzzer / Display / Battery submenus.

### Wi-Fi and phone
- **Welcome page** in the phone's sign-in sheet; **Open remote** then drives full screen in Chrome / Safari.
- `ROBOT > AP Pass`: optional 8-digit PIN for the robot's network (a BIND-held start is always open).
- The remote sees a robot in phone mode (`PHONE MODE`) and can return it to remote control with `ROBOT > AP-Mode`.
- The Wi-Fi updater checks the maker tag (prefix). A file with another tag needs *Allow maker tag change*.

### Status LED
- Short flicker on solid = weapon armed. Slow double blink = low-voltage limit. Three quick blinks at power-up = the last restart was unexpected.

## RX 4.32 / TX 4.22 — 2026-10-23

- Wi-Fi network renamed **`VILKAS-<robot>`** (was `BB-UPD-<robot>`).
- New robot address **4.3.2.1** (drive page `http://4.3.2.1`, Advanced settings `http://4.3.2.1/update`). Newer Android phones (seen on a Samsung with Android 16) hide the sign-in page when the network uses a private address such as 192.168.4.1. The robot's network is closed, so nothing goes to the internet.
- The robot also announces its portal through DHCP (RFC 8910) and answers the captive-portal API (RFC 8908), for Android 11+ and iOS 14+.
- TX 4.22: the `RX AP-Mode` screen shows the right network name and address for the connected robot's firmware version.

## RX 4.31 — 2026-10-22

- New **Advanced settings** page (`192.168.4.1/update`, or ⚙ → hold 2 s on the drive page). It shows robot info and the manufacturer prefix, and has firmware upload, **Restart** and **Back to drive**.
- **Always start in Wi-Fi phone mode** option. A short BIND press at power-up still enters pairing mode.
- New firmware file names: `RX_Rev<R>_V<x.xx>_ota_<PFX>.bin` / `_usb_<PFX>.bin`.

## TX 4.21 — 2026-10-22

- `SYSTEM > RX Update` renamed to **`RX AP-Mode`** (it starts the robot's Wi-Fi mode: phone control and update).
- `Weapon > Out:` cycles **ESC → DC → SERVO** (only options the robot supports; SERVO asks for confirmation).
- Separate limits per weapon channel: `Weapon > W Limits` and `Weapon > Q DC > Q Limits`. `Q DC >` is shown only for Rev D robots.
- Fix: the joystick calibration "CAL DONE" screen now fits the display.
- Supports TX board **Rev B only**. Rev C is not supported yet.

## RX 4.30 — 2026-10-22

- **Phone control**: in Wi-Fi mode the robot opens a captive portal, and the phone shows a drive page by itself (joystick, weapon slider SPIN/LIFT, telemetry, FLIP). One phone at a time, with the same arming, limits and failsafe as with a remote. See [docs/phone-control.md](docs/phone-control.md).
- Firmware update moved to `192.168.4.1/update`.
- **Weapon channels W and Q work together** (Rev D): W = GPIO10 signal, Q = DC H-bridge. Each has its own arming, ramp and limits.
- **SERVO** signal mode on channel W (`PARAM_PWM_SERVO`): 1500 µs centre, no pulses when disarmed.
- **[protocol]** Additions only (still v4.1): `CAP_WIFI_DRIVE`, `PARAM_PWM_SERVO` (16), `PARAM_Q_MAX` (17). `PARAM_WPN_MIN/MAX` now apply to channel W only.

## TX 4.20 / RX 4.20 — 2026-10-12

- 3-point joystick calibration (end–centre–end). Default deadzone reduced.
- Weapon wheel calibration wizard (MIN → centre → MAX, direction detected automatically). Adjustable centre deadzone for DC weapons.
- Reorganised menu: `TX OPTS` (Cal Joy, Cal Wep, Battery, Buzzer, Display) and `RX OPTS` (Drive, Weapon, Failsafe, Battery).
- Drive-curve graph on the Max Rate / S-Curve / Deadzone screens. Steering trim L/R.
- WEP Sound: a tone whose pitch follows weapon power while the weapon is armed.
- DC weapon settings: start power, direction, coast/brake, ramp time (parameters 12–15).
- Protocol unchanged (4.1).

## TX 4.10 / RX 4.10 — 2026-10-12

- **[protocol]** v4.1: `ControlPacket.wpn_live`, `PARAM_WPN_OUT`, `TLM_FLAG_WPN_SAFE`. Compatible with 4.0.
- RX Rev D: DC weapon output (DRV8231). ESC or DC, selected from the menu.
- Weapon arming: after power-up, failsafe, menu or output change, the weapon only starts after the wheel returns to 0 % (ESC) or the centre (DC).

## TX 4.00 / RX 4.00 — 2026-10-12

- **[protocol]** v4: packet header, append-only packets and the parameter channel. All 4.x remotes and receivers work together from now on. **Not compatible with 3.x and older.**
- RX Wi-Fi update from a phone (BIND for 5 s at power-up, or `SYSTEM > RX Update`), with automatic rollback.
- Remote warns about old robot firmware, and the robot signals an old remote.
