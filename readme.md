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
- 🧠 **New: Web-based configuration UI!**

Clean layout, large fonts, and dual clocks make it ideal for quick glances while operating.

---

## ✨ Features

| Feature                   | Description                                                                 |
|---------------------------|-----------------------------------------------------------------------------|
| 🕒 Dual Clocks             | Local time (top) and UTC (bottom) in 7-segment style fonts                  |
| 🌐 NTP Time Sync           | Always accurate via `pool.ntp.org`                                          |
| ☀️ Weather Data            | From OpenWeather API, updated every 5 minutes                              |
| 📄 PNG Splash Screen       | Loads `logo1.png`, `logo2.png`, or `logo3.png` from SPIFFS on boot          |
| 🧑‍💻 Web Configurator (NEW!) | Change colors, labels, font style, banner speed, location — via webpage!     |
| 🔁 Auto-Reconnect Wi-Fi    | Retries Wi-Fi and reboots after repeated failures                           |
| 📦 OTA Updates (optional)  | Upload firmware **wirelessly** after first USB flash                        |

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

## 🚀 Flashing Instructions

### 1️⃣ First Upload via USB Cable

- Connect the ESP32 CYD via USB
- Open `platformio.ini` and use this default:

```ini
upload_protocol = esptool
```

- Upload both firmware and SPIFFS:

```bash
pio run -t upload
pio run -t uploadfs
```

---

### 2️⃣ Enable OTA for Future Uploads

Once the device is online and `hb9iiuhamclock.local` is reachable:

- Edit `platformio.ini`:

```ini
# Uncomment below for OTA update
upload_protocol = espota
upload_port = hb9iiuhamclock.local
```

- Upload code wirelessly:

```bash
pio run -t upload
```

> ℹ️ SPIFFS uploads over OTA **are not supported** — for web UI changes, re-upload via USB.

---

## ⚙️ Configuration

All defaults live in `config.h`, but **everything can now be changed via the built-in web interface**.

### 🧾 Example `config.h` Template

```cpp
#define WIFI_SSID      "YourNetwork"
#define WIFI_PASSWORD  "YourPassword"

#define WEATHER_API_KEY "your_api_key_here"

#define LATITUDE        46.4667118
#define LONGITUDE       6.8590456
#define TIME_OFFSET     2 // e.g. 2 = CEST

#define LOCAL_TIME_COLOUR TFT_GREEN
#define UTC_TIME_COLOUR   TFT_GOLD

#define DOUBLE_FRAME      false
#define LOCAL_FRAME       TFT_DARKGREY
#define UTC_FRAME         TFT_DARKGREY
#define BANNER_COLOUR     TFT_DARKGREEN
#define BANNER_SPEED      20 // Lower = faster scroll

#define LOCAL_TIME_FRAME_LABEL "  QTH Time  "
#define UTC_TIME_FRAME_LABEL   "  UTC Time  "

#define START_UP_LOGO "logo1.png" // logo1.png / logo2.png / logo3.png
#define ITALIC_CLOCK_FONTS false  // Use italic 7-segment fonts or not
```

---

## 🧪 Tested With

- ✅ ESP32 DevKitC and ESP32 CYD boards
- ✅ PlatformIO Core 6.1+
- ✅ OpenWeatherMap free API tier
- ✅ Chrome/Firefox for web configuration

---

## 73! de HB9IIU

Enjoy this shack companion — and if you're operating digital, **may the prop be with you!**
