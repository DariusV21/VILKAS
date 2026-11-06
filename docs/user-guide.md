# VILKAS user guide

For **TX 5.36 / RX 5.36**. Older firmware may not have every item; settings a robot doesn't support show `N/A` on the remote.

- [Quick start](#quick-start)
- [Installing the RX](#installing-the-rx)
- [Pairing and the robot list](#pairing-and-the-robot-list)
- [Using the remote](#using-the-remote)
- [Menu reference](#menu-reference)
- [Where settings are stored](#where-settings-are-stored)
- [Failsafe and safety](#failsafe-and-safety)
- [Tournament use: sharing remotes](#tournament-use-sharing-remotes)
- [Status indicators](#status-indicators)
- [Troubleshooting](#troubleshooting)

Driving with a phone: [phone-control.md](phone-control.md). Updating firmware: [flashing.md](flashing.md).

---

## Quick start

1. Install the RX in your robot (see [below](#installing-the-rx)). Leave the weapon disconnected for the first test.
2. Turn the **weapon knob fully to zero**.
3. Hold **BIND** on the remote and switch it on. Release the button. The robot list opens. (Or, from the menu: `Pair Bot`.) If the remote has never been calibrated, it offers stick calibration first.
4. Put the RX into bind mode: hold its BIND button while powering the robot. The LED blinks 5 times quickly. (A brand-new RX that has never been paired goes into bind mode on its own.)
5. Pick your robot (e.g. `LB-7K3QD`) with the joystick and press the joystick button or **OK**, then **hold the joystick button for 2 s** to confirm. The remote beeps.
6. The home screen shows the robot name with signal bars, and the RX LED is solid. Drive.

From now on the pair reconnects automatically at every power-up, and the robot stays in the remote's list.

---

## Installing the RX

| RX connection | Connect to |
|---|---|
| Battery in | Robot battery, 2S–4S LiPo / LiHV |
| Left motor | Left drive motor |
| Right motor | Right drive motor |
| Weapon (W) | Signal wire of your weapon ESC, or a servo |
| Weapon Q (Rev D only) | Brushed DC weapon motor (driven by the on-board H-bridge) |
| BIND | On-board button |

- If a motor spins the wrong way, **don't rewire it**. Fix it in `ROBOT > Motor L` / `Motor R`. The setting is stored in the RX, so it stays correct with any remote. The RX only applies a direction change while the motors are stopped.
- Calibrate your ESC's throttle range to 1000–2000 µs. The remote can do it for you: `ROBOT > Weapon > ESC Cal`.
- Pinouts and electrical data: [VILKAS-RX hardware](hardware/VILKAS-RX.md).

---

## Pairing and the robot list

An RX remembers **one remote** (by its radio MAC address). Once bound, it ignores every other remote until it is put into bind mode again. A remote remembers **up to 8 robots**.

**Opening the robot list on the remote**, either:

- hold **BIND** while switching the remote on (clears the current robot, keeps the list), or
- in the menu, `Pair Bot` (no robot selected) or `RADIO > Robots` (keeps the current robot until you pick another).

**Putting an RX into bind mode**, either:

- hold its BIND button while powering on (5 fast LED blinks confirm), or
- power on an RX that has no saved remote.

### The robot list

```
BOTS   Ch5          channel being scanned
 MY-BOT    <        saved robot, currently selected
 LB-7K3QD  *        saved robot
 LB-Q2M8F  ▂▄▆      new robot in bind mode, signal strength
 DV-9XRTA  !        robot with old firmware: can't be paired
```

- **Saved robots** are listed first, most recently used at the top. They show `<` / `*` instead of signal bars.
- **New robots** in bind mode follow, strongest signal first.
- Joystick up/down moves, joystick button or **OK** selects. On the confirmation screen, **hold the joystick button for 2 s**. A short **BIND** press goes back to the list; holding BIND for 1 s leaves the list.

### Switching between saved robots

Pick the robot in `RADIO > Robots` and hold the joystick button for 2 s. No bind mode is needed. If the robot has moved channel, the remote finds it automatically.

When the list is full, a new robot replaces the one used least recently. `RADIO > Forget` removes the current robot from the remote.

> **Forget only affects the remote.** The robot stays bound to this remote until it is put into bind mode. An RX can't be unbound over the air.

---

## Using the remote

| Control | Function |
|---|---|
| Joystick | Drive. Up/down = forward/back, left/right = turn. Mixing is done in the remote. |
| Weapon knob | **ESC:** weapon power from 0 %, scaled between the robot's W limits (default 0–30 %). **DC (Rev D) / SERVO:** centre = stop, ends = full forward / reverse. |
| FLIP | Reverses forward/back so an upside-down robot drives the same as the right way up. Steering is unchanged. |
| Joystick button | Confirm actions. On the home screen, hold 1 s to start / reset the match timer. |
| OK (hold 0.5 s) | Open the menu |
| BIND | Back / cancel in menus |

### Home screen

```
LB-7K3QD     ▂▄▆█    robot name (or status) · signal bars
TX ▮▮▯ 82 RX ▮▮▯ 64   remote / robot battery %
──────────────────
 3:00       ARMED    match timer · weapon ARMED / SAFE
            8ms      latency (FLIP when flipped, Ch# when offline)
```

- **Match timer:** hold the joystick button for 1 s to start; hold again to reset. Double beep at 1:00 and 0:30, a tick every second for the last 10 s, a long signal at 0:00. Set the length in `REMOTE > Timer`; with `Timer OFF` the big number shows the weapon knob in %.
- **Signal bars** show the weaker direction of the link (robot ↔ remote) and packet loss.
- **Batteries:** a number blinks when that battery is at 20 % or below. On USB with the power switch off, the remote's battery icon shows `CH` (charging).
- `SAFE` = weapon disarmed: return the knob to 0 % (ESC) or the centre (DC / SERVO). A chirp sounds when the weapon arms. `PHONE` = the robot is in phone mode.
- Rising chime = robot connected; falling tone + blinking `LINK LOST!` = robot lost.

| Status in the name line | Meaning |
|---|---|
| `NO ROBOT` | No robot selected. Menu → `Pair Bot`. |
| `SEARCHING` | Selected robot not answering yet; the remote is searching the channels |
| `LINK LOST!` (blinking) | The robot stopped answering for 0.5 s. The remote keeps searching and chimes when it's back. |
| `PHONE MODE` (blinking) | The robot is in Wi-Fi phone mode. The remote can't drive it; `ROBOT > AP-Mode` brings it back. |
| `BROWNOUT!` / `RX WDOG!` / `RX CRASH!` (8 s) | The robot reset unexpectedly (battery sag, watchdog, crash) and is back. Repeated `BROWNOUT!` = battery, wiring or current problem. |
| `LOW V LIM!` | Robot battery fell below `Cut-off`; drive and weapon are limited to 50 % until the robot restarts |
| `HOPPING...` | A channel change is in progress |
| `OLD RX FW!` | The robot's firmware is too old (before 4.0). Update the RX. |
| `UPDATE TX!` | The robot is newer than this remote. Update the remote. |

> While the menu is open the remote sends **zero drive and a weapon stop**. The weapon has to be re-armed afterwards.

---

## Menu reference

Open with **OK held 0.5 s**. Joystick **up/down** moves; **left/right changes the value** shown on the right of a line. **OK / joystick button** opens a submenu, runs an action or opens a full-screen editor (with a graph where there is one). **BIND** goes back.

Top level with a robot selected: `ROBOT`, `REMOTE`, `RADIO`, `SYSTEM`. Without a robot: `Pair Bot`, `REMOTE`, `SYSTEM`.

### ROBOT (stored in the robot)

A value shows `--` while it's being read and `N/A` if the robot's firmware doesn't have it.

| Item | What it does |
|---|---|
| `Speed` | Top speed, %. OK opens the response graph. |
| `Turn` | Steering sensitivity, 10–100 % (default 100). Lower it if the robot spins too fast. |
| `Expo` | Softer response near the centre, full power at the ends |
| `Trim` | Corrects a robot that pulls to one side |
| `Min Start` | Power at the smallest stick movement, 0–50 % (default 10) |
| `Stop` | `Coast` / `Brake` with the stick centred. Failsafe always coasts. |
| `Failsafe` | 250 / 500 / 1000 ms without signal before the drive stops (weapon 350 ms later) |
| `Rev Ramp` | Full-power direction changes ramp through zero in OFF / 25 / 50 / 100 ms (default 25). Protects the battery from brown-outs. |
| `Motor L` / `Motor R` | Motor direction `FWD` / `REV` (only while stopped) |
| `Weapon >` | See below |
| `Battery >` | Battery type (LiPo 4.20 V / LiHV 4.35 V), cells (2S–4S), `Volt Cal` against a multimeter, `Cut-off` (OFF / 3.0–3.4 V per cell: below it, power is limited to 50 %), robot low-battery alert on/off |
| `Name` | Name the robot (up to 8 characters, on this remote) |
| `AP-Mode` | Put the robot into Wi-Fi mode for [phone driving](phone-control.md) and [firmware update](flashing.md) (hold joystick button 2 s; robot stopped). Shows network, address and PIN. In phone mode it offers to return the robot to the remote instead. |
| `AP Pass` | `Open` / `PIN`: password for the robot's Wi-Fi network |

**`Weapon >`**

| Item | What it does |
|---|---|
| `Out:` | What the knob drives: `ESC` (channel W), `DC` (channel Q, Rev D only) or `SERVO` (channel W with a servo signal; needs a 2 s confirmation). Only while stopped. |
| `Pot:` | `NORM` / `REV`, knob direction (stored in the remote; set by `Cal Knob`) |
| `W Limits` | Min / max power of channel W (ESC), default 0–30 % |
| `W Ramp` | ESC spin-up time OFF / 0.25 / 0.5 / 1 / 2 s. Use `OFF` if your ESC already ramps. |
| `W Rate` | ESC signal 50 / 100 / 200 / 400 Hz. Most modern ESCs (BLHeli, AM32, SimonK) accept 400 Hz. Check yours first. SERVO stays at 50 Hz. |
| `ESC Cal` | ESC throttle calibration. **Remove the belt / weapon first**, confirm twice, then unplug the robot battery and plug it back in within 60 s. The robot sends full throttle for 4 s, then stop for 4 s, then starts normally. ESC signal only. |
| `Q DC >` | Channel Q settings (shown only for Rev D robots): `Q Limits` (max, then min = power of the first 10 % step), `Dir:` NORM/REV, `Stop:` Coast (spinners) / Brake (lifters, arms), `Ramp:` OFF / 0.25 / 0.5 / 1 / 2 s |

### REMOTE (stored in the remote)

| Item | What it does |
|---|---|
| `Cal Stick` | Joystick calibration: centre → roll the stick around its edges 3× → push UP → push RIGHT |
| `Cal Knob` | Weapon knob calibration: min end → max end → centre → centre deadzone (0–25 %, for DC / SERVO) |
| `Deadzone` | Joystick centre deadzone (default ~2 %) |
| `Bright` | Screen brightness 1–3 |
| `Sound` | `OFF` / `LOW` / `MED` / `HIGH` |
| `Clicks` | Menu click sounds on/off |
| `Wep Tone` | Tone while the weapon runs, pitch rising with power |
| `Timer` | Match timer: OFF, 1:00, 2:00, 3:00 (default), 5:00 |
| `Idle` | OFF / 2 / 5 / 10 min: screen off and a reminder beep every 30 s |
| `Bat Alert` | Remote low-battery alarm on/off |
| `TX Cal` | Remote battery voltage calibration |

### RADIO

| Item | What it does |
|---|---|
| `Robots` | Open the robot list |
| `Channel` | Move the link to channel 1, 5, 9 or 13 (non-overlapping). The robot follows automatically. |
| `Ch Scan` | How busy channels 1 / 5 / 9 / 13 are (all 2.4 GHz traffic); `>` marks the quietest. The robot stops for ~1 s per scan. |
| `Forget` | Remove the current robot from the remote |

### SYSTEM

| Item | What it does |
|---|---|
| `Info` | TX and RX firmware, RX board revision, channel, signal (remote side / robot side, dBm), latency, link quality, unexpected resets `R<n>` |
| `Diagnostic` | Live raw inputs (Rev C: also the button-ladder voltage) |
| `Reset` | Factory reset of the remote's settings. The robot list and robot-side settings are kept. |

---

## Where settings are stored

| In the **robot (RX)** | In the **remote (TX)** |
|---|---|
| Motor direction, Speed, Turn, Expo, Trim, Min Start, Stop, Failsafe, Rev Ramp | Stick and knob calibration, deadzone, knob direction |
| Battery type, cells, voltage calibration, Cut-off | Remote battery calibration and alerts |
| Weapon output, W limits / ramp / rate, SERVO, all Q settings | Sound, clicks, weapon tone, brightness, idle, timer |
| Wi-Fi password / PIN, phone page layout, *Always start in Wi-Fi phone mode* | Robot list: MAC, channel, prefix and name of up to 8 robots |
| Radio channel, bound remote | — |

Robot settings changed from the remote are confirmed by the robot. If the robot rejects a change (e.g. a direction change while moving), the remote beeps and shows the robot's actual value.

---

## Failsafe and safety

| Output | Stops after | Stop state |
|---|---|---|
| Drive | `ROBOT > Failsafe` (default 0.5 s) | Coast |
| Weapon | 0.35 s later (default 0.85 s) | ESC: 1000 µs. SERVO: no pulses. DC: coast or brake (`Stop:`) |

- **Failsafe can't be switched off.** Drive and weapon always stop when the link drops.
- **Weapon arming:** after power-up, failsafe, a reset, leaving the menu or changing the weapon output, the weapon stays off until the knob is back at 0 % (ESC) or the centre (DC / SERVO).
- **Soft start:** the weapon ramps up gradually (`W Ramp`, Q `Ramp:`) but stops instantly when you lower the knob.
- **Fast recovery:** after a brown-out, watchdog or crash reset, the robot is back on the radio in ~0.2 s with all outputs stopped, and the remote shows why.
- **Low-voltage limit** (`Cut-off`): a nearly empty battery limits power to 50 % instead of browning out.
- **Boot safe:** all outputs start stopped at power-up.

Recommendations: always turn the weapon knob to zero before powering the robot, and do the first test with the weapon belt removed.

---

## Tournament use: sharing remotes

The system is designed for events where a few remotes are shared across many robots.

- **Before the event:** bind every robot to the remote once (up to 8 per remote).
- **At the event:** `RADIO > Robots` → pick the robot → hold the joystick button 2 s. The robot drives with its own settings, because they're stored in the robot.
- A robot bound to another remote has to be put into bind mode and picked again.
- Give robots readable names (`ROBOT > Name`) and spread them across channels 1, 5, 9 and 13 if the pits are crowded.
- Several robots bound to the same remote can be on at once; the remote only drives the selected one.

---

## Status indicators

### RX LED

| Pattern | Meaning |
|---|---|
| 1 long flash at power-up | Normal start |
| 5 fast blinks at power-up | Bind mode entered, old pairing cleared |
| 3 quick blinks at power-up | The last restart was unexpected (brown-out / watchdog / crash): check battery and wiring |
| Slow blink (1 Hz) | Waiting to be paired |
| Solid | Connected |
| Solid with a short flicker | Connected, weapon armed |
| Slow double blink | Connected, low-voltage limit active |
| Fast blink (2 Hz) | Paired but no signal (failsafe) |
| Fast blink ~4 s, then slow blink ~4 s at power-up | ESC calibration (full throttle, then stop) |
| Three quick blinks, repeating | Bound remote has old firmware (before 4.0) |
| Lights up after holding BIND 5 s at power-up | Release the button: entering Wi-Fi mode |
| Double blink, repeating | Wi-Fi mode, waiting for a phone |
| Solid (Wi-Fi mode) | Driven from a phone |
| Fast flicker | Writing new firmware. Don't power off. |

### Remote sounds

| Sound | Meaning |
|---|---|
| Rising chime | Robot connected |
| Falling tone | Robot lost (`LINK LOST!`) |
| Chirp | Weapon armed |
| Low triple tone | Robot reset unexpectedly |
| Three short high beeps, repeating | Robot battery low (faster when critical) |
| Two falling tones, repeating | Remote battery low |
| Two low tones and a longer lower one | Robot and remote firmware don't match |

---

## Troubleshooting

| Symptom | Fix |
|---|---|
| `SEARCHING` forever | Robot is off, out of range, or bound to another remote. Re-bind it. |
| `OLD RX FW!` / robot marked `!` | Update the robot's firmware. |
| `UPDATE TX!` | Update the remote. |
| RX LED triple-blinks repeatedly | The bound remote's firmware is older than 4.0. Update the remote. |
| A setting shows `N/A` | The robot's firmware doesn't have it yet. Update the robot over Wi-Fi. |
| Robot drives backwards or turns the wrong way | `ROBOT > Motor L / Motor R` (robot stopped) |
| Robot creeps or pulls to one side | `REMOTE > Cal Stick`, then `ROBOT > Trim` |
| Robot doesn't move at small stick movements | Raise `ROBOT > Min Start` |
| Repeated `BROWNOUT!` | Battery sag: check battery and wiring, raise `Rev Ramp`, set `Cut-off` |
| Weapon is weak | Default W max is 30 %. Raise it in `ROBOT > Weapon > W Limits`. |
| Weapon doesn't reach full / doesn't stop | `REMOTE > Cal Knob`, and run `ROBOT > Weapon > ESC Cal` |
| DC weapon hums but doesn't turn | Raise Q min in `ROBOT > Weapon > Q DC > Q Limits` |
| Weapon won't arm (bar frame keeps blinking) | Return the knob to 0 % / the centre. If it still won't arm, run `REMOTE > Cal Knob`. |
| Low link quality, high latency | `RADIO > Ch Scan`, then move to the quietest channel with `RADIO > Channel` |
| Robot battery reading is off | `ROBOT > Battery`: set type and cells, then `Volt Cal` against a multimeter |
| `VILKAS-…` network doesn't appear | Hold BIND for the full 5 s until the LED lights, or use `ROBOT > AP-Mode`. US phones can't see channel 13. |
| Wi-Fi update refuses the file | Use the `_ota_` file for your board revision. A different prefix needs *Allow maker tag change*. |
| Status LED dark, right motor won't reverse | Wrong board revision firmware. Flash the matching `RevB` / `RevC` / `RevD` file over USB. |
