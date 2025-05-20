/*
 * Program Name: ESP32 Weather and Time Display
 * Author: HB9IIU
 * Created for: Marco T77PM (for testing and customizing)
 * Description:
 * This program displays the local and UTC time on a TFT screen and fetches weather data from the OpenWeather API. 
 * The program includes a PNG logo display and uses custom fonts for the time display. 
 * It connects to Wi-Fi and the NTP server for accurate time synchronization.
 * 
 * Color List (TFT_eSPI):
 * --------------------------------
 * TFT_BLACK      - Black
 * TFT_WHITE      - White
 * TFT_BLUE       - Blue
 * TFT_RED        - Red
 * TFT_GREEN      - Green
 * TFT_CYAN       - Cyan
 * TFT_MAGENTA    - Magenta
 * TFT_YELLOW     - Yellow
 * TFT_ORANGE     - Orange
 * TFT_LIGHTGREY  - Light Grey
 * TFT_DARKGREY   - Dark Grey
 * TFT_BROWN      - Brown
 * TFT_PINK       - Pink
 * TFT_PURPLE     - Purple
 * TFT_GREY       - Grey
 * TFT_DARKGREEN  - Dark Green
 * TFT_LIGHTBLUE  - Light Blue
 * TFT_LIGHTGREEN - Light Green
 * TFT_LIGHTCYAN  - Light Cyan
 * TFT_LIGHTMAGENTA - Light Magenta
 *
 * Notes:
 * This code is based on the TFT_eSPI library and uses the NTPClient and HTTPClient libraries for fetching time 
 * and weather data from external sources. 
 * The program supports custom fonts and displays the information clearly on a TFT screen.
 */



#include <TFT_eSPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <TimeLib.h>
#include <HB9IIU7seg42pt.h> // https://rop.nl/truetype2gfx/ https://fontforge.org/en-US/
#include <HB9IIUOrbitronMed8pt.h>
#include <PNGdec.h>
#include <SPIFFS.h>

// Configuration Constants
const char *SSID = "NO WIFI FOR YOU!!!";
const char *WiFiPassword = "Nestle2010Nestle";
const int retriesBeforeReboot = 5; // Retry count before rebooting on failure
const String weatherAPI = "https://api.openweathermap.org/data/2.5/weather"; // OpenWeather API endpoint
const String apiKey = "4444fcd8624a929dedc1d56235723ff3"; // Your OpenWeather API key

// Location for Weather Data
const float latitude = 43.9424;  // Replace with your latitude
const float longitude = 12.4578; // Replace with your longitude

// Time Offset (in hours)
const int tOffset = 2; // Replace with your desired time offset (e.g., +2 for Central European Time)

// Global variables for previous time tracking
String previousLocalTime = "";
String previousUTCtime = "";

// TFT Display Setup
TFT_eSPI tft = TFT_eSPI(); // Create TFT display object

// NTP Client Setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); // UTC offset and update interval

// WiFi Reconnect Logic
int retryCount = 0;

// Function Prototypes
void connectWiFi();
void fetchWeatherData();
String formatLocalTime(long epochTime);
String convertEpochToTimeString(long epochTime);
void displayTime(int x, int y, String time, String& previousTime, int yOffset);

// PNG Decoder Setup
PNG png;
fs::File pngFile; // Global File handle (required for PNGdec callbacks)

// Callback functions for PNGdec
void *fileOpen(const char *filename, int32_t *size);
void fileClose(void *handle);
int32_t fileRead(PNGFILE *handle, uint8_t *buffer, int32_t length);
int32_t fileSeek(PNGFILE *handle, int32_t position);
void displayPNGfromSPIFFS(const char *filename, int duration_ms);

void setup() {
  // Start Serial Monitor
  Serial.begin(115200);
  Serial.println("Starting setup...");
  delay(5000);

  // Initialize TFT display
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  Serial.println("TFT Display initialized!");

  // Display PNG from SPIFFS
  displayPNGfromSPIFFS("logo.png", 3000);

  // Connect to Wi-Fi
  connectWiFi();

  // Initialize NTP Client
  timeClient.begin();
  timeClient.setTimeOffset(0); // UTC Offset (0 for UTC)
  Serial.println("NTP Client initialized.");
  tft.fillScreen(TFT_BLACK);

  fetchWeatherData();
  tft.setFreeFont(&Orbitron_Medium8pt7b);
  tft.drawRoundRect(0, 0, 320, 87, 5, TFT_GREEN);
  tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
  tft.drawCentreString(" Orario QTH  ", 160, 76, 1);
  tft.drawRoundRect(0, 105, 320, 87, 5, TFT_GREEN);
  tft.drawCentreString(" Orario UTC  ", 160, 76 + 105, 1);
}

void loop() {
  // Calculate time elapsed since last weather data fetch
  unsigned long currentMillis = millis();
  static unsigned long previousMillis = 0; // Store the last time the weather data was fetched

  // Update time every second
  timeClient.update();

  // Get Local Time by adding tOffset to UTC time
  long localEpoch = timeClient.getEpochTime() + (tOffset * 3600); // Add offset (in seconds)
  String localTime = formatLocalTime(localEpoch);                 // Format the local time

  // Get UTC Time
  String utcTime = timeClient.getFormattedTime();

  // Display Local and UTC Time with different y positions
  tft.setTextColor(TFT_WHITE); // Set text color to white
  tft.setFreeFont(&digital_7_monoitalic42pt7b);

  // Corrected y positions for both clocks
  displayTime(8, 5, localTime, previousLocalTime, 0);    // Display local time at y = 5
  displayTime(10, 107, utcTime, previousUTCtime, 0);   // Display UTC time at y = 106

  // Fetch Weather Data once every hour
  if (currentMillis - previousMillis >= 3600000) { // 3600000 milliseconds = 1 hour
    previousMillis = currentMillis;  // Save the current time
    fetchWeatherData();
  }

  delay(1000); // Update display every second
}


// Function to connect to Wi-Fi
void connectWiFi() {
  Serial.print("Connecting to Wi-Fi: ");
  Serial.print(SSID);
  WiFi.begin(SSID, WiFiPassword);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    retryCount++;
    if (retryCount >= retriesBeforeReboot) {
      Serial.println("Wi-Fi connection failed too many times, rebooting...");
      ESP.restart();
    }
  }

  Serial.println("Wi-Fi connected!");
}

// Fetch weather data
void fetchWeatherData() {
  HTTPClient http;
  String weatherURL = weatherAPI + "?lat=" + String(latitude) + "&lon=" + String(longitude) + "&appid=" + apiKey + "&units=metric";

  // Make GET Request
  http.begin(weatherURL);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    Serial.println("Weather data received.");

    // Parse the JSON response
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);

    // Extract Temperature, Feels Like, Name, Sunrise, Sunset, and Timezone
    float temp = doc["main"]["temp"];
    String cityName = doc["name"];

    // Display Weather Information on TFT
    tft.fillRect(0, 200, 240, 40, TFT_BLACK); // Clear previous weather data
    tft.setCursor(10, 120);
    tft.setFreeFont(&Orbitron_Medium8pt7b);
    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    tft.drawString(cityName, 10, 210, 1);
    char tempString[20];
    sprintf(tempString, "Temp: %.1f °C", temp); 
    tft.drawRightString(tempString, 310, 210, 1);
  } else {
    Serial.print("Error fetching weather data, HTTP code: ");
    Serial.println(httpCode);
  }

  http.end();
}

// Function to format the local time from epoch time
String formatLocalTime(long epochTime) {
  struct tm *timeInfo;
  timeInfo = localtime(&epochTime);  // Convert epoch to local time
  char buffer[9];
  strftime(buffer, sizeof(buffer), "%H:%M:%S", timeInfo);  // Format time as HH:MM:SS
  return String(buffer);
}

// Function to convert an epoch time to a human-readable time string
String convertEpochToTimeString(long epochTime) {
  struct tm *timeInfo;
  timeInfo = localtime(&epochTime);  // Convert epoch to local time
  char buffer[9];
  strftime(buffer, sizeof(buffer), "%H:%M:%S", timeInfo);  // Format time as HH:MM:SS
  return String(buffer);
}

// Function to display time (local or UTC) with change detection
void displayTime(int x, int y, String time, String& previousTime, int yOffset) {
  // Define the calculated positions for each character
  int positions[] = {x, x + 48, x + 78, x + 108, x + 156, x + 186, x + 216, x + 264};

  // Loop over the time string and compare it with the previous time
  for (int i = 0; i < time.length(); i++) {
    if (time[i] != previousTime[i]) {  // If the digit is different
      tft.setTextColor(TFT_BLACK);
      tft.drawString(String(previousTime[i]), positions[i], y + yOffset, 1); // Erase previous digit in black
      tft.setTextColor(TFT_WHITE);
      tft.drawString(String(time[i]), positions[i], y + yOffset, 1); // Draw new digit in white
    }
  }

  // Update the previous time to the new time for the next loop
  previousTime = time;
}

// PNG Decoder Callback Functions
void *fileOpen(const char *filename, int32_t *size) {
  String fullPath = "/" + String(filename);
  pngFile = SPIFFS.open(fullPath, "r");
  if (!pngFile) return nullptr;
  *size = pngFile.size();
  return (void *)&pngFile;
}

void fileClose(void *handle) {
  ((fs::File *)handle)->close();
}

int32_t fileRead(PNGFILE *handle, uint8_t *buffer, int32_t length) {
  return ((fs::File *)handle->fHandle)->read(buffer, length);
}

int32_t fileSeek(PNGFILE *handle, int32_t position) {
  return ((fs::File *)handle->fHandle)->seek(position);
}

// Function to display PNG from SPIFFS
void displayPNGfromSPIFFS(const char *filename, int duration_ms) {
  if (!SPIFFS.begin(true)) {
    Serial.println("Failed to mount SPIFFS!");
    return;
  }

  int16_t rc = png.open(filename, fileOpen, fileClose, fileRead, fileSeek, [](PNGDRAW *pDraw) {
    uint16_t lineBuffer[480];  // Adjust to your screen width if needed
    png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xFFFFFFFF);
    tft.pushImage(0, pDraw->y, pDraw->iWidth, 1, lineBuffer);
  });

  if (rc == PNG_SUCCESS) {
    Serial.printf("Displaying PNG: %s\n", filename);
    tft.startWrite();
    png.decode(nullptr, 0);
    tft.endWrite();
  } else {
    Serial.println("PNG decode failed.");
  }

  delay(duration_ms);
}
