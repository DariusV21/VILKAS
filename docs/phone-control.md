# Driving with a phone (RX Wi-Fi mode)

From **RX firmware 4.30**, every VILKAS-RX can be driven from a phone, with no remote and no app. The robot creates its own Wi-Fi network, and the phone opens the drive page automatically, like hotel Wi-Fi.

This is handy for testing, demos, kids' workshops, or robots that don't need a full remote. At events, use a VILKAS-TX.

## Requirements

| | |
|---|---|
| RX firmware | ≥ 4.30 (robot reports `CAP_WIFI_DRIVE`) |
| Phone | Any phone or tablet with a modern browser (Android / iOS), in **landscape** |
| Remote | Not needed. A paired TX ≥ 4.21 can start the mode from its menu (TX ≥ 4.22 shows the right network name and address for the robot). |

## Starting phone mode

Use one of these:

1. **BIND button:** hold BIND on the robot for **≥ 5 s** while powering it up. Release it when the LED lights.
2. **From a paired remote:** `SYSTEM > RX AP-Mode` (TX 4.21; called `RX Update` in TX 4.20).
3. **Always on:** in *Advanced settings*, tick **Always start in Wi-Fi phone mode**. The robot then starts in phone mode at every power-up. A short BIND press at power-up still enters normal pairing mode, which is how you get back to a remote.

Then:

1. On the phone, connect to the open Wi-Fi network **`VILKAS-<robot name>`** (e.g. `VILKAS-DV-7K2QF`).
2. The drive page opens by itself ("Sign in to network" on Android, a pop-up on iPhone). If it doesn't, open **http://4.3.2.1/** in the browser.

   > Robots on RX 4.31 or older use the network `BB-UPD-<robot name>` and the address **192.168.4.1**.
   >
   > Why 4.3.2.1? Newer Android phones (e.g. Samsung, Android 16) hide the sign-in page when the network's address is private, such as 192.168.x.x. The robot's network is closed, so nothing is sent to the internet. The robot also announces its portal through DHCP (RFC 8910) for Android 11+ and iOS 14+.
3. Turn the phone to landscape.

Only **one phone** can connect at a time. Once you're connected, nobody else can join and drive your robot. The network uses the robot's own radio channel.

## The drive page

| Area | Function |
|---|---|
| Right | **Drive joystick**: touch and drag |
| Left | **Weapon slider**: *SPIN* (stays where you leave it) or *LIFT* (springs back slowly to 0 when released) |
| Centre | Telemetry: battery voltage and %, link quality %, round-trip time, signal (dBm), weapon output %, armed/safe, FLIP |
| FLIP | Inverts drive for an upside-down robot |
| Button *(optional)* | Controls the **other** weapon channel (hold or toggle) |
| ⚙ | Robot settings: which channel the slider controls, W signal (ESC/SERVO), button on/off and mode, slider style, weapon limits. **Advanced settings** (hold 2 s). |

All settings are **saved in the robot**, because the captive-portal window on a phone can't store data.

Driving feels the same as with a remote. The robot uses its own Max Rate, S-Curve and Steering Trim, and the same weapon arming, ramp-up, limits and failsafe.

## Safety

- **Weapon arming** works as with a remote. After connecting, the weapon only starts once the slider has been returned to 0 (ESC) or the centre (SERVO / DC).
- **Failsafe:** the phone sends a command every 40 ms. If commands stop, drive stops after 500 ms and the weapon after 850 ms.
- **Disconnect:** if the phone disconnects, everything stops immediately and the weapon is disarmed. The connection is closed after 3 s without commands.
- While the robot is moving, the web server doesn't serve other pages, so a slow page can never delay the failsafe.
- Phone Wi-Fi is less reliable than ESP-NOW. Don't use phone mode in competition.

## Status LED in Wi-Fi mode

| LED | Meaning |
|---|---|
| Double blink | Waiting for a phone |
| On | Being driven by a phone |
| Fast flicker | Firmware is being written |

## Advanced settings page

Open it from ⚙ → **Advanced settings** (hold 2 s), or go to **http://4.3.2.1/update**. It shows:

- robot info (name, manufacturer prefix, board revision, firmware version),
- **Always start in Wi-Fi phone mode** on/off,
- **firmware update**: upload an `_ota_` file, see [flashing.md](flashing.md),
- **Restart**: leave Wi-Fi mode and start normally,
- **Back to drive**.

If nothing happens for 10 minutes, the robot restarts into normal mode. This doesn't apply when *Always start in Wi-Fi phone mode* is on.

## For developers: phone interface

The page talks to the robot over a WebSocket on **port 81** (`ws://4.3.2.1:81/`), using text frames. You can write your own controller app against it.

**Phone → robot**

| Message | Meaning |
|---|---|
| `C,<x>,<y>,<s>,<b>,<flip>,<t>` | Control, sent every 40 ms even when nothing moves. `x`, `y`: −1000..1000 (`y` + = forward, `x` + = right). `s`: slider, 0..1000 for ESC or −1000..1000 for SERVO/DC (0 = stop). `b`: button 0/1. `flip`: 0/1. `t`: phone time in ms (& 0xFFFFFF), echoed back for RTT. |
| `P,<id>,<value>` | Set a robot parameter (same IDs as the protocol, §6) |
| `U,<btn>,<bmode>,<styleW>,<styleQ>` | Page layout: button on/off, 0 = hold / 1 = toggle, slider style per channel 0 = HOLD / 1 = SPRING |

**Robot → phone** (JSON): `{"t":"i"}` info, `{"t":"g"}` configuration (after connecting and after each change), `{"t":"s"}` telemetry every 100 ms (only in reply to control), `{"t":"r"}` reply to `P`, `{"t":"c"}` reason the connection was closed.

Mixing (rate, expo, trim, FLIP) is done in the robot, so `x`/`y` are raw stick values.
