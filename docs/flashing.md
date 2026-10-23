# Flashing firmware

VILKAS boards are never locked. You can flash official firmware, your own firmware or someone else's firmware, then go back again.

## Which file?

Official images are in [`firmware/`](../firmware/), one folder per version and prefix (`RX_v4.32_LB/`, `TX_v4.22/`):

| File | Use for | How |
|---|---|---|
| `RX_Rev<R>_V<x.xx>_usb_<PFX>.bin` | RX full image (bootloader + partitions + app) | **USB**, flash at `0x0` |
| `RX_Rev<R>_V<x.xx>_ota_<PFX>.bin` | RX app only | **Wi-Fi update** in the browser |
| `TX_Rev_<R>_V<x.xx>.bin` | TX full image | **USB**, flash at `0x0` |

- `<R>` is the **board revision** (`C`, `D`). It must match your board, and the Wi-Fi updater refuses a wrong one.
- `<PFX>` is the **manufacturer prefix** that the robot shows on the remote: `LB` for boards bought from [Lithuanian Bots](https://lithuanianbots.com/), `DV` for boards from DV Electronics (firmware on request). Pick the one your board was sold with. It only changes the robot's name, not how it drives.
- **RX Rev D:** the firmware is ready, but there is no hardware yet.

Check the download against `SHA256SUMS.txt`:

```bash
sha256sum -c SHA256SUMS.txt
```

## 1. USB (TX and RX)

**TX:** connect the USB-C port on the remote's ESP32-C3 module.
**RX:** connect through the VILKAS programming board, or wire USB D−/D+ (GPIO18/19), 5 V and GND to the board.

### Option A: browser (no install)

1. Open the [Espressif web flasher](https://espressif.github.io/esptool-js/) in Chrome or Edge.
2. Click **Connect** and select the ESP32-C3 serial/JTAG port.
3. Set the flash address to `0x0`, select the `_usb_` file (RX) or the `TX_Rev_…` file and click **Program**.
4. Power-cycle the device.

### Option B: esptool

```bash
pip install esptool
esptool.py --chip esp32c3 --port COM5 write_flash 0x0 RX_RevC_V4.32_usb_DV.bin
```

### If the port doesn't appear

Custom firmware can disable or crash the USB port. The ESP32-C3 **ROM bootloader can't be overwritten**, so you can always recover:

1. Hold **GPIO9 (BOOT) LOW** while you power up or reset the board.
2. The chip enters ROM download mode and the USB port appears.
3. Flash a full image as above.

## 2. Wi-Fi update (RX only, no cable)

1. Put the robot in **Wi-Fi mode** in one of these ways:
   - Hold BIND for **≥ 5 s** while powering up, and release it when the LED lights.
   - On a paired remote, choose `SYSTEM > RX AP-Mode` (`RX Update` on TX 4.20).
   - If *Always start in Wi-Fi phone mode* is on, the robot is already in Wi-Fi mode.
2. Connect a phone or PC to the open network `VILKAS-<robot name>`.
3. The phone drive page opens. Tap ⚙ → **Advanced settings** (hold 2 s), or go to **http://4.3.2.1/update**.
   - RX 4.30–4.31: network `BB-UPD-<robot name>`, page http://192.168.4.1/update
   - RX 4.00–4.2x: network `BB-UPD-<robot name>`, page http://192.168.4.1
4. Select the `_ota_` file for your board revision and upload it. Don't power off while it uploads.
5. The RX checks the image, writes it to the spare OTA slot and reboots. If the new firmware doesn't start correctly, the bootloader rolls back to the previous one.

The drive motors and weapons stop while firmware is being written. If nothing happens for 10 minutes, the robot leaves Wi-Fi mode, unless *Always start in Wi-Fi phone mode* is on.

The updater accepts only images that:

- are a valid ESP32 app image (not a full `_usb_` image),
- contain the marker string `BBRX-IMG/HW:<rev>/END`, where `<rev>` matches the board revision,
- pass ESP-IDF checksum verification.

**Custom RX firmware** that should be installable through the official Wi-Fi updater must include that marker string, e.g.

```cpp
extern "C" const char RX_IMAGE_MARKER[] __attribute__((used)) = "BBRX-IMG/HW:C/END";
```

Once custom firmware is running, the official updater is no longer present. To go back to official firmware, use USB.

## 3. Going back to official firmware

Flash the latest official full image over USB (section 1). Robot settings (motor direction, limits, rates, phone layout) and pairing are stored in NVS. They survive a Wi-Fi app update, but a full USB flash may reset them. If so, set the robot up again from the remote menu or the phone.
