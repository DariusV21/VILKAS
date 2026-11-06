# Driving with a phone (RX Wi-Fi mode)

Every VILKAS-RX (firmware **4.30 or newer**, any board revision) can be driven from a phone, with no remote and no app. The robot creates its own Wi-Fi network, and the phone opens a welcome page by itself, like hotel Wi-Fi.

This is handy for testing, demos, workshops, or simple starter robots. At events, use a VILKAS-TX.

## Requirements

| | |
|---|---|
| RX firmware | ≥ 4.30. This page describes **5.36**. |
| Phone | Any phone or tablet with Chrome or Safari, in **landscape** |
| Remote | Not needed. A paired remote can start phone mode and later return the robot to remote control. |

## 1. Start Wi-Fi mode

Use one of these:

- **BIND button:** hold BIND on the robot for **≥ 5 s** while powering it up. Release it when the LED lights. Pairing is kept.
- **From a paired remote:** `ROBOT > AP-Mode`, then hold the joystick button 2 s. The robot must be standing still. The remote shows the network name, the address and, if set, the PIN.
- **Always on:** in *Advanced settings*, tick **Always start in Wi-Fi phone mode**. The robot then starts in phone mode at every power-up. A short BIND press at power-up still enters normal bind mode.

The LED double-blinks while the robot waits for a phone.

## 2. Connect

1. On the phone, join the Wi-Fi network **`VILKAS-<robot>`** (e.g. `VILKAS-LB-7K3QD`).
   - It's **open**, unless `ROBOT > AP Pass` is set to `PIN`. Then enter the 8-digit PIN shown on the remote's `AP-Mode` screen.
   - Starting Wi-Fi mode with BIND held for 5 s **always** gives an open network, so a forgotten PIN can't lock you out.
   - Only **one** device can join at a time, so nobody else can drive your robot while you're connected.
2. The phone shows the **VILKAS RX welcome page** by itself ("Sign in to network" on Android, a pop-up sheet on iPhone).
3. Tap **Open remote**. The robot tells the phone the network is fine, so the pop-up closes or shows *Done*.
4. Open **Chrome / Safari** at **http://4.3.2.1**. The drive page opens full screen. Turn the phone to landscape.
   - If the phone asks *Keep using this network?*, answer **Yes**.
   - If the address doesn't open, switch off mobile data.
   - The welcome page also has *Drive in this window instead* and *Settings*.

> **Older firmware:** RX 4.30–4.31 use the network `BB-UPD-<robot>` and the address **192.168.4.1**, and open the drive page directly inside the pop-up.
>
> **Why 4.3.2.1?** Newer Android phones (e.g. Samsung, Android 16) hide the sign-in page when the network uses a private address such as 192.168.x.x. The robot's network is closed, so nothing is sent to the internet.
>
> **US phones:** phones set to a US region can't see a Wi-Fi network on channel 13. For phone driving there, put the robot on channel 1, 5 or 9.

## 3. The drive page

| Area | Control |
|---|---|
| Right | **Drive joystick.** Touch anywhere and drag; the point you touch is the centre. Releasing stops the robot. |
| Left | **Weapon slider.** The badge on top shows which channel it drives (**W** orange, **Q** cyan) and its type. `HOLD` stays where you leave it; `SPRING` slowly returns to 0 when you let go. The value never jumps to your finger. An optional **push button** next to it drives the *other* channel. |
| Middle | Robot battery (V, %), link quality (% of commands received, round-trip time, Wi-Fi signal), a line per weapon channel (output %, type, slider / button / off, armed or safe), **FLIP**, ⚙ settings. |

The robot uses its own Speed, Turn, Expo, Trim, motor direction and limits, so it drives the same as with a remote.

| Channel / type | Slider | Push button (off / on) |
|---|---|---|
| **W · ESC** | Bottom = stop, top = max (W limits, spin-up ramp). HOLD only. | Stop / full (W max) |
| **W · SERVO** | Centre = 1500 µs, ends = 1000 / 2000 µs. HOLD or SPRING. | 1000 µs / 2000 µs |
| **Q · DC** (Rev D) | Centre = stop, up / down = forward / reverse in 10 % steps (Q min/max, ramp). HOLD or SPRING. | Stop / full forward (Q max) |

**⚙ settings** (stored in the robot; opening ⚙ stops the robot):

- **Slider controls W / Q**: the same setting as `Weapon > Out:` on the remote
- **W signal ESC / SERVO**: SERVO needs a second tap. Never select it with a spinner ESC connected.
- **Button** Off / On, and **Hold** (on while pressed) or **Toggle** (tap on, tap off)
- **W min/max** and **Q min/max**
- **Advanced settings** (hold 2 s)

## 4. Safety

- **Arming:** each weapon channel arms on its own. After connecting, a settings change or signal loss, a channel starts only once its control is back at rest (ESC slider at 0, SERVO / Q slider centred, button released).
- **Failsafe:** the phone sends 25 commands a second. If they stop for the robot's failsafe time (default 0.5 s: page closed, screen locked, out of range), the drive stops; the weapon stops 0.35 s later and has to be re-armed.
- **Disconnect:** leaving the page or losing the connection stops everything immediately.
- While the robot is moving, the web server serves no other pages, so a slow page can never delay the failsafe.
- Phone Wi-Fi is less reliable than ESP-NOW. Don't use phone mode in competition.

## 5. Phone mode and the remote (RX ≥ 5.35, TX ≥ 5.35)

- The paired remote keeps seeing the robot. Its home screen shows a blinking **`PHONE MODE`**, and from TX 5.36 `PHONE` instead of `ARMED` / `SAFE`.
- The remote **can't drive** the robot in phone mode, but robot settings in the remote's menu still work.
- **Back to the remote:** `ROBOT > AP-Mode` on the remote offers to return the robot to remote control (hold the joystick button 2 s; the phone must not be driving). This is one-time: with *Always start in Wi-Fi phone mode* ticked, the next power-up is phone mode again.
- The remote refuses `RADIO > Channel` while the robot is in phone mode.

## 6. Advanced settings page

Open it from ⚙ → **Advanced settings** (hold 2 s), or go to **http://4.3.2.1/update**. It shows:

- robot info: name, manufacturer prefix, board revision, firmware version
- **Always start in Wi-Fi phone mode** on/off
- **Firmware update**: upload an `_ota_` file (see [flashing.md](flashing.md)). *Allow maker tag change* is needed only to install a file with a different prefix.
- **Restart** (leave Wi-Fi mode) and **Back to drive**

If nothing happens for 10 minutes, the robot restarts into normal mode. This doesn't apply when *Always start in Wi-Fi phone mode* is on.

## Status LED in Wi-Fi mode

| LED | Meaning |
|---|---|
| Double blink, repeating | Waiting for a phone |
| On | Being driven by a phone |
| Fast flicker | Firmware is being written. Don't power off. |

## For developers: phone interface

The drive page talks to the robot over a WebSocket on **port 81** (`ws://4.3.2.1:81/`), using text frames. You can write your own controller app against it.

**Phone → robot**

| Message | Meaning |
|---|---|
| `C,<x>,<y>,<s>,<b>,<flip>,<t>` | Control, sent every 40 ms even when nothing moves. `x`, `y`: −1000..1000 (`y` + = forward, `x` + = right). `s`: slider, 0..1000 for ESC or −1000..1000 for SERVO/DC (0 = stop). `b`: button 0/1. `flip`: 0/1. `t`: phone time in ms (& 0xFFFFFF), echoed back for RTT. |
| `P,<id>,<value>` | Set a robot parameter (same IDs as the [protocol](protocol/VILKAS-protocol-v4.md#6-parameter-ids)) |
| `U,<btn>,<bmode>,<styleW>,<styleQ>` | Page layout: button on/off, 0 = hold / 1 = toggle, slider style per channel 0 = HOLD / 1 = SPRING |

**Robot → phone** (JSON): `{"t":"i"}` info, `{"t":"g"}` configuration (after connecting and after each change), `{"t":"s"}` telemetry every 100 ms (only in reply to control), `{"t":"r"}` reply to `P`, `{"t":"c"}` reason the connection was closed.

Mixing (Speed, Turn, Expo, Trim, FLIP) is done in the robot, so `x` / `y` are raw stick values. The connection is closed after 3 s without commands.
