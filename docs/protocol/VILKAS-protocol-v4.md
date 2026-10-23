# VILKAS radio protocol — v4.1

This document describes the radio link between VILKAS-TX (remote) and VILKAS-RX (receiver). Use it to build your own compatible receiver or remote. The reference C header is [`bb_protocol.h`](bb_protocol.h). If this text and the header disagree, the header wins.

| | |
|---|---|
| Protocol version | **4.1** (`BB_PROTO_MAJOR = 4`, `BB_PROTO_MINOR = 1`) |
| Official firmware | TX 4.x and RX 4.x (all 4.x versions work with each other). Current: TX 4.22, RX 4.32 |
| Transport | ESP-NOW over 2.4 GHz Wi-Fi PHY, Wi-Fi STA mode, unencrypted |
| Channels | Wi-Fi channels **1, 3, 5, 7, 9, 11, 13** (odd only) |
| Byte order | Little-endian, all structs packed (no padding) |

## 1. Packet header

Every packet starts with a 4-byte header:

| Offset | Type | Field | Value |
|---|---|---|---|
| 0 | u8 | `magic` | `0xB0` |
| 1 | u8 | `type` | Packet type, see below |
| 2 | u8 | `proto_major` | `4` |
| 3 | u8 | `proto_minor` | Sender's minor version (currently `1`) |

| `type` | Name | Direction | Rate |
|---|---|---|---|
| 1 | `PKT_OFFER` | RX → broadcast `FF:FF:FF:FF:FF:FF` | Every 150 ms while in pairing mode |
| 2 | `PKT_CONTROL` | TX → RX | Every 20 ms (50 Hz) |
| 3 | `PKT_TELEMETRY` | RX → TX | Every 100 ms |
| 4 | `PKT_PARAM_CMD` | TX → RX | On demand |
| 5 | `PKT_PARAM_REPLY` | RX → TX | In reply to `PKT_PARAM_CMD` |

## 2. Compatibility rules

These rules mean devices that are already sold don't need updating with every release:

1. The packet type comes from `header.type`, **never** from the packet length.
2. Packets are **append-only**. Existing fields are never removed, moved or redefined. A new field bumps `proto_minor` and must have a default meaning for `0`.
3. A receiver accepts any packet with `len >= *_MIN_LEN` (the v4.0 size). It reads the fields it knows and ignores extra bytes. A shorter (older minor) packet is zero-filled before use.
4. `proto_major` changes only when compatibility is impossible. **A different major means incompatible, so the packet is dropped.**
5. New robot settings are added as new parameter IDs (section 6), not as new packet fields.
6. New features are announced with `CAP_*` bits, not with version numbers.

The reference helpers `bbAccept()` and `bbCopy()` in the header implement rules 1, 3 and 4.

## 3. Packets

### 3.1 `PKT_OFFER` (13 bytes, min 13)

Broadcast by an RX that is in pairing mode.

| Offset | Type | Field | Notes |
|---|---|---|---|
| 0 | header | `h` | type = 1 |
| 4 | char[2] | `prefix` | Manufacturer prefix, `A–Z`/`0–9`, not null-terminated (e.g. `'D','V'`) |
| 6 | char | `hwRev` | Board revision `'A'..'Z'` |
| 7 | u16 | `fwVersion100` | RX firmware version × 100 (`420` = 4.20) |
| 9 | u32 | `caps` | `CAP_*` bits |

### 3.2 `PKT_CONTROL` (19 bytes, min 18)

Sent by the TX to the paired RX every 20 ms.

| Offset | Type | Field | Notes |
|---|---|---|---|
| 0 | header | `h` | type = 2 |
| 4 | i16 | `motor_left` | −4095..4095, **already mixed** by the TX. + = forward |
| 6 | i16 | `motor_right` | −4095..4095, already mixed. + = forward |
| 8 | u16 | `weapon` | 0..4095, full raw wheel travel. **The RX applies weapon limits.** |
| 10 | u8 | `btn` | Joystick button, 1 = pressed |
| 11 | u8 | `flip` | FLIP switch, 1 = active (already applied in the mix) |
| 12 | u8 | `cmd_channel` | 0 = no change. Odd 1..13 = RX switches to this channel and saves it |
| 13 | u8 | `fs_mot1` | 1 = stop left motor on link loss |
| 14 | u8 | `fs_mot2` | 1 = stop right motor on link loss |
| 15 | u8 | `fs_weapon` | 1 = stop weapon on link loss |
| 16 | u16 | `tx_time` | TX `millis() & 0xFFFF`, echoed back for round-trip latency |
| 18 | u8 | `wpn_live` | *(4.1)* 1 = `weapon` is the real wheel position. 0 = weapon must stop (TX in menu, or a 4.0 TX). |

`wpn_live` is needed for DC weapons. In DC mode `weapon = 0` means full reverse, so "stop" has to be sent separately.

### 3.3 `PKT_TELEMETRY` (21 bytes, min 21)

Sent by the RX to its paired TX every 100 ms.

| Offset | Type | Field | Notes |
|---|---|---|---|
| 0 | header | `h` | type = 3 |
| 4 | u16 | `rxVolt100` | Calibrated, filtered robot battery voltage × 100 (`840` = 8.40 V) |
| 6 | u8 | `packetCount` | Counter of received `PKT_CONTROL` (wraps at 255), used for drop rate |
| 7 | u16 | `fwVersion100` | RX firmware version × 100 |
| 9 | u16 | `echo_time` | Copy of the last received `tx_time` |
| 11 | char[2] | `prefix` | Manufacturer prefix |
| 13 | char | `hwRev` | Board revision |
| 14 | u8 | `flags` | `TLM_FLAG_*` |
| 15 | u16 | `paramRev` | Changes whenever any parameter changes. When it changes, the TX re-reads all parameters. |
| 17 | u32 | `caps` | `CAP_*` bits |

### 3.4 `PKT_PARAM_CMD` (12 bytes, min 12)

| Offset | Type | Field | Notes |
|---|---|---|---|
| 0 | header | `h` | type = 4 |
| 4 | u8 | `op` | 1 = GET, 2 = SET (and save to NVS) |
| 5 | u8 | `seq` | Request number, echoed in the reply |
| 6 | u16 | `id` | Parameter ID |
| 8 | i32 | `value` | Value for SET (ignored for GET) |

### 3.5 `PKT_PARAM_REPLY` (20 bytes, min 20)

| Offset | Type | Field | Notes |
|---|---|---|---|
| 0 | header | `h` | type = 5 |
| 4 | u8 | `seq` | From the request |
| 5 | u8 | `status` | `PARAM_ST_*` |
| 6 | u16 | `id` | Parameter ID |
| 8 | i32 | `value` | Current value (the new value after a SET) |
| 12 | i32 | `minVal` | Allowed minimum (the TX limits editing to this range) |
| 16 | i32 | `maxVal` | Allowed maximum |

## 4. Flags and capabilities

| Name | Value | Meaning |
|---|---|---|
| `CAP_PARAMS` | `0x00000001` | RX supports the parameter channel |
| `CAP_WIFI_UPDATE` | `0x00000002` | RX supports Wi-Fi mode (`PARAM_ACT_UPDATE_MODE`): firmware update, and from RX 4.30 also phone control |
| `CAP_WIFI_DRIVE` | `0x00000004` | *(RX 4.30)* In Wi-Fi mode the RX can be driven from a phone. See [phone-control.md](../phone-control.md). |
| `TLM_FLAG_DRV_DEFAULT` | `0x01` | Rate / S-Curve / Trim have never been saved in this robot. The TX copies its own values once. |
| `TLM_FLAG_WPN_SAFE` | `0x02` | *(4.1)* Weapon is disarmed and waits for the wheel to return to 0 % (ESC) or the centre (DC) |

## 5. Link behaviour

### 5.1 Pairing (binding)

1. **RX enters pairing mode.** Hold the RX BIND button while powering up (LOW for ≥150 ms within the first 600 ms). The RX clears the stored TX MAC and broadcasts `PKT_OFFER` every 150 ms on its current channel. The pairing window lasts 90 s.
2. **TX scans.** In pairing mode (hold BIND while powering up the remote) the TX steps through the odd channels and lists the robots it hears, sorted by RSSI. Previously saved robots are listed first.
3. **The user selects a robot.** The TX adds the RX MAC as an ESP-NOW peer and starts sending `PKT_CONTROL`.
4. **RX locks on.** While in pairing mode, the RX accepts the **first** valid v4 `PKT_CONTROL` from any MAC, saves that MAC to NVS and is paired from then on. After pairing, the RX accepts `PKT_CONTROL` and `PKT_PARAM_CMD` **only from the paired MAC**.

A paired RX doesn't broadcast offers. The TX can store up to 8 robots (MAC + channel + prefix + name) and switch between them without binding again.

### 5.2 Channels

- Only odd channels 1–13 are used.
- The TX can move a robot by sending `cmd_channel` in `PKT_CONTROL`. The RX saves the new channel and switches.
- If telemetry is lost (>2.5 s), the official TX searches the odd channels for its robot again.
- ESP-NOW peers are added with `channel = 0` ("send on the current channel").

### 5.3 Failsafe (RX)

| Time since last valid `PKT_CONTROL` | Action |
|---|---|
| 500 ms | Drive motors stop (if `fs_mot1` / `fs_mot2`) |
| 850 ms | Weapons stop (if `fs_weapon`): ESC → 1000 µs, SERVO → no pulses, DC → brake/coast |

After a weapon failsafe, after power-up, after the TX menu has been open and after a weapon output change, the weapon stays **disarmed** until the wheel is returned to 0 % (ESC) or the centre ±5 % (DC). While disarmed, `TLM_FLAG_WPN_SAFE` is set.

**A compatible RX must implement failsafe.** Combat robot rules require motors and weapon to stop within 1 s of link loss.

### 5.4 Parameter channel

- One request is in flight at a time. The TX retries every 150 ms (up to 8 tries) until a reply with the same `seq` arrives.
- If the RX is still processing a previous request, it ignores the new one (the TX will retry).
- Unknown ID → `PARAM_ST_UNKNOWN`, and the TX shows that menu item as **N/A**.
- When `paramRev` changes in telemetry, or the link is restored, the TX re-reads all IDs `1..PARAM_ID_LAST`.

### 5.5 Wi-Fi mode (firmware update and phone control)

`PARAM_ACT_UPDATE_MODE`, or holding BIND for ≥ 5 s at power-up, reboots the RX into Wi-Fi mode. In this mode it creates an open access point `VILKAS-<name>` on its radio channel (max. 1 client), with a captive portal at `4.3.2.1` (DNS for every name, plus DHCP option 114 per RFC 8910). RX ≤ 4.31 used `BB-UPD-<name>` and `192.168.4.1`.

- `/`: phone drive page (RX ≥ 4.30, `CAP_WIFI_DRIVE`), with a WebSocket on port 81
- `/update` (or `/advanced`): advanced settings and firmware upload

ESP-NOW control is not active in Wi-Fi mode. To return to normal mode, restart the robot. Details: [phone-control.md](../phone-control.md).

## 6. Parameter IDs

**IDs are never renumbered. New IDs are only added.**

| ID | Name | Range | Applied by | Notes |
|---|---|---|---|---|
| 1 | `PARAM_MOT_DIR` | bit0 = left REV, bit1 = right REV | RX | Change only while the robot is stopped |
| 2 | `PARAM_BATT_TYPE` | 0 = LiPo (4.20 V/cell), 1 = LiHV (4.35 V/cell) | TX display | |
| 3 | `PARAM_BATT_CELLS` | 2..4 | TX display | |
| 4 | `PARAM_VCAL` | 500..2000 (multiplier × 1000) | RX | Voltage calibration |
| 5 | `PARAM_SPD_LMT` | 0..100 % | RX | Drive speed limit |
| 6 | `PARAM_WPN_MIN` | 0..WPN_MAX % | RX | Channel **W** (ESC) output at wheel minimum. From RX 4.30, W only. |
| 7 | `PARAM_WPN_MAX` | WPN_MIN..100 % | RX | Channel **W** (ESC) output at wheel maximum. From RX 4.30, W only. |
| 8 | `PARAM_RATE` | 10..100 % | Stored in RX, applied by TX | Max rate |
| 9 | `PARAM_EXPO` | 0..100 % | Stored in RX, applied by TX | S-curve |
| 10 | `PARAM_TRIM` | −100..100 | Stored in RX, applied by TX | Steering trim |
| 11 | `PARAM_WPN_OUT` | 0 = W (ESC/SERVO), 1 = Q (DC) | RX | *(4.1)* Which weapon channel the remote's wheel (and the phone slider) controls. `maxVal = 0` if the board has no Q channel. Weapon disarms after a change. |
| 12 | `PARAM_DC_START` | 0..Q_MAX % | RX | Channel **Q** (DC) minimum: power of the first speed step |
| 13 | `PARAM_DC_REV` | 0/1 | RX | DC weapon direction (only while stopped) |
| 14 | `PARAM_DC_BRAKE` | 0 = coast, 1 = brake | RX | DC weapon stop behaviour |
| 15 | `PARAM_DC_RAMP` | 0..2000 ms | RX | DC weapon spin-up time 0→100 % (0 = no ramp) |
| 16 | `PARAM_PWM_SERVO` | 0 = ESC, 1 = SERVO | RX | *(RX 4.30)* Signal type on channel W (GPIO10). SERVO: 1500 µs = centre/stop, 1000–2000 µs, no ramp, no pulses when disarmed. Change only while stopped. A separate ID, so an older remote toggling `Out:` can never switch on SERVO by accident. |
| 17 | `PARAM_Q_MAX` | DC_START..100 % | RX | *(RX 4.30)* Channel **Q** (DC) maximum |
| `0x100` | `PARAM_ACT_UPDATE_MODE` | SET with key `0x54445055` ("UPDT") | RX | Action: RX reboots into Wi-Fi update mode. GET returns 0. |

`PARAM_ID_LAST` = 17. A remote reads IDs 1..17; an RX that doesn't know an ID replies UNKNOWN.

Status codes: `0` OK, `1` UNKNOWN, `2` RANGE (limits in the reply), `3` BUSY (e.g. robot moving), `4` READONLY.

## 7. Weapon channels (RX)

From RX 4.30 a VILKAS-RX has up to two independent weapon channels:

| Channel | Output | Boards | Modes |
|---|---|---|---|
| **W** | GPIO10 signal | all revisions | **ESC**: 50 Hz, 1000–2000 µs. Wheel 0 % = stop (1000 µs). Scaled into `WPN_MIN..WPN_MAX`, with ramp-up.<br>**SERVO** (`PARAM_PWM_SERVO = 1`): wheel centre = 1500 µs, ends = 1000 / 2000 µs, no ramp, no MIN/MAX. Disarmed or failsafe = no pulses. |
| **Q** | DRV8231 H-bridge (GPIO2 / GPIO8) | **Rev D** only | **DC**: wheel centre (2048) = stop, ends = full FWD / REV. 10 % steps from `DC_START` to `Q_MAX`, 2 % hysteresis, ramp `DC_RAMP`. Stop = coast (both IN LOW) or brake (both IN HIGH). |

- On Rev D, **W and Q work at the same time**, each with its own arming, filter, ramp and limits.
- The remote's wheel controls the channel selected by `PARAM_WPN_OUT`. The other channel stays stopped, or it can be controlled by the phone's button in Wi-Fi mode.
- The remote shows `Weapon > Out:` as ESC → DC → SERVO (TX 4.21). It shows `Q DC >` settings only for Rev D robots.

## 8. Minimal compatible receiver checklist

- [ ] Wi-Fi STA mode, ESP-NOW initialised, broadcast peer added
- [ ] Pairing mode: broadcast `PKT_OFFER` every ≤150 ms and pair with the first valid `PKT_CONTROL`
- [ ] Store the TX MAC (and channel) in non-volatile memory, and accept packets only from that MAC
- [ ] Drop packets with the wrong magic or major. Zero-fill short packets.
- [ ] Send `PKT_TELEMETRY` every 100 ms, echoing `tx_time` in `echo_time`
- [ ] Failsafe: stop drive and weapon within 1 s of link loss
- [ ] Reply `PARAM_ST_UNKNOWN` to unknown parameter IDs (or set `caps = 0`)
- [ ] Use your own manufacturer prefix

A working example is in [`examples/minimal_rx`](../../examples/minimal_rx/).
