
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
- 🖥️ Configuration via built-in web interface

---

## ✨ Features

| Feature                  | Description                                                                 |
|--------------------------|-----------------------------------------------------------------------------|
| 🕒 Dual Clocks            | Local time (top) and UTC (bottom) in 7-segment style fonts                  |
| 🌐 NTP Time Sync         | Always accurate via `pool.ntp.org`                                          |
| ☀️ Weather Data           | From OpenWeather API, updated every 5 minutes                              |
| 📄 PNG Splash Screen      | Loads `logo1.png`, `logo2.png`, or `logo3.png` from SPIFFS on boot          |
| 🖌️ Configurable via Web UI | Colors, fonts, labels, coordinates, scroll speed — all from browser         |
| 🔁 Auto-Reconnect Wi-Fi   | Retries Wi-Fi and reboots after repeated failures                           |
| 🔧 OTA Updates            | Update firmware and filesystem over the air (wireless)                      |

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

Edit `config.h` before the first upload:

```
#define WIFI_SSID        "YourWiFi"
#define WIFI_PASSWORD    "YourPassword"

#define WEATHER_API_KEY  "your_api_key_here"
#define WEATHER_API_URL  "https://api.openweathermap.org/data/2.5/weather"

#define LATITUDE         46.2044   // Geneva
#define LONGITUDE        6.1432    // Geneva
```

---

## 🚀 Flashing for the First Time

1. Connect your ESP32 CYD to your PC via USB.
2. Upload the **firmware** via PlatformIO:
   ```ini
   upload_protocol = esptool
   ```
3. Also upload the **SPIFFS filesystem**:
   ```bash
   pio run --target uploadfs
   ```

---

## 🌐 Accessing the Web Interface

Once connected to Wi-Fi, the ESP32 will:

- Sync time via NTP
- Fetch weather using your coordinates and API key
- Start a **web configuration portal**

You can access it via:

- **http://hb9iiuhamclock.local** (if mDNS is supported by your OS)
- Or use the **IP shown in Serial Monitor** (e.g., `http://192.168.1.123`)

From the web interface you can:

- Adjust colors and fonts in real time
- Change labels (e.g., “QTH Time”)
- Set scroll speed and position
- Switch startup logo
- Save all settings to flash memory

---

## 🔄 OTA Updates (Over the Air)

Once the ESP32 is on your network, you can upload future updates wirelessly.

### 1. Edit `platformio.ini`:

Uncomment or add:

```ini
upload_protocol = espota
upload_port = hb9iiuhamclock.local
```

> You can also replace `hb9iiuhamclock.local` with the IP address of the device.

### 2. Upload Firmware

```bash
pio run --target upload
```

### 3. Upload Filesystem

```bash
pio run --target uploadfs
```

> ⚠️ OTA works best when no Serial Monitor is connected.

---

## 🤝 Credits

Thanks to Marco T77PM for the original idea and testing.  
Developed by HB9IIU with love from Switzerland 🇨🇭

---

## 📸 Screenshots

![example1](docs/screenshot1.png)  
![example2](docs/screenshot2.png)

---

## 📬 Feedback

Got improvements or bug reports? Open a GitHub issue or reach out via ham radio!

**73 de HB9IIU**
