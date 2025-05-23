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
| 🖌️ Customizable Appearance | Colors, font styles, scroll speed, and labels via web interface             |
| 🔁 Auto-Reconnect Wi-Fi   | Retries Wi-Fi and reboots after repeated failures                           |
| 🧑‍💻 OTA Support           | Upload new firmware or SPIFFS files over-the-air after first USB flash      |

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

First-time setup is done via USB. Edit the `config.h` file like this:

```cpp
#ifndef CONFIG_H
#define CONFIG_H

// WiFi Credentials
#define WIFI_SSID "your_wifi_name"
#define WIFI_PASSWORD "your_wifi_password"

// Weather API Key
#define WEATHER_API_KEY "your_api_key_here"
#define WEATHER_API_URL "https://api.openweathermap.org/data/2.5/weather"

// Coordinates for Geneva, Switzerland
#define LATITUDE 46.2044
#define LONGITUDE 6.1432

#endif // CONFIG_H
```

Then upload once via USB. After that, you can use **OTA**:

### OTA Upload (PlatformIO)

In your `platformio.ini` file:

```ini
; Uncomment below for OTA update
upload_protocol = espota
upload_port = hb9iiuhamclock.local
```

Then simply hit `Upload` again. No wires needed! 🔁

---

## 🌐 Web-Based Configurator

After uploading, open the web interface at:

```
http://hb9iiuhamclock.local
```

Use it to update:

- Digit & frame colors for both clocks
- Labels (e.g., "QTH Time")
- Weather banner color & scroll speed
- Fonts (italic toggle)
- Boot logo selection (logo1, logo2, logo3)
- Latitude & Longitude (to get local weather)
- Save all to flash memory via a single button

No need to recompile for visual changes!

---

## 🧪 Tips

- Reboot automatically if Wi-Fi fails too often
- OTA updates show progress and RAM info directly on the TFT
- Includes NTP sync and real-time updates

---

## 📷 Screenshots / Demos

📸 Coming soon…

---

## 📡 73! from HB9IIU

Built for real ham shacks, tested on the bench, ready for the world. Enjoy! 🇨🇭
