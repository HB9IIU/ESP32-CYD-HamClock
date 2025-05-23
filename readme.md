# 🌦️ ESP32 Weather & UTC Clock Display

**Author:** HB9IIU  
**Built for:** Marco T77PM  
**Target Device:** ESP32 CYD (Cheap Yellow Display)

> ⚠️ **Perfect for radio amateurs** who rely on both **Local and UTC time** — for DXing, satellite tracking, or digital mode operation like FT8, JS8Call, or WSPR.

---

## 📟 Overview

This project turns your **ESP32 CYD board** into a full-screen **TFT time and weather station**, designed to sit proudly in your ham shack.

It displays:

- ⏰ Local & UTC time in large, easy-to-read fonts
- 🌤️ Real-time weather data (temperature, humidity, sunrise/sunset)
- 🖼️ A splash screen logo (loaded from SPIFFS)
- 🔄 Auto-syncs time using NTP
- 🧭 Your QTH location via coordinates for precise weather info

Clean layout, large fonts, and dual clocks make it ideal for quick glances while operating.

---

## ✨ Features

| Feature                  | Description                                                                 |
|--------------------------|-----------------------------------------------------------------------------|
| 🕒 Dual Clocks            | Local time (top) and UTC (bottom) in 7-segment style fonts                  |
| 🌐 NTP Time Sync         | Always accurate via `pool.ntp.org`                                          |
| ☀️ Weather Data           | From OpenWeather API, updated every 5 minutes                              |
| 📄 PNG Splash Screen      | Loads `logo1.png`, `logo2.png`, or `logo3.png` from SPIFFS on boot          |
| 🖌️ Customizable Appearance | Colors, font styles, scroll speed, and labels via onboard web interface     |
| 🔁 Auto-Reconnect Wi-Fi   | Retries Wi-Fi and reboots after repeated failures                           |
| 🌐 Web Configurator       | Change all settings live: colors, text, banner speed, and more              |
| 📲 OTA Updates            | Upload firmware wirelessly after first flash via USB                        |

---

## 🛠️ Requirements

| Component             | Description                                      |
|----------------------|--------------------------------------------------|
| ESP32 CYD Board      | "Cheap Yellow Display" with ILI9341 320x240 TFT  |
| Wi-Fi                | For NTP and OpenWeather API                      |
| OpenWeather API Key  | [Get yours here](https://openweathermap.org/api) |
| PlatformIO           | (Preferred) or Arduino IDE with SPIFFS uploader  |

> ⚠️ **Display color issue?** If you see **washed-out or pale colors**, edit your `platformio.ini`:  
> Replace: `-D TFT_INVERSION_ON`  
> With: `-D TFT_INVERSION_OFF`

---

## ⚙️ Configuration

All static settings are in `config.h`. Here's a sample:

```cpp
// WiFi Credentials
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"

// OpenWeatherMap API Key
#define WEATHER_API_KEY "your_openweather_key"
#define WEATHER_API_URL "https://api.openweathermap.org/data/2.5/weather"

// Location (Geneva, Switzerland)
#define LATITUDE 46.2044
#define LONGITUDE 6.1432
```

---

## 🔄 Initial Flash & Filesystem Upload

To get started:

1. **Flash the firmware via USB cable**:
   ```bash
   pio run --target upload
   ```

2. **Upload the filesystem (SPIFFS)** — required to display the web interface and boot images:
   ```bash
   pio run --target uploadfs
   ```

> 📁 The SPIFFS contains:
> - HTML & JavaScript files for the configurator
> - Boot logo PNG images (`logo1.png`, `logo2.png`, `logo3.png`)
> - Settings stored in `settings.json`

3. Once flashed, the device hosts a **web interface** for configuration at:
   ```
   http://hb9iiuhamclock.local
   ```

4. ✅ After the first successful flash, you can **enable OTA updates** by editing `platformio.ini`:
   ```ini
   ; Uncomment for OTA updates
   upload_protocol = espota
   upload_port = hb9iiuhamclock.local
   ```

---

## 🧑‍💻 Customization via Web Interface

Visit your ESP32 on the network to customize:
- Clock colors (digits, frame)
- Labels (e.g., “QTH Time”)
- Scroll speed
- Boot image (PNG from SPIFFS)
- Location coordinates (for weather)
- Italic vs normal fonts
- Save everything to flash memory

---

## 🙌 Final Notes

This project is designed for **ease of use and ham shack aesthetics**.  
Big digits, readable fonts, OTA updates, and a slick web interface — everything you need in one elegant display.

Enjoy & 73 de **HB9IIU** 🇨🇭