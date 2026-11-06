# Official firmware

Official VILKAS firmware images. How to flash them: [docs/flashing.md](../docs/flashing.md). The same files are attached to each [GitHub Release](https://github.com/DariusV21/VILKAS/releases). License: free to use and redistribute unmodified (see [LICENSE.md](../LICENSE.md)).

## Current versions

| Unit | Version | Folder | Boards |
|---|---|---|---|
| VILKAS-RX from **[Lithuanian Bots](https://lithuanianbots.com/)** | **5.36** | [`RX_v5.36_LB/`](RX_v5.36_LB/) | Rev B ✅ · Rev C ✅ · Rev D ✅ (no hardware yet) |
| VILKAS-TX | **5.36** | [`TX_v5.36/`](TX_v5.36/) | Rev B ✅ · Rev C ✅ |

Update the remote and the robots together to get every 5.x feature. Mixed versions still work (see below).

**Which prefix?** The robot's name starts with it. `LB-…` boards are from Lithuanian Bots. For `DV-…` boards, ask DV Electronics for firmware.

```
firmware/
├── RX_v5.36_LB/
│   ├── RX_RevB_V5.36_ota_LB.bin   # Wi-Fi update (http://4.3.2.1/update)
│   ├── RX_RevB_V5.36_usb_LB.bin   # USB, flash at 0x0
│   ├── … same for RevC and RevD
│   └── SHA256SUMS.txt
├── TX_v5.36/
│   ├── TX_Rev_B_V5.36.bin         # USB, flash at 0x0
│   ├── TX_Rev_C_V5.36.bin
│   └── SHA256SUMS.txt
└── _archive/                      # previous versions
```

File name parts: `Rev<R>` = board revision, `V<x.xx>` = version, `usb`/`ota` = USB full image / Wi-Fi app image, `<PFX>` = manufacturer prefix ([list](../docs/protocol/manufacturer-prefixes.md)).

## Compatibility

All 4.x and 5.x remotes and robots work together (protocol major 4). Features the robot doesn't have show as `N/A` on the remote.

| TX | RX | Works | Notes |
|---|---|---|---|
| 5.36 | 5.36 | ✅ | **Recommended** |
| 5.x | 4.x | ✅ | New robot settings show `N/A`. Update the robot over Wi-Fi. |
| 4.x | 5.x | ✅ | New settings aren't in the old remote's menu. Update the remote. |
| ≤ 4.21 | ≥ 4.32 | ✅ | `RX AP-Mode` shows the old network name / address; use `VILKAS-…` and 4.3.2.1 |
| 4.x / 5.x | ≤ 3.x | ❌ | Remote shows `OLD RX FW!`. Update the robot over USB. |
| ≤ 3.x | 4.x / 5.x | ❌ | Robot triple-blinks. Update the remote. |
| — (phone) | ≥ 4.30 | ✅ | [Phone control](../docs/phone-control.md), no remote needed |

**RX 5.00 note:** the robot's battery voltage calibration starts again from 1.00 after updating from 4.x. Check `ROBOT > Battery > Volt Cal` once.

## Board support

| Board | Status | Official image |
|---|---|---|
| VILKAS-RX Rev B | In the field | `RX_RevB_…` |
| VILKAS-RX Rev C | Shipping | `RX_RevC_…` |
| VILKAS-RX Rev D | **No hardware yet. Firmware ready.** | `RX_RevD_…` |
| VILKAS-TX Rev B | In the field | `TX_Rev_B_…` |
| VILKAS-TX Rev C | Released | `TX_Rev_C_…` |

## Releases

| Release | Date | Highlights |
|---|---|---|
| RX 5.36 / TX 5.36 | 2026-11-06 | 100 Hz control, 20 kHz quiet drive, new menu and home screen, match timer, fast reset recovery, low-voltage limit, ESC calibration, Wi-Fi PIN, phone welcome page, TX Rev C |
| RX 4.32 / TX 4.22 | 2026-10-23 | Wi-Fi network `VILKAS-<name>`, address 4.3.2.1 |
| RX 4.31 | 2026-10-22 | Advanced settings page, "Always start in Wi-Fi phone mode" |
| TX 4.21 | 2026-10-22 | `RX AP-Mode`, ESC/DC/SERVO selection, W/Q limits |

Full history: [CHANGELOG.md](../CHANGELOG.md).
