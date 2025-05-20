// config.h

#ifndef CONFIG_H
#define CONFIG_H

// WiFi Credentials
#define WIFI_SSID "NO WIFI FOR YOU!!!"
#define WIFI_PASSWORD "Nestle2010Nestle"

// Weather API Configuration
// To get your own OpenWeather API key:
// 1. Go to https://home.openweathermap.org/users/sign_up and create a free account.
// 2. After registering, log in and go to the API keys section to get your key.
// 3. Replace the placeholder below with your personal API key.
#define WEATHER_API_KEY "4444fcd8624a929dedc1d56235723ff3"

#define WEATHER_API_URL "https://api.openweathermap.org/data/2.5/weather"

// Location for Weather Data (Replace with your coordinates)
#define LATITUDE 46.4667118
#define LONGITUDE 6.8590456

// Time Offset (in hours) for local time
#define TIME_OFFSET 2


#define LOCAL_TIME_COLOUR TFT_GREEN
#define UTC_TIME_COLOUR TFT_GOLD
#define DOUBLE_FRAME false
#define LOCAL_FRAME TFT_DARKGREY
#define UTC_FRAME TFT_DARKGREY
#define BANNER_COLOUR TFT_DARKGREEN
#define BANNER_SPEED 20 // low number: high speed
#define LOCAL_TIME_FRAME_LABEL "  QTH Time  "
#define UTC_TIME_FRAME_LABEL "  UTC Time  "
#define START_UP_LOGO "logo1.png"  //select logo1.png /logo2.png or logo3.png
#define ITALIC_CLOCK_FONTS false // true or false

#endif // CONFIG_H

/*
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

*/