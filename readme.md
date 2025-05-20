# 🌦️ ESP32 Weather & UTC Clock Display

**Author:** HB9IIU  
**Built for:** Marco T77PM

> ⚠️ Designed especially for **radio amateurs** who rely on both **Local and UTC time** — whether for DX contacts, satellite passes, or digital mode logging.

---

## 📟 What It Is

This project turns an **ESP32 CYD (Cheap Yellow Display)** into a clean, full-screen **weather + time TFT display** — ideal for your shack or portable setup.

### ✨ Key Features

- ⏰ **Dual Clock Display**:
  - Top: **Local time** (QTH)
  - Bottom: **UTC time**
- 🌤️ **Live weather info** via OpenWeather API (updated every 5 min)
- 🖼️ **Startup PNG logo** (user-selectable)
- 📡 **NTP time sync** over Wi-Fi for accurate clocking
- 🔋 Low-resource design, crisp fonts, and graphical layout

---

## 📸 Example Output

> _(Add a screenshot or photo here if you can)_

---

## 🔧 Requirements

| Component             | Description                                      |
|----------------------|--------------------------------------------------|
| ESP32 CYD Board      | "Cheap Yellow Display" with ILI9341 TFT          |
| Internet             | Wi-Fi connection for NTP & weather API           |
| OpenWeather API Key  | Free at [openweathermap.org](https://openweathermap.org/api) |
| PlatformIO (preferred) | Or Arduino IDE with filesystem uploader plugin |

---

## ⚙️ Configuration

All settings are in `config.h`.  
A working example is included — but you should replace Wi-Fi and API key for your setup.

### 📝 Example `config.h`

```cpp
#define WIFI_SSID      "NO WIFI FOR YOU!!!"
#define WIFI_PASSWORD  "balblabla"

#define WEATHER_API_KEY "your_api_key_here"
#define LATITUDE        46.4667118
#define LONGITUDE       6.8590456
#define TIME_OFFSET     2 // Timezone offset from UTC (e.g. 2 = CEST)

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
#define ITALIC_CLOCK_FONTS false  // Set true for italic font

