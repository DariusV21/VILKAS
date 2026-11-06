# VILKAS system overview

```
  VILKAS-TX (remote)                               VILKAS-RX (robot)
 ┌────────────────────┐   PKT_CONTROL  10 ms     ┌──────────────────────┐
 │ joystick, knob,    │ ───────────────────────▶ │ drive L/R  (DRV8231) │
 │ FLIP, buttons      │   PKT_PARAM_CMD          │ weapon W (ESC/SERVO) │
 │ mixing, speed,     │ ◀─────────────────────── │ limits, failsafe,    │
 │ turn, expo, menu   │   PKT_TELEMETRY 100 ms   │ robot settings (NVS) │
 └────────────────────┘   PKT_PARAM_REPLY        └──────────────────────┘
          ESP-NOW, 2.4 GHz, channels 1 / 5 / 9 / 13

  Phone (browser)  ── Wi-Fi AP + WebSocket ──▶  VILKAS-RX in Wi-Fi mode (RX ≥ 4.30)
```

## Who does what

| Function | Where | Why |
|---|---|---|
| Stick reading, calibration, deadzone | TX | Depends on the remote's hardware |
| Mixing (arcade → left/right), FLIP | TX | The RX receives ready-made motor values |
| Turn, expo, steering trim | **Stored in RX**, applied by TX | Each robot keeps its own "feel" with any remote |
| Motor direction, speed, min start, stop mode, reverse ramp | RX | Set once per robot, valid with any remote |
| Weapon channels W (ESC/SERVO) and Q (DC, Rev D), limits, ramp | RX | Safety-relevant, so it belongs to the robot |
| Failsafe (configurable time, always stops), watchdog, low-voltage limit | RX | Must work even if the remote dies |
| Battery voltage | RX measures, TX displays and warns | |

The result is that **one remote can drive many robots**: at an event, 20 robots can share 5 remotes without being set up again. The remote can keep up to 8 robots in its list and switch between them without binding again.

## Typical use

1. **First pairing:** hold BIND on the robot while powering it up. On the remote, hold BIND while powering it up, then pick the robot from the list (it's shown as `<prefix>-<id>`, e.g. `LB-7K2QF`).
2. **Set up the robot once** in the remote's `ROBOT` menu (motor direction, speed, weapon output and limits, battery). The settings are saved in the robot.
3. **Drive.** The home screen shows signal, both batteries, the match timer, ARMED / SAFE and latency.
4. **No remote?** Hold BIND for 5 s while powering up the robot, connect your phone to `VILKAS-<name>` and drive from the page that opens. See [phone control](phone-control.md).
5. **Weapon arming:** after power-up, link loss or leaving the menu, the weapon only starts after the knob has been returned to 0 % (ESC) or the centre (DC / SERVO). The home screen shows `SAFE` until then.

## Versions

- Protocol **v4.x**: every 4.x / 5.x remote works with every 4.x / 5.x receiver. Settings that an older receiver doesn't know are shown as `N/A`.
- Remotes warn `OLD RX FW!` when a robot runs pre-v4 firmware, and robots triple-blink when the remote is too old.

See [firmware/README.md](../firmware/README.md) for the compatibility table, and the [user guide](user-guide.md) for daily use.
