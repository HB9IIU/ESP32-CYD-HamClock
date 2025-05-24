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
#include <HB9IIU7seg42ptItalic.h> // https://rop.nl/truetype2gfx/ https://fontforge.org/en-US/
#include <HB9IIUOrbitronMed8pt.h>
#include <HB9IIOrbitronMed10pt.h>
#include <HB9IIU7seg42ptNormal.h>
#include <PNGdec.h>
#include <SPIFFS.h>
#include <config.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>
#include <WebServer.h>

// Global variables for configuration
String SSID = WIFI_SSID; // Wi-Fi credentials
String WiFiPassword = WIFI_PASSWORD;
String apiKey = WEATHER_API_KEY; // API Key

int tOffset = 2; // e.g. 2 = CEST

// Create web server
WebServer server(80); // HTTP server on port 80

// Configurable Settings (replace all previous #defines)
float latitude = 46.4667118;
float longitude = 6.8590456;
uint16_t localTimeColour = TFT_GREEN;
uint16_t utcTimeColour = TFT_GOLD;
bool doubleFrame = false;
uint16_t localFrameColour = TFT_DARKGREY;
uint16_t utcFrameColour = TFT_DARKGREY;
uint16_t bannerColour = TFT_DARKGREEN;
int bannerSpeed = 5;
String localTimeLabel = "  QTH Time  ";
String utcTimeLabel = "  UTC Time  ";
String startupLogo = "logo1.png";
bool italicClockFonts = false;
volatile bool refreshDigits = false;
const String weatherAPI = "https://api.openweathermap.org/data/2.5/weather"; // OpenWeather API endpoint

int retriesBeforeReboot = 5;

// Global variables for previous time tracking
String previousLocalTime = "";
String previousUTCtime = "";
int refreshDigitsCounter = 0;
volatile bool refreshFrames = false;
int refreshFramesCounter = 0;

// TFT Display Setup
TFT_eSPI tft = TFT_eSPI();                   // Create TFT display object
TFT_eSprite stext2 = TFT_eSprite(&tft);      // Sprite object for "Hello World" text
TFT_eSprite progressBar = TFT_eSprite(&tft); // Create sprite for OTA progress bar

// Scrolling Text
int textX;                                                                                      // Variable for text position (to start at the rightmost side)
String scrollText = "Sorry, No Weather Info At This Moment!!! Have you enterred your API key?"; // Text to scroll
// Timing variables
unsigned long previousMillisForScroller = 0; // Store last time the action was performed

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
void displayTime(int x, int y, String time, String &previousTime, int yOffset, uint16_t fontColor);
String convertTimestampToDate(long timestamp);
void loadSettings();
void handleRoot();
void handleSave();
void drawOrredrawStaticElements();

// PNG Decoder Setup
PNG png;
fs::File pngFile; // Global File handle (required for PNGdec callbacks)

// Callback functions for PNGdec
void *fileOpen(const char *filename, int32_t *size);
void fileClose(void *handle);
int32_t fileRead(PNGFILE *handle, uint8_t *buffer, int32_t length);
int32_t fileSeek(PNGFILE *handle, int32_t position);
void displayPNGfromSPIFFS(const char *filename, int duration_ms);

void saveSettings();
void setup()
{
    // Start Serial Monitor
    Serial.begin(115200);
    Serial.println("Starting setup...");
    // 🔧 Mount SPIFFS
    if (!SPIFFS.begin(true))
    {
        Serial.println("❌ SPIFFS mount failed!");
        return;
    }
    // Load saved settings first
    loadSettings();
    // saveSettings();
    //  bannerSpeed=40;
    //   Initialize TFT display
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    Serial.println("TFT Display initialized!");

    // Display PNG from SPIFFS
    displayPNGfromSPIFFS(startupLogo.c_str(), 0);

    // Connect to Wi-Fi
    connectWiFi();

    // Start OTA
    ArduinoOTA.setHostname("hb9iiuhamclock"); // 🧠 Make sure OTA uses the same hostname

    ArduinoOTA.onStart([]()
                       {
    String type = (ArduinoOTA.getCommand() == U_FLASH) ? "Firmware" : "Filesystem";

    Serial.println("🛠️ OTA Update Start: " + type);

    // Clear TFT and display OTA message
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setFreeFont(&Orbitron_Light_32);
    tft.drawCentreString("Receiving New", 160, 10, 1); 
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawCentreString(type, 160, 70, 1); });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                          {
                              int percent = (progress * 100) / total;
                              Serial.printf("Progress: %u%%\r", percent);

                              // Clear the sprite
                              progressBar.fillSprite(TFT_DARKGREY);

                              // Draw filled portion
                              int barWidth = map(percent, 0, 100, 0, 300);
                              progressBar.fillRect(0, 0, barWidth, 30, TFT_GREEN);

                              // Optional: Draw percent text
                              char buf[16];
                              sprintf(buf, "%d%%", percent);
                              progressBar.setTextDatum(CC_DATUM);
                              ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                                                    {
                                                        int percent = (progress * 100) / total;
                                                        Serial.printf("Progress: %u%%\r", percent);

                                                        // Clear the sprite
                                                        progressBar.fillSprite(TFT_DARKGREY);

                                                        // Draw filled portion
                                                        int barWidth = map(percent, 0, 100, 0, 300);
                                                        progressBar.fillRect(0, 0, barWidth, 30, TFT_GREEN);

                                                        // Optional: Draw percent text
                                                        char buf[16];
                                                        sprintf(buf, "%d%%", percent);
                                                        progressBar.setTextDatum(CC_DATUM);
                                                        progressBar.setTextColor(TFT_WHITE, TFT_DARKGREY);
                                                        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                                                                              {
                                                                                  int percent = (progress * 100) / total;
                                                                                  Serial.printf("Progress: %u%%\r", percent);

                                                                                  // Clear the sprite
                                                                                  progressBar.fillSprite(TFT_DARKGREY);

                                                                                  // Draw filled portion
                                                                                  int barWidth = map(percent, 0, 100, 0, 300);
                                                                                  progressBar.fillRect(0, 0, barWidth, 30, TFT_GREEN);

                                                                                  // Optional: Draw percent text
                                                                                  char buf[16];
                                                                                  sprintf(buf, "%d%%", percent);
                                                                                  progressBar.setTextDatum(CC_DATUM);
                                                                                  progressBar.setTextSize(2);
                                                                                  progressBar.setTextColor(TFT_BLACK);

                                                                                  progressBar.drawString(buf, 150, 15); // Centered

                                                                                  // Push to screen
                                                                                  progressBar.pushSprite(10, 140); // Position on screen
                                                                              });

                                                        progressBar.drawString(buf, 150, 15); // Centered

                                                        // Push to screen
                                                        progressBar.pushSprite(10, 140); // Position on screen
                                                    });

                              progressBar.drawString(buf, 150, 15); // Centered

                              // Push to screen
                              progressBar.pushSprite(10, 140); // Position on screen
                          });

    ArduinoOTA.onEnd([]()
                     {
                         Serial.println("✅ OTA Update Completed");

                         tft.fillScreen(TFT_BLACK);
                         tft.setTextColor(TFT_GREEN, TFT_BLACK);
                         tft.setFreeFont(&Orbitron_Light_32);
                         tft.drawCentreString("Update Done!", 160, 10, 1);

                         // Use smaller Orbitron font for info
                         tft.setFreeFont(&Orbitron_Medium8pt7b);

                         // 1️⃣ Free RAM (Heap)
                         uint32_t freeHeap = ESP.getFreeHeap();
                         uint32_t totalHeap = ESP.getHeapSize();
                         float heapUsage = 100.0f * (1.0f - ((float)freeHeap / totalHeap));
                         int heapUsedPercent = (int)heapUsage;

                         String heapComment;
                         uint16_t heapColor;

                         if (heapUsedPercent < 60)
                         {
                             heapComment = "Memory status: Excellent";
                             heapColor = TFT_GREEN;
                         }
                         else if (heapUsedPercent < 80)
                         {
                             heapComment = "Memory status: OK";
                             heapColor = TFT_ORANGE;
                         }
                         else
                         {
                             heapComment = "Memory status: Low";
                             heapColor = TFT_RED;
                         }

                         char buf[64];

                         // Heap usage in kB — split into two lines
                         float freeKB = freeHeap / 1024.0;
                         float totalKB = totalHeap / 1024.0;
                         sprintf(buf, "Free RAM: %.1f / %.1f kB", freeKB, totalKB);
                         tft.setTextColor(TFT_CYAN, TFT_BLACK);
                         tft.drawCentreString(buf, 160, 60, 1);

                         sprintf(buf, "(%d%% used)", heapUsedPercent);
                         tft.drawCentreString(buf, 160, 75, 1);

                         tft.setTextColor(heapColor, TFT_BLACK);
                         tft.drawCentreString(heapComment, 160, 92, 1);

                         // 2️⃣ Sketch info in kB
                         float usedSketchKB = ESP.getSketchSize() / 1024.0;
                         float freeSketchKB = ESP.getFreeSketchSpace() / 1024.0;
                         sprintf(buf, "Sketch: %.1f / %.1f kB", usedSketchKB, freeSketchKB);
                         tft.setTextColor(TFT_YELLOW, TFT_BLACK);
                         tft.drawCentreString(buf, 160, 115, 1);

                         // 3️⃣ Uptime
                         sprintf(buf, "Uptime: %.1f sec", millis() / 1000.0);
                         tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
                         tft.drawCentreString(buf, 160, 135, 1);

                         // Final message
                         tft.setTextColor(TFT_WHITE, TFT_BLACK);
                         tft.drawCentreString("73 from HB9IIU", 160, 200, 1);

                         delay(7000); // Show for 8 seconds before reboot or resume
                     });

    ArduinoOTA.onError([](ota_error_t error)
                       {
                           Serial.printf("❌ OTA Error[%u]: ", error);
                           if (error == OTA_AUTH_ERROR)
                               Serial.println("Auth Failed");
                           else if (error == OTA_BEGIN_ERROR)
                               Serial.println("Begin Failed");
                           else if (error == OTA_CONNECT_ERROR)
                               Serial.println("Connect Failed");
                           else if (error == OTA_RECEIVE_ERROR)
                               Serial.println("Receive Failed");
                           else if (error == OTA_END_ERROR)
                               Serial.println("Failed");

                           tft.fillScreen(TFT_BLACK);
                           tft.setTextColor(TFT_RED, TFT_BLACK);
                           tft.setFreeFont(&Orbitron_Light_32);
                           tft.drawCentreString("❌ Update Failed", 160, 40, 1);

                           switch (error)
                           {
                           case OTA_AUTH_ERROR:
                               tft.drawCentreString("Auth Failed", 160, 90, 1);
                               break;
                           case OTA_BEGIN_ERROR:
                               tft.drawCentreString("Begin Failed", 160, 90, 1);
                               break;
                           case OTA_CONNECT_ERROR:
                               tft.drawCentreString("Connect Failed", 160, 90, 1);
                               break;
                           case OTA_RECEIVE_ERROR:
                               tft.drawCentreString("Receive Failed", 160, 90, 1);
                               break;
                           case OTA_END_ERROR:
                               tft.drawCentreString("End Failed", 160, 90, 1);
                               break;
                           default:
                               tft.drawCentreString("Unknown Error", 160, 90, 1);
                               break;
                           }
                           // 📻 Friendly sign-off
                           tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
                           tft.setFreeFont(&Orbitron_Medium8pt7b);
                           tft.drawCentreString("73! from HB9IIU", 160, 200, 1); // Adjust Y to your screen

                           delay(4000); // Let the user read the error
                       });

    ArduinoOTA.begin();
    Serial.println("🚀 OTA Ready");

    // Start mDNS AFTER OTA
    if (!MDNS.begin("hb9iiuhamclock"))
    {
        Serial.println("⚠️ Failed to start mDNS responder!");
    }
    else
    {
        Serial.println("🌍 mDNS started successfully. You can access via http://hb9iiuhamclock.local");
    }

    // Start Web Server
    server.on("/", handleRoot);                       // Serve the HTML page
    server.on("/save", HTTP_POST, handleSave);        // Handle form submit
                                                      // Serve all static files (HTML, PNG, CSS, etc.)
    server.serveStatic("/images", SPIFFS, "/images"); // if you have images in /images/
    server.serveStatic("/fonts", SPIFFS, "/fonts");   // optional
    server.serveStatic("/logo1.png", SPIFFS, "/logo1.png");
    server.serveStatic("/logo2.png", SPIFFS, "/logo2.png");
    server.serveStatic("/logo3.png", SPIFFS, "/logo3.png");

    Serial.println("🌐 Web server started at http://" + WiFi.localIP().toString());

    server.on("/config", HTTP_GET, []()
              {
  StaticJsonDocument<1024> doc;

  doc["latitude"] = latitude;
  doc["longitude"] = longitude;
  doc["localTimeColour"] = localTimeColour;
  doc["utcTimeColour"] = utcTimeColour;
  doc["doubleFrame"] = doubleFrame;
  doc["localFrameColour"] = localFrameColour;
  doc["utcFrameColour"] = utcFrameColour;
  doc["bannerColour"] = bannerColour;
  doc["bannerSpeed"] = bannerSpeed;
  doc["localTimeLabel"] = localTimeLabel;
  doc["utcTimeLabel"] = utcTimeLabel;
  doc["startupLogo"] = startupLogo;
  doc["italicClockFonts"] = italicClockFonts;

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response); });

    server.on("/scrolltext", []()
              { server.send(200, "text/plain", scrollText); });

    server.on("/setcolor", HTTP_POST, []()
              {
    if (!server.hasArg("plain")) {
        server.send(400, "text/plain", "Missing body");
        return;
    }

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));

    if (error) {
        server.send(400, "text/plain", "JSON parse error");
        return;
    }

    String target = doc["target"];
    
if (target == "doubleFrame") {
    // Invert the received value: if thin is true, then doubleFrame should be false
    bool thinBorder = doc["value"];
    doubleFrame = !thinBorder;
    Serial.printf("🪟 doubleFrame set to: %s (thinBorder: %s)\n", doubleFrame ? "true" : "false", thinBorder ? "true" : "false");
    drawOrredrawStaticElements();
    server.send(200, "text/plain", "OK");
    return;
}

    uint16_t color = doc["color"];

    if (target == "localTimeDigits") {
        localTimeColour = color;
        Serial.printf("🎨 localTimeColour set to 0x%04X\n", localTimeColour);
    } else if (target == "localTimeFrame") {
        localFrameColour = color;
        Serial.printf("🖼️ localFrameColour set to 0x%04X\n", localFrameColour);
    } else if (target == "utcTimeDigits") {
        utcTimeColour = color;
        Serial.printf("🎨 utcTimeColour set to 0x%04X\n", utcTimeColour);
    } else if (target == "utcTimeFrame") {
        utcFrameColour = color;
        Serial.printf("🖼️ utcFrameColour set to 0x%04X\n", utcFrameColour);
    } else if (target == "weatherBannerText") {
        bannerColour = color;
        Serial.printf("🟩 bannerColour set to 0x%04X\n", bannerColour);
    } else {
        server.send(400, "text/plain", "Unknown target");
        return;
    }

    drawOrredrawStaticElements();
    refreshDigits = true;

    server.send(200, "text/plain", "OK"); });

    server.on("/setspeed", HTTP_POST, []()
              {
    if (!server.hasArg("plain")) {
        server.send(400, "text/plain", "Missing body");
        return;
    }

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));
    if (error) {
        server.send(400, "text/plain", "JSON parse error");
        return;
    }

    int speed = doc["speed"];  // This will already be 45 - slider
    bannerSpeed = constrain(speed, 0, 45);
    Serial.println(bannerSpeed);

    Serial.printf("🎬 bannerSpeed set to %d seconds\n", bannerSpeed);
    server.send(200, "text/plain", "OK"); });

    // ESP32 WebServer endpoint for setting labels without saving
    server.on("/setlabel", HTTP_POST, []()
              {
    if (!server.hasArg("plain")) {
        server.send(400, "text/plain", "Missing body");
        return;
    }

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));
    if (error) {
        server.send(400, "text/plain", "JSON parse error");
        return;
    }

    String target = doc["target"];
    String value = doc["value"];

    if (target == "localTimeLabel") {
 localTimeLabel = "  " + value + "  ";
        Serial.printf("🕒 Updated localTimeLabel: %s\n", localTimeLabel.c_str());
    } else if (target == "utcTimeLabel") {
        utcTimeLabel = "  " + value + "  ";;
        Serial.printf("🌐 Updated utcTimeLabel: %s\n", utcTimeLabel.c_str());
    } else {
        server.send(400, "text/plain", "Unknown target");
        return;
    }

    // Redraw labels immediately on screen
    refreshFrames = true;
    drawOrredrawStaticElements();

    server.send(200, "text/plain", "OK"); });

    server.on("/setposition", HTTP_POST, []()
              {
    if (!server.hasArg("plain")) {
        server.send(400, "text/plain", "Missing body");
        Serial.println("⚠️ No body received");
        return;
    }

    String body = server.arg("plain");
    Serial.println("📩 Received JSON:");
    Serial.println(body);  // 🔍 Print raw incoming JSON for debugging

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, body);
    if (error) {
        Serial.println("❌ JSON parse error");
        server.send(400, "text/plain", "JSON parse error");
        return;
    }

    if (!doc.containsKey("latitude") || !doc.containsKey("longitude")) {
        Serial.println("⚠️ Missing latitude or longitude in JSON");
        server.send(400, "text/plain", "Missing latitude or longitude");
        return;
    }

    latitude = doc["latitude"].as<float>();
    longitude = doc["longitude"].as<float>();

    Serial.printf("📍 Latitude updated to: %.6f\n", latitude);
    Serial.printf("📍 Longitude updated to: %.6f\n", longitude);

    fetchWeatherData();

    server.send(200, "text/plain", "OK"); });

    server.on("/setitalic", HTTP_POST, []()
              {
    if (!server.hasArg("plain")) {
        server.send(400, "text/plain", "Missing body");
        return;
    }

    StaticJsonDocument<128> doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));
    if (error) {
        server.send(400, "text/plain", "JSON parse error");
        return;
    }

    italicClockFonts = doc["italicClockFonts"] | italicClockFonts;

    Serial.printf("✏️ italicClockFonts set to: %s\n", italicClockFonts ? "true" : "false");

    drawOrredrawStaticElements();
    // Optionally persist
    // saveSettings();

    server.send(200, "text/plain", "OK"); });

    server.on("/saveall", HTTP_POST, []()
              {
    saveSettings();
    server.send(200, "text/plain", "💾 Settings saved to flash"); });

    server.on("/setbootimage", HTTP_POST, []()
              {
                  if (!server.hasArg("plain"))
                  {
                      server.send(400, "text/plain", "Missing body");
                      return;
                  }

                  StaticJsonDocument<256> doc;
                  DeserializationError error = deserializeJson(doc, server.arg("plain"));
                  if (error)
                  {
                      server.send(400, "text/plain", "JSON parse error");
                      return;
                  }

                  if (!doc.containsKey("bootImageId"))
                  {
                      server.send(400, "text/plain", "Missing bootImageId");
                      return;
                  }

                  startupLogo = doc["bootImageId"].as<String>();
                  Serial.printf("🖼️ Boot logo updated to: %s\n", startupLogo.c_str());

                  server.send(200, "text/plain", "Boot logo saved");
                  saveSettings(); // 💾 Persist the change
              });

    server.on("/setbootimage", HTTP_POST, []()
              {
    if (!server.hasArg("plain")) {
        server.send(400, "text/plain", "Missing body");
        return;
    }

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, server.arg("plain"));
    if (error) {
        server.send(400, "text/plain", "JSON parse error");
        return;
    }

    if (!doc.containsKey("bootImageId")) {
        server.send(400, "text/plain", "Missing bootImageId");
        return;
    }

    startupLogo = doc["bootImageId"].as<String>();
    Serial.printf("🖼️ Boot logo updated to: %s\n", startupLogo.c_str());

    saveSettings(); // 💾 Persist the change

    server.send(200, "text/plain", "Boot logo saved"); });

    server.begin();

    // Initialize NTP Client
    timeClient.begin();
    timeClient.setTimeOffset(0); // UTC Offset (0 for UTC)
    Serial.println("NTP Client initialized.");
    tft.fillScreen(TFT_BLACK);

    fetchWeatherData();

    drawOrredrawStaticElements();

    // Create a sprite for the Weather text
    progressBar.setColorDepth(8);      // Use 8-bit color for efficiency
    progressBar.createSprite(300, 30); // Width and height of the bar

    stext2.setColorDepth(8);
    stext2.createSprite(310, 30);      // Create a 310x20 sprite to accommodate the text width
    stext2.setTextColor(bannerColour); // White text
    stext2.setTextDatum(TL_DATUM);     // Top-left alignment for text

    // Set the font for the sprite
    stext2.setFreeFont(&Orbitron_Medium10pt7b); // Apply custom font to the sprite

    // Calculate the initial position (rightmost position)
    textX = stext2.width();
}

void loop()
{
    ArduinoOTA.handle();
    server.handleClient(); // ⬅️ Serve HTTP requests
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
    if (italicClockFonts)
    {
        tft.setFreeFont(&digital_7_monoitalic42pt7b);
    }
    else
    {
        tft.setFreeFont(&digital_7__mono_42pt7b);
    }
    // Corrected y positions for both clocks
    displayTime(8, 5, localTime, previousLocalTime, 0, localTimeColour); // Display local time at y = 5

    displayTime(10, 107, utcTime, previousUTCtime, 0, utcTimeColour); // Display UTC time at y = 106

    // Fetch Weather Data once every 5 minutes
    if (currentMillis - previousMillis >= 1000 * 60 * 5)
    {
        previousMillis = currentMillis; // Save the current time
        fetchWeatherData();
    }
    // Check if the interval has passed
    if (currentMillis - previousMillisForScroller >= bannerSpeed)
    {
        // Save the last time the action was performed
        previousMillisForScroller = currentMillis;

        // Clear the sprite
        stext2.fillSprite(TFT_BLACK); // Fill sprite with background color

        // Draw the text inside the sprite at the specified position
        stext2.setTextColor(bannerColour);
        stext2.drawString(scrollText, textX, 0); // Draw text in sprite at position `textX`

        // Scroll the text by shifting the position to the left
        textX -= 1; // Move text left by 1 pixel

        // Reset position when text has scrolled off the screen
        if (textX < -stext2.textWidth(scrollText))
        {                           // Text has completely scrolled off screen
            textX = stext2.width(); // Reset position to the far right
        }

        // Push the sprite onto the TFT at the specified coordinates
        stext2.pushSprite(5, 205); // Push the sprite to the screen at position (5, 220)
    }
}

// 📶 Function to connect to Wi-Fi and initialize mDNS
void connectWiFi()
{
    // Set a custom hostname BEFORE connecting to Wi-Fi
    WiFi.setHostname("hb9iiuhamclock");
    String hostname = WiFi.getHostname();

    Serial.println("🔌 Connecting to Wi-Fi...");
    Serial.print("📡 SSID: ");
    Serial.println(SSID);
    Serial.print("🧾 Hostname: ");
    Serial.println(hostname);

    WiFi.begin(SSID, WiFiPassword);

    // Attempt to connect, retrying with a delay
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("⏳ Waiting for Wi-Fi connection...");
        retryCount++;

        if (retryCount >= retriesBeforeReboot)
        {
            Serial.println("❌ Wi-Fi connection failed too many times. Rebooting...");
            ESP.restart();
        }
    }

    Serial.println("✅ Wi-Fi connected!");
    Serial.print("📶 IP Address: ");
    Serial.println(WiFi.localIP());
}

// Fetch weather data
void fetchWeatherData()
{
    HTTPClient http;
    String weatherURL = weatherAPI + "?lat=" + String(latitude) + "&lon=" + String(longitude) + "&appid=" + apiKey + "&units=metric";

    // Make GET Request
    http.begin(weatherURL);
    Serial.println("");
    Serial.println(weatherURL);
    Serial.println("");

    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
        String payload = http.getString();
        Serial.println(payload);
        Serial.println("Weather data received.");

        // Parse the JSON response
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, payload);

        // Extracting values from the JSON response and assigning to variables

        // Coordinates
        float lon = doc["coord"]["lon"];
        float lat = doc["coord"]["lat"];

        // Weather
        int weatherId = doc["weather"][0]["id"];
        const char *weatherMain = doc["weather"][0]["main"];
        const char *weatherDescription = doc["weather"][0]["description"];
        const char *weatherIcon = doc["weather"][0]["icon"];

        // Base
        const char *base = doc["base"];

        // Main weather data
        float temp = doc["main"]["temp"];
        float feels_like = doc["main"]["feels_like"];
        float temp_min = doc["main"]["temp_min"];
        float temp_max = doc["main"]["temp_max"];
        int pressure = doc["main"]["pressure"];
        int humidity = doc["main"]["humidity"];
        int sea_level = doc["main"]["sea_level"];
        int grnd_level = doc["main"]["grnd_level"];

        // Visibility
        int visibility = doc["visibility"];

        // Wind data
        float wind_speed = doc["wind"]["speed"];
        int wind_deg = doc["wind"]["deg"];
        float wind_gust = doc["wind"]["gust"];

        // Rain data
        float rain_1h = doc["rain"]["1h"];

        // Clouds data
        int clouds_all = doc["clouds"]["all"];

        // Date/Time
        long dt = doc["dt"];

        // System data
        int sys_type = doc["sys"]["type"];
        int sys_id = doc["sys"]["id"];
        const char *sys_country = doc["sys"]["country"];
        long sunrise = doc["sys"]["sunrise"];
        long sunset = doc["sys"]["sunset"];

        // Timezone
        int timezone = doc["timezone"];

        // Location data
        int id = doc["id"];
        const char *name = doc["name"];

        // Status code
        int cod = doc["cod"];

        // Print the extracted values
        Serial.println("Weather data received.");
        Serial.print("Coordinates: ");
        Serial.print("Longitude: ");
        Serial.print(lon);
        Serial.print(", Latitude: ");
        Serial.println(lat);

        Serial.print("Weather ID: ");
        Serial.println(weatherId);
        Serial.print("Main: ");
        Serial.println(weatherMain);
        Serial.print("Description: ");
        Serial.println(weatherDescription);
        Serial.print("Icon: ");
        Serial.println(weatherIcon);

        Serial.print("Base: ");
        Serial.println(base);

        Serial.print("Temperature: ");
        Serial.println(temp);
        Serial.print("Feels like: ");
        Serial.println(feels_like);
        Serial.print("Min Temp: ");
        Serial.println(temp_min);
        Serial.print("Max Temp: ");
        Serial.println(temp_max);
        Serial.print("Pressure: ");
        Serial.println(pressure);
        Serial.print("Humidity: ");
        Serial.println(humidity);
        Serial.print("Sea level: ");
        Serial.println(sea_level);
        Serial.print("Ground level: ");
        Serial.println(grnd_level);

        Serial.print("Visibility: ");
        Serial.println(visibility);

        Serial.print("Wind speed: ");
        Serial.println(wind_speed);
        Serial.print("Wind degree: ");
        Serial.println(wind_deg);
        Serial.print("Wind gust: ");
        Serial.println(wind_gust);

        Serial.print("Rain 1h: ");
        Serial.println(rain_1h);

        Serial.print("Clouds: ");
        Serial.println(clouds_all);

        Serial.print("Timestamp: ");
        Serial.println(dt);

        Serial.print("System type: ");
        Serial.println(sys_type);
        Serial.print("System ID: ");
        Serial.println(sys_id);
        Serial.print("Country: ");
        Serial.println(sys_country);
        Serial.print("Sunrise: ");
        Serial.println(sunrise);
        Serial.print("Sunset: ");
        Serial.println(sunset);

        Serial.print("Timezone: ");
        Serial.println(timezone);

        Serial.print("Location ID: ");
        Serial.println(id);
        Serial.print("Location Name: ");
        Serial.println(name);

        Serial.print("Status code: ");
        Serial.println(cod);

        // Convert sunrise and sunset times to local time
        long localSunrise = sunrise + (tOffset * 3600); // Adjust for local time (seconds)
        long localSunset = sunset + (tOffset * 3600);   // Adjust for local time (seconds)

        // Convert sunrise and sunset times to human-readable format
        String sunriseTime = convertEpochToTimeString(localSunrise);
        String sunsetTime = convertEpochToTimeString(localSunset);
        String date = convertTimestampToDate(dt); // Convert to DD:MM:YY format
        // Build the scrollText with the date, weather, sunrise, and sunset times
        scrollText = String(name) + "     " + sys_country + "    " +
                     date + "     " +
                     "Temp: " + String(temp, 1) + "°C     " + // One decimal place for temp
                     "RH: " + String(humidity) + "%" + "       " +
                     String(weatherDescription) + "       " +
                     "Sunrise: " + sunriseTime + "     " +
                     "Sunset: " + sunsetTime;

        stext2.drawString(scrollText, textX, 0); // Draw text in sprite at position `textX`
        textX = stext2.width();
        Serial.println(scrollText);
    }
    else
    {
        Serial.print("Error fetching weather data, HTTP code: ");
        Serial.println(httpCode);
        scrollText = "Sorry, No Weather Info At This Moment!!!"; // Text to scroll
        textX = stext2.width();
    }

    http.end();
}

// Function to format the local time from epoch time
String formatLocalTime(long epochTime)
{
    struct tm *timeInfo;
    timeInfo = localtime(&epochTime); // Convert epoch to local time
    char buffer[9];
    strftime(buffer, sizeof(buffer), "%H:%M:%S", timeInfo); // Format time as HH:MM:SS
    return String(buffer);
}

// Function to convert an epoch time to a human-readable time string
String convertEpochToTimeString(long epochTime)
{
    struct tm *timeInfo;
    timeInfo = localtime(&epochTime); // Convert epoch to local time
    char buffer[9];
    strftime(buffer, sizeof(buffer), "%H:%M:%S", timeInfo); // Format time as HH:MM:SS
    return String(buffer);
}

// Function to display time (local or UTC) with change detection and custom font color
void displayTime(int x, int y, String time, String &previousTime, int yOffset, uint16_t fontColor)
{
    if (refreshDigits)
    {
        refreshDigitsCounter++;
        if (refreshDigitsCounter == 1)
        {
            // First call — force clear by using empty string
            previousTime = "";
        }
        else if (refreshDigitsCounter >= 2)
        {
            // Second call — stop refreshing
            previousTime = "";
            refreshDigits = false;
            refreshDigitsCounter = 0;
        }
    }

    // Define the calculated positions for each character
    int positions[] = {x, x + 48, x + 78, x + 108, x + 156, x + 186, x + 216, x + 264};

    // Loop over the time string and compare it with the previous time
    for (int i = 0; i < time.length(); i++)
    {
        if (time[i] != previousTime[i])
        {
            tft.setTextColor(TFT_BLACK);
            tft.drawString(String(previousTime[i]), positions[i], y + yOffset, 1);
            tft.setTextColor(fontColor);
            tft.drawString(String(time[i]), positions[i], y + yOffset, 1);
        }
    }

    previousTime = time;
}

// PNG Decoder Callback Functions
void *fileOpen(const char *filename, int32_t *size)
{
    String fullPath = "/" + String(filename);
    pngFile = SPIFFS.open(fullPath, "r");
    if (!pngFile)
        return nullptr;
    *size = pngFile.size();
    return (void *)&pngFile;
}

void fileClose(void *handle)
{
    ((fs::File *)handle)->close();
}

int32_t fileRead(PNGFILE *handle, uint8_t *buffer, int32_t length)
{
    return ((fs::File *)handle->fHandle)->read(buffer, length);
}

int32_t fileSeek(PNGFILE *handle, int32_t position)
{
    return ((fs::File *)handle->fHandle)->seek(position);
}

// Function to display PNG from SPIFFS

void displayPNGfromSPIFFS(const char *filename, int duration_ms)
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("Failed to mount SPIFFS!");
        return;
    }

    int16_t rc = png.open(filename, fileOpen, fileClose, fileRead, fileSeek, [](PNGDRAW *pDraw)
                          {
    uint16_t lineBuffer[480];  // Adjust to your screen width if needed
    png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xFFFFFFFF);
    tft.pushImage(0, pDraw->y, pDraw->iWidth, 1, lineBuffer); });

    if (rc == PNG_SUCCESS)
    {
        Serial.printf("Displaying PNG: %s\n", filename);
        tft.startWrite();
        png.decode(nullptr, 0);
        tft.endWrite();
    }
    else
    {
        Serial.println("PNG decode failed.");
    }

    delay(duration_ms);
}

// Function to convert Unix timestamp to human-readable format (DD:MM:YY)
String convertTimestampToDate(long timestamp)
{
    struct tm *timeinfo;
    timeinfo = localtime(&timestamp);                       // Convert epoch to local time
    char buffer[11];                                        // Buffer for "DD:MM:YY"
    strftime(buffer, sizeof(buffer), "%d:%m:%y", timeinfo); // Format as DD:MM:YY
    return String(buffer);
}

// Load settings from SPIFFS JSON
void loadSettings()
{
    fs::File file = SPIFFS.open("/settings.json", "r"); // ✅ OPEN IN READ MODE
    if (!file)
    {
        Serial.println("⚠️ Could not open settings file. Using defaults.");
        saveSettings();
        esp_restart();
        return;
    }

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, file);
    if (error)
    {
        Serial.println("⚠️ Failed to parse settings file. Using defaults.");
        file.close();
        return;
    }

    file.close(); // Always close file after use

    latitude = doc["latitude"] | latitude;
    longitude = doc["longitude"] | longitude;
    localTimeColour = doc["localTimeColour"] | localTimeColour;
    utcTimeColour = doc["utcTimeColour"] | utcTimeColour;
    doubleFrame = doc["doubleFrame"] | doubleFrame;
    localFrameColour = doc["localFrameColour"] | localFrameColour;
    utcFrameColour = doc["utcFrameColour"] | utcFrameColour;
    bannerColour = doc["bannerColour"] | bannerColour;
    bannerSpeed = doc["bannerSpeed"] | bannerSpeed;
    localTimeLabel = doc["localTimeLabel"] | localTimeLabel;
    utcTimeLabel = doc["utcTimeLabel"] | utcTimeLabel;
    startupLogo = doc["startupLogo"] | startupLogo;
    italicClockFonts = doc["italicClockFonts"] | italicClockFonts;

    Serial.println("✅ Settings loaded from SPIFFS:");
    Serial.printf("📍 latitude: %.6f\n", latitude);
    Serial.printf("📍 longitude: %.6f\n", longitude);
    Serial.printf("🎨 localTimeColour: 0x%04X\n", localTimeColour);
    Serial.printf("🎨 utcTimeColour: 0x%04X\n", utcTimeColour);
    Serial.printf("🌀 doubleFrame: %s\n", doubleFrame ? "true" : "false");
    Serial.printf("🎨 localFrameColour: 0x%04X\n", localFrameColour);
    Serial.printf("🎨 utcFrameColour: 0x%04X\n", utcFrameColour);
    Serial.printf("🖍️ bannerColour: 0x%04X\n", bannerColour);
    Serial.printf("🐢 bannerSpeed: %d\n", bannerSpeed);
    Serial.printf("🕓 localTimeLabel: %s\n", localTimeLabel.c_str());
    Serial.printf("🌍 utcTimeLabel: %s\n", utcTimeLabel.c_str());
    Serial.printf("🖼️ startupLogo: %s\n", startupLogo.c_str());
    Serial.printf("🔤 italicClockFonts: %s\n", italicClockFonts ? "true" : "false");
}

// Save settings to SPIFFS

void saveSettings()
{
    StaticJsonDocument<1024> doc;
    doc["latitude"] = latitude;
    doc["longitude"] = longitude;
    doc["localTimeColour"] = localTimeColour;
    doc["utcTimeColour"] = utcTimeColour;
    doc["doubleFrame"] = doubleFrame;
    doc["localFrameColour"] = localFrameColour;
    doc["utcFrameColour"] = utcFrameColour;
    doc["bannerColour"] = bannerColour;
    doc["bannerSpeed"] = bannerSpeed;
    doc["localTimeLabel"] = localTimeLabel;
    doc["utcTimeLabel"] = utcTimeLabel;
    doc["startupLogo"] = startupLogo;
    doc["italicClockFonts"] = italicClockFonts;

    fs::File file = SPIFFS.open("/settings.json", "w"); // ✅ This line is fixed

    if (!file)
    {
        Serial.println("❌ Failed to open settings file for writing");
        return;
    }

    serializeJsonPretty(doc, file);
    file.close();
    Serial.println("💾 Settings saved to SPIFFS");
    esp_restart();
}

// Serve index.html from SPIFFS
void handleRoot()
{

    fs::File file = SPIFFS.open("/index.html", "r"); // ✅ Declare 'file' properly here
    if (!file)
    {
        server.send(500, "text/plain", "⚠️ Failed to open index.html");
        return;
    }
    server.streamFile(file, "text/html");
    file.close();
}

// Handle POST form submission and save settings
void handleSave()
{
    if (server.hasArg("latitude"))
        latitude = server.arg("latitude").toFloat();
    if (server.hasArg("longitude"))
        longitude = server.arg("longitude").toFloat();
    if (server.hasArg("bannerSpeed"))
        bannerSpeed = server.arg("bannerSpeed").toInt();
    if (server.hasArg("localLabel"))
        localTimeLabel = server.arg("localLabel");
    if (server.hasArg("utcLabel"))
        utcTimeLabel = server.arg("utcLabel");
    if (server.hasArg("logo"))
        startupLogo = server.arg("logo");
    if (server.hasArg("italicFont"))
        italicClockFonts = (server.arg("italicFont") == "on");

    saveSettings(); // Save updated settings

    server.send(200, "text/html", "<h1>✅ Settings saved!</h1><a href='/'>Back</a>");
}
void drawOrredrawStaticElements()
{
    // Only run if we want to refresh the frames
    if (refreshFrames)
    {
        refreshFramesCounter++;
        if (refreshFramesCounter < 2)
        {
            return; // Wait for second execution
        }
        refreshFrames = false;
        refreshFramesCounter = 0;
    }
    previousLocalTime = "";
    previousUTCtime = "";
    tft.setFreeFont(&Orbitron_Medium8pt7b);
    tft.fillRect(25, 0 + 85 - 10, 270, 20, TFT_BLACK);
    tft.fillRect(25, 106 + 85 - 10, 270, 20, TFT_BLACK);

    // 🟩 Local Frame
    tft.fillRect(0, 0, 320, 87, TFT_BLACK); // Clear previous frame
    tft.drawRoundRect(1, 1, 318, 85, 4, TFT_BLACK);

    tft.drawRoundRect(0, 0, 320, 87, 5, localFrameColour);
    if (doubleFrame)
    {
        tft.drawRoundRect(1, 1, 318, 85, 4, localFrameColour);
        tft.drawRoundRect(2, 2, 316, 83, 4, localFrameColour);
        tft.drawRoundRect(3, 3, 314, 81, 4, localFrameColour);
    }

    // 🟦 Local Time Label

    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    tft.drawCentreString(localTimeLabel, 160, 76, 1);

    // 🟥 UTC Frame
    tft.fillRect(0, 105, 320, 87, TFT_BLACK); // Clear previous frame
    tft.drawRoundRect(1, 106, 318, 85, 4, TFT_BLACK);

    tft.drawRoundRect(0, 105, 320, 87, 5, utcFrameColour);
    if (doubleFrame)
    {
        tft.drawRoundRect(1, 106, 318, 85, 4, utcFrameColour);
        tft.drawRoundRect(2, 107, 316, 83, 4, utcFrameColour);
        tft.drawRoundRect(3, 108, 314, 81, 4, utcFrameColour);
    }

    // ⬜ UTC Label
    tft.drawCentreString(utcTimeLabel, 160, 76 + 105, 1);
}
