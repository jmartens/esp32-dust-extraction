# Flashing the ESP32 firmware

The GitHub Release provides the current `open_extraction` ESP-IDF application build for the ESP32 target as individual assets. Download all `.bin` images, `flash_args`, and `manifest.json` into one directory. The images include the application, bootloader, partition table, and any additional images required by the build. This is the firmware currently buildable in the repository; it does not imply that the intended future controller and satellite roles or production hardware are complete.

1. Connect the ESP32 board to your computer with a USB data cable.
2. Install Python 3 and esptool if needed:

   ```sh
   python -m pip install esptool
   ```

3. Identify the serial port for the board (`COM3` on Windows, or `/dev/ttyUSB0` on Linux as examples), then run this from the directory containing the downloaded assets:

   ```sh
   python -m esptool --chip esp32 -p PORT -b 460800 --before default-reset --after hard-reset write-flash '@flash_args'
   ```

   Replace `PORT` with the board's serial port. If flashing fails at the selected speed, retry with `-b 115200`. Some boards require holding the BOOT button while connecting or starting the flash.

The included `flash_args` supplies the flash mode, frequency, size, image addresses, and filenames generated from the ESP-IDF build output. The accompanying `manifest.json` records the source commit and SHA-256 checksums for the binary images.

## Flashing from the Docker development environment

The containerised build (see [docs/implementation/build-system.md](../docs/implementation/build-system.md)) can flash a locally built firmware.

**Linux** (pass the serial device through):

```sh
make flash IDF_DEVICE=/dev/ttyUSB0
make monitor IDF_DEVICE=/dev/ttyUSB0
```

**Windows and macOS** (Docker cannot pass USB serial ports through): run an RFC2217 bridge on the host and point the container at it.

```sh
python -m pip install esptool
esp_rfc2217_server -v -p 4000 COM3   # use /dev/cu.usbserial-XXXX on macOS
```

Then, in a second terminal:

```sh
# Windows PowerShell
./scripts/idf-docker.ps1 flash -Port 'rfc2217://host.docker.internal:4000?ign_set_control'
# macOS
make flash IDF_PORT='rfc2217://host.docker.internal:4000?ign_set_control'
```

`monitor` takes the same `-Port` / `IDF_PORT` argument.
