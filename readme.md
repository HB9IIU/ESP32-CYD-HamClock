# ESP32 HamClock

This project is a **TFT-based ham radio clock and propagation display** for the ESP32.  
It shows **local and UTC time**, **HF/VHF band conditions**, **solar/geomagnetic data**, and **weather info** on a touch-enabled screen.

---

## ✨ Features
- 📺 **TFT Display (ILI9341/ILI9488 via TFT_eSPI)**
- ⏰ **Dual clocks**: Local time and UTC time
- 🌤️ **Weather data** from OpenWeather API (requires API key)
- ☀️ **Solar & HF propagation data** from [hamqsl.com](https://www.hamqsl.com/)
- 📡 **Band condition indicators** (Good / Fair / Poor)
- 📶 **Wi-Fi setup via captive portal** (AP mode `HB9IIUSetup`)
- 🌐 **Web interface** for configuration:
  - Time labels
  - Colors
  - Banner speed
  - Boot logo selection
- 🖼️ **Custom splash screen upload** via web
- 💤 **Screensaver mode** with random pixel animation
- 🔧 **Settings saved in SPIFFS** (JSON file)
- 🔗 **OTA updates + mDNS (`http://hamclock.local`)**

---

## 📦 Requirements
- ESP32 (ESP32-S3 recommended with TFT touch support)
- TFT display compatible with [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)
- OpenWeather API key (for weather info)
- PlatformIO or Arduino IDE

---

## 🚀 Getting Started
1. Flash the firmware to your ESP32.
2. On first boot, connect to the Wi-Fi AP **`HB9IIUSetup`** and open `192.168.4.1` to enter your Wi-Fi credentials.
3. Access the device at [http://hamclock.local](http://hamclock.local) once connected.
4. Enter your OpenWeather API key via the web UI.
5. Enjoy real-time ham radio propagation and clock information!

---

## 🖼️ Screenshots / Demo

---

## 📝 To-Do
- 📱 Adapt Web UI for better viewing on **mobile phones**

---

## 📜 License
This project is provided under the MIT License. See [LICENSE](LICENSE) for details.
# ESP32 CYD HamClock

This project is a **TFT-based ham radio clock and propagation display** for the ESP32,  
**adapted for the Cheap Yellow Display (CYD)**.  
It shows **local and UTC time**, **HF/VHF band conditions**, **solar/geomagnetic data**, and **weather info** on the CYD’s built-in TFT + touch screen.

---

## ✨ Features
- 📺 **Cheap Yellow Display (CYD)** supported (2.8" ILI9341 TFT + touchscreen + ESP32)
- ⏰ **Dual clocks**: Local time and UTC time
- 🌤️ **Weather data** from OpenWeather API (requires API key)
- ☀️ **Solar & HF propagation data** from [hamqsl.com](https://www.hamqsl.com/)
- 📡 **Band condition indicators** (Good / Fair / Poor)
- 📶 **Wi-Fi setup via captive portal** (AP mode `HB9IIUSetup`)
- 🌐 **Web interface** for configuration:
  - Time labels
  - Colors
  - Banner speed
  - Boot logo selection
- 🖼️ **Custom splash screen upload** via web
- 💤 **Screensaver mode** with random pixel animation
- 🔧 **Settings saved in SPIFFS** (JSON file)
- 🔗 **OTA updates + mDNS (`http://hamclock.local`)**

---

## 📦 Requirements
- Cheap Yellow Display (ESP32 + 2.8" TFT ILI9341 + touch controller)
- OpenWeather API key (for weather info)
- PlatformIO (recommended) or Arduino IDE

---

## 🚀 Getting Started
1. Flash the firmware to your CYD.
2. On first boot, connect to the Wi-Fi AP **`HB9IIUSetup`** and open `192.168.4.1` to enter your Wi-Fi credentials.
3. Access the device at [http://hamclock.local](http://hamclock.local) once connected.
4. Enter your OpenWeather API key via the web UI.
5. Enjoy real-time ham radio propagation and clock information!

---

## 🖼️ Screenshots / Demo
<p align="center">
  <img src="https://github.com/HB9IIU/ESP32-CYD-HamClock/blob/main/doc/Photos/IMG_8504.png?raw=true" width="600" alt="CYD HamClock Screenshot">
</p>

---

## 🌐 Web Installer
No need to compile anything! You can flash the latest version of **CYD HamClock** directly from your browser using the  
👉 [**ESP32 HamClock Web Installer**](https://esp32.hb9iiu.com/)  

Works with Chrome/Edge and any browser that supports WebSerial.

---

## 📝 To-Do
- 📱 Adapt Web UI for better viewing on **mobile phones**
- 🎨 Add optimized layouts for the CYD’s smaller 2.8" screen

---

## 📜 License
This project is provided under the MIT License. See [LICENSE](LICENSE) for details.
