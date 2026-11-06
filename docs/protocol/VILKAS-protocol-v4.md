# VILKAS radio protocol — v4.3

This document describes the radio link between VILKAS-TX (remote) and VILKAS-RX (receiver). Use it to build your own compatible receiver or remote. The reference C header is [`bb_protocol.h`](bb_protocol.h). If this text and the header disagree, the header wins.

| | |
|---|---|
| Protocol version | **4.3** (`BB_PROTO_MAJOR = 4`, `BB_PROTO_MINOR = 3`) |
| Official firmware | TX and RX 4.x / 5.x all work with each other. Current: TX 5.36, RX 5.36 |
| Transport | ESP-NOW over 2.4 GHz Wi-Fi PHY, Wi-Fi STA mode, unencrypted, Wi-Fi power save off |
| Channels | Odd Wi-Fi channels **1, 3, 5, 7, 9, 11, 13** are valid. The official remote offers the non-overlapping **1 / 5 / 9 / 13**. |
| Byte order | Little-endian, all structs packed (no padding) |
| Control rate | TX → RX every **10 ms** (100 Hz) from TX 5.10; 20 ms before |
| Telemetry rate | RX → TX every 100 ms (10 Hz) |

Minor version history: **4.0** (firmware 4.00), **4.1** (4.10: `wpn_live`), **4.2** (5.20: RSSI, channel acknowledgement, echo hold time), **4.3** (5.25: reset reason / count). New parameter IDs, capability and flag bits don't change the minor.

## 1. Packet header

Every packet starts with a 4-byte header:

| Offset | Type | Field | Value |
|---|---|---|---|
| 0 | u8 | `magic` | `0xB0` |
| 1 | u8 | `type` | Packet type, see below |
| 2 | u8 | `proto_major` | `4` |
| 3 | u8 | `proto_minor` | Sender's minor version (currently `3`) |

| `type` | Name | Direction | Rate |
|---|---|---|---|
| 1 | `PKT_OFFER` | RX → broadcast `FF:FF:FF:FF:FF:FF` | Every ≤ 150 ms while unpaired / in bind mode |
| 2 | `PKT_CONTROL` | TX → RX | Every 10 ms (TX ≥ 5.10) |
| 3 | `PKT_TELEMETRY` | RX → TX | Every 100 ms (also straight away after a link gap ≥ 250 ms) |
| 4 | `PKT_PARAM_CMD` | TX → RX | On demand |
| 5 | `PKT_PARAM_REPLY` | RX → TX | In reply to `PKT_PARAM_CMD` |

## 2. Compatibility rules

These rules mean devices that are already sold don't need updating with every release:

1. The packet type comes from `header.type`, **never** from the packet length.
2. Packets are **append-only**. Existing fields are never removed, moved or redefined. A new field bumps `proto_minor` and must treat `0` as "not present".
3. A receiver accepts any packet with `len >= *_MIN_LEN` (the v4.0 size). It reads the fields it knows and ignores extra bytes. A shorter (older minor) packet is zero-filled before use.
4. `proto_major` changes only when compatibility is impossible. **A different major means incompatible, so the packet is dropped.**
5. New robot settings are added as new parameter IDs (section 6), not as new packet fields.
6. New features are announced with `CAP_*` bits, not with version numbers.

The reference helpers `bbAccept()` and `bbCopy()` in the header implement rules 1, 3 and 4.

## 3. Packets

### 3.1 `PKT_OFFER` (13 bytes, min 13)

| Offset | Type | Field | Notes |
|---|---|---|---|
| 0 | header | `h` | type = 1 |
| 4 | char[2] | `prefix` | Manufacturer prefix, `A–Z`/`0–9`, not null-terminated (e.g. `'L','B'`) |
| 6 | char | `hwRev` | Board revision `'A'..'Z'` |
| 7 | u16 | `fwVersion100` | RX firmware version × 100 (`536` = 5.36) |
| 9 | u32 | `caps` | `CAP_*` bits |

### 3.2 `PKT_CONTROL` (19 bytes, min 18)

| Offset | Type | Field | Notes |
|---|---|---|---|
| 0 | header | `h` | type = 2 |
| 4 | i16 | `motor_left` | −4095..4095, **already mixed** by the TX. + = forward |
| 6 | i16 | `motor_right` | −4095..4095, already mixed. + = forward |
| 8 | u16 | `weapon` | 0..4095, full raw knob travel. **The RX applies weapon limits.** |
| 10 | u8 | `btn` | Joystick button, 1 = pressed |
| 11 | u8 | `flip` | FLIP, 1 = active (already applied in the mix) |
| 12 | u8 | `cmd_channel` | 0 = no change. Odd 1..13 = RX switches to this channel and saves it |
| 13 | u8 | `fs_mot1` | Always 1 from TX 5.00. **RX ≥ 5.00 ignores it**: failsafe always stops. |
| 14 | u8 | `fs_mot2` | As above |
| 15 | u8 | `fs_weapon` | As above |
| 16 | u16 | `tx_time` | TX `millis() & 0xFFFF`, echoed back for round-trip latency |
| 18 | u8 | `wpn_live` | *(4.1)* 1 = `weapon` is the real knob position. 0 = weapon must stop (menu open, or a 4.0 TX). |

### 3.3 `PKT_TELEMETRY` (27 bytes, min 21)

| Offset | Type | Field | Notes |
|---|---|---|---|
| 0 | header | `h` | type = 3 |
| 4 | u16 | `rxVolt100` | Calibrated, filtered robot battery voltage × 100 (`840` = 8.40 V) |
| 6 | u8 | `packetCount` | Counter of accepted `PKT_CONTROL` (wraps at 255) |
| 7 | u16 | `fwVersion100` | RX firmware version × 100 |
| 9 | u16 | `echo_time` | Copy of the last received `tx_time` |
| 11 | char[2] | `prefix` | Manufacturer prefix |
| 13 | char | `hwRev` | Board revision |
| 14 | u8 | `flags` | `TLM_FLAG_*` |
| 15 | u16 | `paramRev` | Changes whenever any parameter changes. The TX then re-reads all parameters. |
| 17 | u32 | `caps` | `CAP_*` bits |
| 21 | i8 | `rxRssi` | *(4.2)* RSSI of the last `PKT_CONTROL` at the robot, dBm (0 = unknown) |
| 22 | u8 | `channel` | *(4.2)* Robot channel. After a `cmd_channel` this is **already the new channel** (acknowledgement); the RX switches ~60 ms later. 0 = unknown. |
| 23 | u16 | `echoHoldMs` | *(4.2)* ms between receiving the echoed `PKT_CONTROL` and sending this packet. The TX subtracts it from the RTT. |
| 25 | u8 | `resetReason` | *(4.3)* Last **unexpected** RX reset: 0 none, 1 brown-out, 2 watchdog, 3 crash, 4 other |
| 26 | u8 | `resetCount` | *(4.3)* Unexpected resets since power-on |

### 3.4 `PKT_PARAM_CMD` (12 bytes, min 12)

| Offset | Type | Field | Notes |
|---|---|---|---|
| 0 | header | `h` | type = 4 |
| 4 | u8 | `op` | 1 = GET, 2 = SET |
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
| 8 | i32 | `value` | Current value (after a SET: the new value, or the old one if rejected) |
| 12 | i32 | `minVal` | Allowed minimum (the TX limits editing to this range) |
| 16 | i32 | `maxVal` | Allowed maximum |

## 4. Flags and capabilities

| Name | Value | Meaning |
|---|---|---|
| `CAP_PARAMS` | `0x00000001` | RX supports the parameter channel |
| `CAP_WIFI_UPDATE` | `0x00000002` | RX supports Wi-Fi mode (`PARAM_ACT_UPDATE_MODE`) |
| `CAP_WIFI_DRIVE` | `0x00000004` | *(RX 4.30)* In Wi-Fi mode the RX can be driven from a phone. See [phone-control.md](../phone-control.md). |
| `TLM_FLAG_DRV_DEFAULT` | `0x01` | Legacy: driving settings never saved. Not sent from RX 5.35. |
| `TLM_FLAG_WPN_SAFE` | `0x02` | *(4.1)* Weapon disarmed; waits for the knob to return to 0 % (ESC) or the centre (DC / SERVO) |
| `TLM_FLAG_LV_LIMIT` | `0x04` | *(RX 5.30)* Low-voltage limit active: drive and weapon limited to 50 % until the RX restarts |
| `TLM_FLAG_PHONE_MODE` | `0x08` | *(RX 5.35)* RX is in Wi-Fi phone mode. `PKT_CONTROL` is **ignored** for driving, but telemetry, `echo_time`, `rxRssi` and parameters still work. |

## 5. Link behaviour

### 5.1 Pairing (binding)

```
RX (bind mode)                         TX (robot list)
──────────────                         ───────────────
OFFER broadcast every ≤150 ms ─────►   hops channels, lists PREFIX-XXXXX
                                       user confirms → stores RX MAC + channel
                              ◄──────  CONTROL (unicast to RX MAC)
store sender MAC, accept only it
TELEMETRY every 100 ms        ──────►  link shown as connected
                              ◄──────  PARAM_CMD GET 1..PARAM_ID_LAST (again when paramRev changes)
PARAM_REPLY                   ──────►  menus show the robot's values
```

1. **RX enters bind mode.** Hold BIND while powering up (5 fast LED blinks), or power up an RX that has never been paired. The stored TX MAC is cleared.
2. **TX lists robots.** Hold BIND while powering up the remote, or `Pair Bot` / `RADIO > Robots` in the menu.
3. **RX locks on.** In bind mode the RX accepts the **first** valid v4 `PKT_CONTROL` from any MAC, saves it and from then on accepts `PKT_CONTROL` and `PKT_PARAM_CMD` **only from that MAC**.

A paired RX never broadcasts offers. The TX stores up to 8 robots (MAC + channel + prefix + name) and switches between them without binding again. Nothing can unbind an RX over the air.

### 5.2 Channels

- The TX moves a robot by putting the new channel in `cmd_channel`. RX ≥ 5.20 acknowledges with the new value in `TelemetryPacket.channel` and switches ~60 ms later. The TX switches as soon as it sees the acknowledgement; for an older RX it switches after 200 ms.
- If telemetry is lost for 0.7 s (or `PARAM_FAILSAFE` + 100 ms, whichever is longer), the official TX searches the channels for its robot, 80 ms per channel, returning to the robot's last channel every other step.
- ESP-NOW peers are added with `channel = 0` ("send on the current channel").

### 5.3 Failsafe (RX)

| Time since last valid `PKT_CONTROL` | Action |
|---|---|
| `PARAM_FAILSAFE` (default 500 ms; 200–1000) | Drive motors coast |
| `PARAM_FAILSAFE` + 350 ms (default 850 ms) | Weapons stop: ESC → 1000 µs, SERVO → no pulses, DC → coast or brake (`PARAM_DC_BRAKE`) |

From RX 5.00 the failsafe **always** stops; the `fs_*` flags are ignored. After a weapon failsafe, after power-up, after a reset, after the TX menu has been open and after a weapon output change, the weapon stays **disarmed** until the knob is returned to 0 % (ESC) or the centre ±5 % (DC / SERVO). While disarmed, `TLM_FLAG_WPN_SAFE` is set.

**A compatible RX must implement failsafe.** Combat robot rules require motors and weapon to stop within 1 s of link loss.

### 5.4 Parameter channel

- One request is in flight at a time. The TX retries every 150 ms (up to 8 tries) until a reply with the same `seq` arrives.
- If the RX is still processing a previous request, it ignores the new one (the TX will retry).
- Unknown ID → `PARAM_ST_UNKNOWN`, and the TX shows that menu item as **N/A**.
- When `paramRev` changes in telemetry, or the link is restored, the TX re-reads all IDs `1..PARAM_ID_LAST`.
- RX ≥ 5.25 applies a changed value immediately but writes it to flash only when the robot is stopped and the battery is above 3.3 V per cell.

### 5.5 Wi-Fi mode (firmware update and phone control)

`PARAM_ACT_UPDATE_MODE`, or holding BIND for ≥ 5 s at power-up, restarts the RX into Wi-Fi mode. It creates the access point `VILKAS-<name>` on its radio channel (max. 1 client; open, or protected by an 8-digit PIN when `PARAM_AP_PASS = 1`; a BIND-held start is always open) with a captive portal at **`4.3.2.1`**:

| Path | Content |
|---|---|
| `/portal` | Welcome page shown by the phone's sign-in sheet (RX 5.36) |
| `/` | Phone drive page (`CAP_WIFI_DRIVE`), WebSocket on port 81 |
| `/update` (or `/advanced`) | Advanced settings and firmware upload |

RX ≤ 4.31 used the network `BB-UPD-<name>` and the address `192.168.4.1`.

**Phone mode and the remote (RX ≥ 5.35).** In Wi-Fi mode the RX still listens on ESP-NOW. For its paired TX it keeps sending telemetry with `TLM_FLAG_PHONE_MODE` and answers parameters, but it **ignores `PKT_CONTROL`** for driving. `PARAM_ACT_EXIT_WIFI` restarts the RX once into normal mode.

## 6. Parameter IDs

**IDs are never renumbered. New IDs are only added.** `PARAM_ID_LAST` = 27.

| ID | Name | Range | Applied by | Since | Notes |
|---|---|---|---|---|---|
| 1 | `PARAM_MOT_DIR` | bit0 = left REV, bit1 = right REV | RX | 4.00 | `BUSY` unless stopped |
| 2 | `PARAM_BATT_TYPE` | 0 = LiPo (4.20 V/cell), 1 = LiHV (4.35 V/cell) | TX display | 4.00 | |
| 3 | `PARAM_BATT_CELLS` | 2..4 | TX display | 4.00 | |
| 4 | `PARAM_VCAL` | 500..2000 (× 1000) | RX | 4.00 | Voltage calibration. RX 5.00 restarts from 1.00. |
| 5 | `PARAM_SPD_LMT` | 0..100 % | RX | 4.00 | **Speed** (top speed) |
| 6 | `PARAM_WPN_MIN` | 0..WPN_MAX % | RX | 4.00 | Channel **W** (ESC) minimum |
| 7 | `PARAM_WPN_MAX` | WPN_MIN..100 % | RX | 4.00 | Channel **W** (ESC) maximum |
| 8 | `PARAM_RATE` | fixed 100 on RX ≥ 5.35 | — | 4.00 | Legacy Max Rate, folded into Speed |
| 9 | `PARAM_EXPO` | 0..100 % | Stored in RX, applied by TX | 4.00 | Expo (S-curve) |
| 10 | `PARAM_TRIM` | −100..100 | Stored in RX, applied by TX | 4.00 | Steering trim |
| 11 | `PARAM_WPN_OUT` | 0 = W, 1 = Q | RX | 4.10 | Which weapon channel the knob / phone slider drives. `maxVal = 0` without channel Q. `BUSY` unless stopped. |
| 12 | `PARAM_DC_START` | 0..Q_MAX % | RX | 4.20 | Channel **Q** (DC) minimum: power of the first 10 % step |
| 13 | `PARAM_DC_REV` | 0/1 | RX | 4.20 | DC weapon direction. `BUSY` unless stopped. |
| 14 | `PARAM_DC_BRAKE` | 0 = coast, 1 = brake | RX | 4.20 | DC weapon stop behaviour |
| 15 | `PARAM_DC_RAMP` | 0..2000 ms | RX | 4.20 | DC spin-up time 0 → 100 % (0 = none) |
| 16 | `PARAM_PWM_SERVO` | 0 = ESC, 1 = SERVO | RX | 4.30 | Signal type on channel W. SERVO: 1500 µs = centre/stop, 1000–2000 µs, no ramp, no pulses when disarmed. `BUSY` unless stopped. |
| 17 | `PARAM_Q_MAX` | DC_START..100 % | RX | 4.30 | Channel **Q** (DC) maximum |
| 18 | `PARAM_DRV_MIN` | 0..50 % | RX | 5.10 | **Min Start**: any non-zero drive command → this % .. 100 % (default 10) |
| 19 | `PARAM_TURN` | 10..100 % | Stored in RX, applied by TX / phone | 5.15 | **Turn**: steering sensitivity (turn axis only) |
| 20 | `PARAM_ESC_RAMP` | 0..2000 ms | RX | 5.20 | Channel W spin-up time 0 → 100 % (0 = none; use the ESC's own ramp). Default 1000. |
| 21 | `PARAM_REV_RAMP` | 0..200 ms | RX | 5.25 | Drive direction change ramps through 0 in this time (from full). Default 25. |
| 22 | `PARAM_DRV_BRAKE` | 0 = coast, 1 = brake | RX | 5.30 | Drive with a zero command. Failsafe always coasts. |
| 23 | `PARAM_LV_CUT` | 0 or 300..380 (V/cell × 100) | RX | 5.30 | Low-voltage limit; 0 = off. Below it for 0.3 s → 50 % power until restart, `TLM_FLAG_LV_LIMIT`. |
| 24 | `PARAM_W_RATE` | 50 / 100 / 200 / 400 Hz | RX | 5.30 | Channel W (ESC) signal rate. SERVO is always 50 Hz. `BUSY` unless stopped. |
| 25 | `PARAM_FAILSAFE` | 200..1000 ms | RX | 5.30 | Drive failsafe time; weapon +350 ms. Default 500. |
| 26 | `PARAM_AP_PASS` | 0 = open, 1 = PIN | RX | 5.35 | Wi-Fi network password. A BIND-held Wi-Fi start is always open. |
| 27 | `PARAM_AP_PIN` | 10000000..99999999 | RX (read-only) | 5.35 | 8-digit PIN created once by the robot. SET → `READONLY`. |

**Actions** (SET with a key performs the action; GET returns 0):

| ID | Name | Key | Since | Notes |
|---|---|---|---|---|
| `0x100` | `PARAM_ACT_UPDATE_MODE` | `0x54445055` ("UPDT") | 4.00 | Restart into Wi-Fi mode. `BUSY` unless stopped. |
| `0x101` | `PARAM_ACT_ESC_CAL` | `0x43435345` ("ESCC"), 0 = cancel | 5.35 | At the next **power-on** channel W outputs 2000 µs for 4 s, then 1000 µs for 4 s. Cancelled if not power-cycled within 60 s; skipped when USB is connected or no battery. `READONLY` in SERVO mode, `BUSY` unless stopped. |
| `0x102` | `PARAM_ACT_EXIT_WIFI` | `0x54495845` ("EXIT") | 5.35 | In phone mode: restart once into normal (remote) mode. `BUSY` while the phone drives. |

Status codes: `0` OK, `1` UNKNOWN, `2` RANGE (limits in the reply), `3` BUSY (e.g. robot moving), `4` READONLY.

## 7. Outputs (official RX)

### Drive

- 20 kHz, 10-bit PWM, **slow decay** (RX ≥ 5.10): forward holds IN1 high and PWMs IN2 (between pulses the motor is braked), reverse the other way round. Both low = coast.
- Any non-zero command gets at least `PARAM_DRV_MIN`. A direction change ramps through zero in `PARAM_REV_RAMP`. A zero command coasts or brakes (`PARAM_DRV_BRAKE`). Failsafe always coasts.

### Weapon channels

| Channel | Output | Boards | Modes |
|---|---|---|---|
| **W** | GPIO10 signal | all revisions | **ESC**: 50–400 Hz (`PARAM_W_RATE`), 1000–2000 µs. Knob 0 % = stop (1000 µs). Scaled into `WPN_MIN..WPN_MAX`, ramp `PARAM_ESC_RAMP`.<br>**SERVO** (`PARAM_PWM_SERVO = 1`): 50 Hz, knob centre = 1500 µs, ends = 1000 / 2000 µs, no ramp, no MIN/MAX. Disarmed or failsafe = no pulses. |
| **Q** | DRV8231 H-bridge (GPIO2 / GPIO8) | **Rev D** only | **DC**: knob centre (2048) = stop, ends = full FWD / REV. 10 % steps from `DC_START` to `Q_MAX`, 2 % hysteresis, ramp `DC_RAMP`, slow decay. Stop = coast or brake. |

- On Rev D, **W and Q work at the same time**, each with its own arming, filter, ramp and limits.
- The remote's knob drives the channel selected by `PARAM_WPN_OUT`. The other channel stays stopped, or it can be driven by the phone's button in Wi-Fi mode.
- Weapon input smoothing is time-based (40 ms, RX ≥ 5.30).

## 8. Robot name

The 5-character ID shown after the prefix (e.g. `LB-7K3QD`) comes from the last 3 bytes of the RX Wi-Fi station MAC, 5 bits per character:

```c
static const char ALPHABET[] = "23456789ABCDEFGHJKLMNPQRSTUVWXYZ";
uint32_t v = (mac[3] << 16) | (mac[4] << 8) | mac[5];
for (int i = 4; i >= 0; i--) { id[i] = ALPHABET[v & 0x1F]; v >>= 5; }
```

The Wi-Fi network is always `VILKAS-` + prefix + `-` + ID. A name given on the remote replaces the label on that remote only.

## 9. Recognising old receivers (before v4)

Receivers before 4.0 have no header. They ignore v4 packets but still send telemetry to the remote they are bound to, so the official remote recognises them by length and shows `OLD RX FW!`:

| Packet | Firmware | Version field |
|---|---|---|
| 6-byte telemetry | RX 2.x | `uint16` at byte offset 2 |
| 17-byte telemetry | RX 3.00–3.02 | `uint16` at byte offset 3 |
| 31-byte telemetry | RX 3.10 (test builds) | `uint16` at byte offset 3 |
| 16-byte offer starting `"BOT_OFFER"` | RX 2.x / 3.x | `uint16` at byte offset 14 (3.x only) |

These checks are only applied to packets **without** a v4 header.

## 10. Checklists

### Compatible receiver

- [ ] Wi-Fi STA mode, ESP-NOW initialised, broadcast peer added
- [ ] Bind mode: broadcast `PKT_OFFER` every ≤ 150 ms and pair with the first valid `PKT_CONTROL`. Provide a way to clear the pairing (button at boot).
- [ ] Store the TX MAC (and channel) in non-volatile memory, and accept packets only from that MAC
- [ ] Drop packets with the wrong magic or major. Zero-fill short packets.
- [ ] Send `PKT_TELEMETRY` every 100 ms, echoing `tx_time` in `echo_time` and counting `packetCount`. The official TX shows `LINK LOST` after 0.5 s without telemetry.
- [ ] Follow `cmd_channel` (odd 1–13), save it, and ideally acknowledge it in `channel` before switching
- [ ] Failsafe: stop drive within 0.5 s and weapon within 1 s of link loss, regardless of `fs_*`
- [ ] Treat the weapon as stopped whenever `wpn_live` is 0
- [ ] Reply `PARAM_ST_UNKNOWN` to unknown parameter IDs (or set `caps = 0`)
- [ ] Use your own manufacturer prefix

### Compatible remote

- [ ] Hop channels and listen for `PKT_OFFER`; pair only with `proto_major = 4`
- [ ] Send `PKT_CONTROL` every 10–20 ms with mixed motor values, the raw knob value and `wpn_live = 1` (0 to stop the weapon)
- [ ] Read parameters with GET after connecting and whenever `paramRev` changes; respect `minVal` / `maxVal`
- [ ] Put a new channel in `cmd_channel` until it is acknowledged (or ~200 ms), then switch; send 0 otherwise
- [ ] If telemetry stops, search the channels until it returns
- [ ] Respect `TLM_FLAG_PHONE_MODE`: the robot is being driven from a phone

Working examples are in [`examples/`](../../examples/).
