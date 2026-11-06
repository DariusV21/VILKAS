# Flashing firmware

VILKAS boards are never locked. You can flash official firmware, your own firmware or someone else's firmware, then go back again.

**Settings and pairing are kept** across normal updates: the RX keeps its bound remote, channel and robot settings; the remote keeps its calibration and robot list.

## Which file?

Official images are in [`firmware/`](../firmware/) and on the [Releases page](https://github.com/DariusV21/VILKAS/releases):

| File | Use for | How |
|---|---|---|
| `RX_Rev<R>_V<x.xx>_ota_<PFX>.bin` | Robot, app only | **Wi-Fi update** in the browser (recommended) |
| `RX_Rev<R>_V<x.xx>_usb_<PFX>.bin` | Robot, full image | **USB**, flash at `0x0` |
| `TX_Rev_<R>_V<x.xx>.bin` | Remote, full image | **USB**, flash at `0x0` |
| `SHA256SUMS.txt` | Checksums | `sha256sum -c SHA256SUMS.txt` |

- `<R>` is the **board revision**: RX `B`, `C`, `D`; TX `B`, `C`. It must match your board. The remote shows the robot's revision in `SYSTEM > Info` (`HW:C`). The Wi-Fi updater refuses a wrong revision.
- `<PFX>` is the **manufacturer prefix**: `LB` for boards from [Lithuanian Bots](https://lithuanianbots.com/). It's the start of the robot's name (`LB-…`). Pick the one your board was sold with. A different prefix renames the robot, so the Wi-Fi updater asks for confirmation.
- The remote has no hardware ID, so make sure you pick the TX file for your remote's revision.

## 1. Wi-Fi update (robot, no cable)

1. Put the robot in **Wi-Fi mode**:
   - **From the remote:** `ROBOT > AP-Mode`, hold the joystick button 2 s (robot standing still). The remote shows the network name, address and PIN.
   - **On the robot:** hold BIND for **≥ 5 s** while powering up, and release it when the LED lights. This doesn't clear the pairing.
2. On a phone or PC, join the network **`VILKAS-<robot>`**. It's open, unless `ROBOT > AP Pass` is `PIN` (then use the PIN from the remote). A BIND-held start is always open.
3. Open **http://4.3.2.1/update**, or on the phone's drive page tap ⚙ → hold **Advanced settings** for 2 s.
   - RX 4.30–4.31: network `BB-UPD-<robot>`, page http://192.168.4.1/update
   - RX 4.00–4.2x: network `BB-UPD-<robot>`, page http://192.168.4.1
4. Select the **`_ota_`** file for your board revision and upload it. The LED flickers while writing. Don't power off.
5. The robot restarts with the new firmware. No rebinding is needed.

**What the updater checks:**

- It's a valid ESP32 app image (not a `_usb_` image or bootloader).
- It contains the marker `BBRX-IMG/HW:<rev>/END` for **the same board revision**.
- From RX 5.35 the **maker tag** `BBRX-PFX:<prefix>/END` must match too, unless *Allow maker tag change* is ticked. Images before 5.35 have no tag and are accepted.
- The checksum is verified before the image is activated. A failed or interrupted upload leaves the old firmware running.
- The new firmware is kept only after it has started fully. If it crashes during start-up, the robot rolls back to the previous firmware.

Holding BIND for 5 s at power-up always gets you back into Wi-Fi mode, because that check runs before anything else.

**Custom RX firmware** that should be installable through the official Wi-Fi updater must include the markers, e.g.

```cpp
extern "C" const char RX_IMAGE_MARKER[] __attribute__((used)) = "BBRX-IMG/HW:C/END";
extern "C" const char RX_TAG_MARKER[]   __attribute__((used)) = "BBRX-PFX:XX/END";
```

Once custom firmware is running, the official updater is gone. To go back to official firmware, use USB.

## 2. USB (remote and robot)

Needed for the remote, for the very first flash, or when Wi-Fi isn't an option.

- **Remote:** connect the USB-C port on its ESP32-C3 module.
- **Robot:** use the VILKAS programming board, or wire USB D−/D+ (GPIO18/19), 5 V and GND to the board.

> **ESC calibration is skipped while the robot is connected by USB**, so a robot on your desk can't spin up its weapon.

### Option A: browser (no install)

1. Open the [Espressif web flasher](https://espressif.github.io/esptool-js/) in Chrome or Edge.
2. Click **Connect** and select the ESP32-C3 port. If none appears, hold BOOT while plugging in.
3. Set the flash address to **`0x0`**, choose the `_usb_` file (robot) or the `TX_Rev_…` file (remote) and click **Program**.
4. Power-cycle the device.

### Option B: esptool

```bash
pip install esptool
esptool.py --chip esp32c3 --port COM5 --baud 460800 write_flash 0x0 TX_Rev_C_V5.36.bin
```

Replace `COM5` with your port (`/dev/ttyACM0` on Linux, `/dev/cu.usbmodem…` on macOS).

If a device behaves strangely after an upgrade, run `esptool.py --chip esp32c3 erase_flash` first, then write the full image again, then re-pair and re-calibrate. Erasing an RX also clears its bound remote and all robot settings.

### Recovery

Custom firmware can disable or crash the USB port. The ESP32-C3 **ROM bootloader can't be overwritten**, so you can always recover: hold **GPIO9 (BOOT) LOW** while powering up or resetting, and the port appears in download mode. Then flash a full image.

## 3. Going back to official firmware

Flash the latest official full image over USB (section 2).
