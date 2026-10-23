<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="assets/logo/vilkas-horizontal-dark.svg">
    <img src="assets/logo/vilkas-horizontal.svg" alt="VILKAS" width="520">
  </picture>
</p>

<p align="center">
  <b>Low-cost radio control for antweight combat robots, built on the ESP32-C3.</b><br>
  Official hardware is available from <a href="https://lithuanianbots.com/"><b>Lithuanian Bots</b></a>.
</p>

# VILKAS

**VILKAS** is a low-latency 2.4 GHz radio control system for small combat robots. It is made up of:

| Unit | What it is |
|------|------------|
| **VILKAS-TX** | Handheld remote: joystick, weapon wheel, FLIP switch, OLED menu, buzzer, 1S battery |
| **VILKAS-RX** | 20 × 30 mm receiver/driver board for the robot: 2 × brushed drive motors, weapon signal output (ESC / SERVO), battery telemetry. Rev D adds a third H-bridge for a brushed DC weapon. |

It works like a Malenki Nano + FlySky FS-i6X combination, but it's cheaper. Robot settings (motor direction, speed and weapon limits, rates) are stored **in the robot**, so any VILKAS-TX can drive any VILKAS-RX without being set up again.

**No remote? Use a phone.** From RX firmware 4.30, any VILKAS-RX can be driven from a phone browser over the robot's own Wi-Fi, with no app. See [phone control](docs/phone-control.md).

## Where to get it

| | |
|---|---|
| <a href="https://lithuanianbots.com/"><b>Lithuanian Bots</b></a> | **Main distributor** of official VILKAS-TX remotes and VILKAS-RX receivers. Robots from these boards show up as `LB-…`. |
| DV Electronics | Designer and maker of VILKAS. Boards sold directly show up as `DV-…`. |

### Got a VILKAS from Lithuanian Bots? Start here

1. **Pair:** hold BIND on the robot while powering it up, then hold BIND on the remote while powering it up and pick your robot (`LB-…`). See the [system overview](docs/system-overview.md).
2. **No remote?** Hold BIND on the robot for 5 s at power-up, join the Wi-Fi network `VILKAS-LB-…` with your phone and drive. See [phone control](docs/phone-control.md).
3. **Update the firmware:** download it from [`firmware/RX_v4.32_LB/`](firmware/RX_v4.32_LB/) and follow [flashing](docs/flashing.md). Wi-Fi update needs no cable.

## Current status

| | Hardware | Firmware |
|---|---|---|
| VILKAS-RX Rev C | ✅ Shipping | ✅ RX 4.32 |
| VILKAS-RX Rev D (triple DC motor) | ⏳ Not built yet | ✅ Ready (RX 4.32) |
| VILKAS-TX Rev B | ✅ Shipping | ✅ TX 4.22 |

> Status: private preview. This repository will be made public.

## What this repository contains

| Folder | Contents | License |
|--------|----------|---------|
| [`docs/protocol/`](docs/protocol/) | Radio protocol specification (ESP-NOW, protocol v4.x) and the reference `bb_protocol.h` header | MIT (header), CC BY 4.0 (text) |
| [`docs/hardware/`](docs/hardware/) | Pinouts, electrical specifications and board dimensions for VILKAS-RX and VILKAS-TX | CC BY 4.0 |
| [`docs/phone-control.md`](docs/phone-control.md) | Driving a robot from a phone (RX ≥ 4.30), plus the phone WebSocket interface | CC BY 4.0 |
| [`docs/flashing.md`](docs/flashing.md) | How to flash official firmware, your own firmware, and how to go back | CC BY 4.0 |
| [`examples/`](examples/) | Minimal compatible RX and TX sketches (Arduino, ESP32-C3) | MIT |
| [`mechanical/`](mechanical/) | STEP models of the boards and STL files of the remote enclosure | CC BY 4.0 |
| [`firmware/`](firmware/) | Official compiled firmware images (`.bin`) with SHA-256 checksums | Free to use and redistribute unmodified (see [LICENSE.md](LICENSE.md)) |

**Not included:** the PCB design files (KiCad, Gerbers, BOM/CPL), the enclosure source CAD files, the full TX/RX firmware source code, and the programmer unit. These stay closed.

## Open to tinkerers

VILKAS hardware is **never locked**. Firmware flashing is always enabled, and you're encouraged to use it:

- Use VILKAS-RX/TX boards for other projects, such as mini-sumo robots, RC cars or a generic ESP32 handheld.
- Write your own firmware using the pinouts in `docs/hardware/`.
- Build your own compatible receiver or remote using the protocol in `docs/protocol/`.
- Go back to the official firmware at any time. The ESP32-C3 ROM bootloader can't be overwritten, so the board can't be bricked by flashing. See [docs/flashing.md](docs/flashing.md).

Third-party or modified firmware is welcome. However, only images published in `firmware/` are **official VILKAS firmware**, and events may require official firmware.

## Manufacturer prefixes

Every VILKAS-RX announces a 2-character manufacturer prefix (shown on the remote, e.g. `LB-7K2QF`): `LB` = Lithuanian Bots, `DV` = DV Electronics. If you build compatible receivers, please request a prefix so robots can be identified. See [docs/protocol/manufacturer-prefixes.md](docs/protocol/manufacturer-prefixes.md).

## Quick links

- [System overview](docs/system-overview.md)
- [Phone control](docs/phone-control.md)
- [Protocol v4 specification](docs/protocol/VILKAS-protocol-v4.md)
- [VILKAS-RX hardware](docs/hardware/VILKAS-RX.md)
- [VILKAS-TX hardware](docs/hardware/VILKAS-TX.md)
- [Firmware releases & compatibility](firmware/README.md)
- [Changelog](CHANGELOG.md)

---

<p align="center">
  Designed and made by<br><br>
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="assets/logo/dv-electronics-logo-white.png">
    <img src="assets/logo/dv-electronics-logo.png" alt="DV Electronics" width="180">
  </picture>
  <br><br>
  Distributed by <a href="https://lithuanianbots.com/"><b>Lithuanian Bots</b></a>
</p>
