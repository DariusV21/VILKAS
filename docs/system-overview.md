# VILKAS system overview

```
  VILKAS-TX (remote)                               VILKAS-RX (robot)
 ┌────────────────────┐   PKT_CONTROL  20 ms     ┌──────────────────────┐
 │ joystick, wheel,   │ ───────────────────────▶ │ drive L/R  (DRV8231) │
 │ FLIP, buttons      │   PKT_PARAM_CMD          │ weapon ESC / DC      │
 │ mixing, rates,     │ ◀─────────────────────── │ limits, failsafe,    │
 │ expo, trim, menu   │   PKT_TELEMETRY 100 ms   │ robot settings (NVS) │
 └────────────────────┘   PKT_PARAM_REPLY        └──────────────────────┘
          ESP-NOW, 2.4 GHz, odd channels 1–13

  Phone (browser)  ── Wi-Fi AP + WebSocket ──▶  VILKAS-RX in Wi-Fi mode (RX ≥ 4.30)
```

## Who does what

| Function | Where | Why |
|---|---|---|
| Stick reading, calibration, deadzone | TX | Depends on the remote's hardware |
| Mixing (arcade → left/right), FLIP | TX | The RX receives ready-made motor values |
| Max rate, S-curve, steering trim | **Stored in RX**, applied by TX | Each robot keeps its own "feel" with any remote |
| Motor direction, speed limit | RX | Set once per robot, valid with any remote |
| Weapon channels W (ESC/SERVO) and Q (DC, Rev D), limits, ramp | RX | Safety-relevant, so it belongs to the robot |
| Failsafe | RX | Must work even if the remote dies |
| Battery voltage | RX measures, TX displays and warns | |

The result is that **one remote can drive many robots**: at an event, 20 robots can share 5 remotes without being set up again. The remote can keep up to 8 robots in its list and switch between them without binding again.

## Typical use

1. **First pairing:** hold BIND on the robot while powering it up. On the remote, hold BIND while powering it up, then pick the robot from the list (it's shown as `<prefix>-<id>`, e.g. `DV-7K2QF`).
2. **Set up the robot once** in `RX OPTS` (motor direction, weapon output and limits, battery type). The settings are saved in the robot.
3. **Drive.** The main screen shows latency, link quality and the robot's battery level.
4. **No remote?** Hold BIND for 5 s while powering up the robot, connect your phone to `VILKAS-<name>` and drive from the page that opens. See [phone control](phone-control.md).
5. **Weapon arming:** after power-up, link loss or leaving the menu, the weapon only starts after the wheel has been returned to 0 % (ESC) or the centre (DC). The weapon bar blinks until then.

## Versions

- Protocol **v4.x**: any 4.x remote works with any 4.x receiver. Settings that an older receiver doesn't know are shown as `N/A`.
- Remotes warn `OLD RX FW!` when a robot runs pre-v4 firmware, and robots triple-blink when the remote is too old.

See [firmware/README.md](../firmware/README.md) for the compatibility table.
