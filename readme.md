# ESP32 CYD HamClock

A standalone ham radio clock for the **Cheap Yellow Display (CYD)** ESP32 board, by HB9IIU. This repository contains the firmware, device web interface, setup instructions and GitHub Pages USB installer.

**[Open the web flasher](https://hb9iiu.github.io/ESP32-CYD-HamClock/)** · [Download merged firmware](firmware/CYDHamClock.bin) · [Report an issue](https://github.com/HB9IIU/ESP32-CYD-HamClock/issues)

## Features

- Local and UTC clocks with configurable colors, frames and fonts.
- Weather information from OpenWeather, using your own API key.
- Solar activity, HF band conditions and VHF information from HamQSL.
- Six touchscreen pages, automatic page changes and a screensaver.
- Browser configuration, selectable splash images and custom 320 × 240 PNG uploads.
- Wi-Fi setup from a phone or computer; settings stored on the device.

## Supported hardware

The supplied configuration targets the classic **ESP32 CYD with 4 MB flash, a 320 × 240 ILI9341 TFT and XPT2046 resistive touchscreen**, commonly sold as ESP32-2432S028R. Check the controller and pinout: boards sold under the CYD name are not all interchangeable. ST7789 displays, capacitive-touch variants and ESP32-S3 boards are not supported by this build.

No external display wiring is needed on a matching CYD. Use a USB data cable and a stable USB power supply.

| Function | GPIO |
| --- | --- |
| TFT MISO / MOSI / clock | 12 / 13 / 14 |
| TFT chip select / data-command | 15 / 2 |
| TFT backlight | 21 |
| Touch MISO / MOSI / clock | 39 / 32 / 25 |
| Touch chip select / interrupt | 33 / 36 |

The TFT uses HSPI; touch uses a separate VSPI bus. The configuration is in [platformio.ini](platformio.ini) and [src/main.cpp](src/main.cpp).

## Install through your browser

1. Connect the CYD to your computer with a USB data cable.
2. Open the [web flasher](https://hb9iiu.github.io/ESP32-CYD-HamClock/) in desktop Chrome or Edge.
3. Close serial monitors, click **Connect and install**, and choose the board's serial port.
4. Follow the installation dialog, wait for completion, and reset the board if it does not restart automatically.

**Record any settings you want to reuse before installing.** The merged image includes the filesystem and overwrites stored configuration and custom images. Set up Wi-Fi again afterward. The browser installer checks the ESP32 chip family, but cannot identify your display controller.

## First-time Wi-Fi setup

1. On the Wi-Fi configuration screen, scan the first QR code or manually join **HB9IIUSetup**. This setup network has no password.
2. Scan the second QR code or open **http://192.168.4.1**. Stay connected even if your phone reports that the network has no internet.
3. Select your **2.4 GHz Wi-Fi network**, enter its password and save. The current firmware expects a password-protected network.
4. The CYD restarts. Reconnect your phone or computer to your normal network.
5. Open **http://hamclock.local**. If mDNS does not work on your network, use the device IP address from your router or the serial monitor at **115200 baud**.

The setup page also supplies the browser's time information. Internet access is needed for time synchronization, weather and solar updates. Some device web-interface assets are loaded from online CDNs.

## Weather and display settings

Open the device's web interface and use **Modify Open Weather API key** to save your OpenWeather key. Weather data is retrieved through the OpenWeather current-weather API. An invalid or missing key prevents weather updates; solar data is fetched separately from HamQSL.

Set your latitude and longitude in the web interface so weather is requested for your location.

The web interface lets you change clock and weather colors, border thickness, italic digits, weather scrolling speed, the screensaver timeout and automatic page changes. Choose a built-in splash image or upload a **320 × 240 PNG**.

Tap the screen to cycle through the clock, propagation, three solar-information pages and Wi-Fi information. Touch also wakes the display from its screensaver.

To reopen setup after changing networks, restart the CYD; a failed connection starts the setup portal. To reset saved Wi-Fi and display settings, touch the screen during startup and choose **Proceed** in the factory-reset dialog, or **Exit** to cancel.

## Build and upload with PlatformIO

Install PlatformIO Core, or the PlatformIO extension in VS Code. Clone this repository and run from its root:

```sh
pio run -e esp32dev
```

Libraries are included in `lib/`; PlatformIO installs the pinned ESP32 platform and any declared transitive dependencies. The build script generates the application and SPIFFS filesystem, then writes the complete image to **`firmware/CYDHamClock.bin`**.

For a connected board, upload both components:

```sh
pio run -e esp32dev -t upload
pio run -e esp32dev -t uploadfs
pio device monitor -b 115200
```

Add `--upload-port PORT` if PlatformIO cannot select the correct port. Close the monitor before uploading again.

Alternatively, use esptool to write the **merged** image at **0x0**:

```sh
esptool.py --chip esp32 --port PORT write_flash 0x0 firmware/CYDHamClock.bin
```

Do not flash the merged image at the application's 0x10000 address. The `no_ota.csv` partition layout places the application at 0x10000 and SPIFFS at 0x210000; this build has no OTA application partition.

## Troubleshooting

| Symptom | Check |
| --- | --- |
| No USB serial port | Use a data cable, try another port, and install the driver matching your board's USB-to-serial chip if needed. |
| Cannot connect to flash | Close serial monitors. Hold BOOT, press and release RESET, then release BOOT when the connection starts. |
| Blank or wrong-colored screen | Confirm the ILI9341 controller and GPIO mapping above. A different CYD variant may need a different driver. |
| Web interface missing after manual upload | Upload SPIFFS with `pio run -e esp32dev -t uploadfs`, or install the complete merged image. |
| `hamclock.local` does not resolve | Connect to the same network and use the device's IP address directly. |
| Weather unavailable | Check the saved OpenWeather key and internet connectivity. |
| TFT_eSPI warns that `TOUCH_CS` is undefined | Touch is handled by the separate XPT2046 library and SPI bus; TFT_eSPI's touch interface is unused. |

## Publishing the flasher

The static installer uses [ESP Web Tools](https://esphome.github.io/esp-web-tools/) and is deployed by [the Pages workflow](.github/workflows/pages.yml). Its manifest flashes the merged image at offset 0.

1. In this GitHub repository, open **Settings → Pages → Build and deployment → Source**, and select **GitHub Actions**.
2. Push these files to `main`, or run **Actions → Publish CYD web flasher → Run workflow** after they are on GitHub.
3. Wait for the deployment to finish. The site will be at **https://hb9iiu.github.io/ESP32-CYD-HamClock/**.

For firmware updates, run `pio run -e esp32dev -t clean` followed by `pio run -e esp32dev` to regenerate the complete image (including filesystem-only changes), test on the target CYD, then commit `firmware/CYDHamClock.bin` together with its source changes. Pushing the binary to `main` redeploys it. The workflow deliberately publishes this checked-in firmware rather than automatically releasing an untested build. The web installer and download always use that same file, identified by its SHA-256 hash. A GitHub release can attach this same binary.

To package and preview locally:

```sh
python3 scripts/build_web.py
python3 -m http.server 8000 --directory build/web
```

Open `http://localhost:8000`. The packaging script checks the merged image's size and headers and generates the manifest and checksum. Only the packaged site and firmware are uploaded to Pages, not the repository or device configuration files.

See [GitHub's Pages workflow documentation](https://docs.github.com/en/pages/getting-started-with-github-pages/using-custom-workflows-with-github-pages) for hosting configuration.

## License and credits

Copyright HB9IIU. See [LICENSE](LICENSE) for the project's terms, including its commercial-device restriction. Bundled libraries retain their respective licenses and notices. Weather data is provided by OpenWeather; solar and propagation data by HamQSL.
