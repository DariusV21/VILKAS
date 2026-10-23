# Official firmware

Official VILKAS firmware images. How to flash them: [docs/flashing.md](../docs/flashing.md). License: free to use and redistribute unmodified (see [LICENSE.md](../LICENSE.md)).

## Current versions

| Unit | Version | Folder | Boards |
|---|---|---|---|
| VILKAS-RX from **[Lithuanian Bots](https://lithuanianbots.com/)** | **4.32** | [`RX_v4.32_LB/`](RX_v4.32_LB/) | Rev C ✅ · Rev D ✅ (no hardware yet) |
| VILKAS-TX | **4.22** | [`TX_v4.22/`](TX_v4.22/) | Rev B ✅ |

**Which prefix?** The robot's name on the remote, or in its Wi-Fi network name, starts with the prefix. `LB-…` boards are from Lithuanian Bots. For `DV-…` boards, ask DV Electronics for firmware.

```
firmware/
├── RX_v4.32_LB/
│   ├── RX_RevC_V4.32_usb_LB.bin   # USB, flash at 0x0
│   ├── RX_RevC_V4.32_ota_LB.bin   # Wi-Fi update (http://4.3.2.1/update)
│   ├── RX_RevD_V4.32_usb_LB.bin
│   ├── RX_RevD_V4.32_ota_LB.bin
│   └── SHA256SUMS.txt
├── TX_v4.22/
│   ├── TX_Rev_B_V4.22.bin         # USB, flash at 0x0
│   └── SHA256SUMS.txt
└── _archive/                      # previous versions
```

File name parts: `Rev<R>` = board revision, `V<x.xx>` = version, `usb`/`ota` = USB full image / Wi-Fi app image, `<PFX>` = manufacturer prefix ([list](../docs/protocol/manufacturer-prefixes.md)).

The same files are also attached to the matching GitHub **Release**.

## Compatibility

| TX firmware | RX firmware | Protocol | Works together | Notes |
|---|---|---|---|---|
| 4.22 | 4.32 | 4.1 | ✅ | **Recommended.** `RX AP-Mode` shows the right network name and address for each robot. |
| 4.21 | 4.30–4.32 | 4.1 | ✅ | All weapon features. `RX AP-Mode` always shows the old name and address (`BB-UPD-` / 192.168.4.1). |
| 4.20 | 4.30–4.32 | 4.1 | ✅ | SERVO is shown as "ESC", and there are no separate Q limits. Use the phone page or TX ≥ 4.21 for those. |
| 4.10+ | 4.10+ | 4.1 | ✅ | DC weapon (RX Rev D) needs TX ≥ 4.10 |
| 4.x | 4.x | 4.x | ✅ | Unknown settings shown as `N/A` |
| 4.x | ≤ 3.x | — | ❌ | Remote shows `OLD RX FW!`. Update the RX. |
| ≤ 3.x | 4.x | — | ❌ | Robot triple-blinks. Update the TX. |
| — (phone) | ≥ 4.30 | Wi-Fi | ✅ | [Phone control](../docs/phone-control.md), no remote needed |

## Board support

| Board | Status | Official image |
|---|---|---|
| VILKAS-RX Rev B | Few units in the field | Not in releases from 4.31 (the source still supports it) |
| VILKAS-RX Rev C | Shipping | `RX_RevC_…` |
| VILKAS-RX Rev D | **No hardware yet. Firmware ready.** | `RX_RevD_…` |
| VILKAS-TX Rev B | Shipping | `TX_Rev_B_…` |

## Releases

| Release | Date | Notes |
|---|---|---|
| RX 4.32 / TX 4.22 | 2026-10-23 | Wi-Fi network `VILKAS-<name>`, address 4.3.2.1 (fixes the sign-in page on newer Android phones) |
| RX 4.31 | 2026-10-22 | Advanced settings page, "Always start in Wi-Fi phone mode" |
| TX 4.21 | 2026-10-22 | `RX AP-Mode`, ESC/DC/SERVO selection, W/Q limits |

Full history: [CHANGELOG.md](../CHANGELOG.md).
