#include "OpenMeteoWeather.h"
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

int main(int argc, char **argv) {
    const char *fixture = R"({
      "utc_offset_seconds": 20700,
      "current": {"time": 1761868800, "temperature_2m": -2.5,
        "relative_humidity_2m": 82, "pressure_msl": 1013.2, "weather_code": 73},
      "daily": {"sunrise": [1761861600], "sunset": [1761901200]}
    })";
    JsonDocument doc;
    assert(!deserializeJson(doc, fixture));
    OpenMeteoWeather weather{};
    assert(parseOpenMeteoWeather(doc.as<JsonVariantConst>(), weather));
    assert(weather.temperature == -2.5f && weather.humidity == 82);
    assert(weather.utcOffsetSeconds == 20700); // UTC+05:45 must not truncate.
    assert(weather.sunrise + weather.utcOffsetSeconds == 1761882300);
    assert(std::strcmp(openMeteoDescription(weather.code), "Moderate snow") == 0);
    assert(std::strcmp(openMeteoDescription(0), "Clear sky") == 0);
    assert(std::strcmp(openMeteoDescription(99), "Thunderstorm with heavy hail") == 0);
    assert(std::strcmp(openMeteoDescription(1000), "Unknown conditions") == 0);

    // Polar locations can have no sunrise/sunset without losing current weather.
    doc["daily"]["sunrise"][0] = nullptr;
    doc["daily"]["sunset"][0] = nullptr;
    assert(parseOpenMeteoWeather(doc.as<JsonVariantConst>(), weather));
    assert(weather.sunrise == 0 && weather.sunset == 0);
    doc["utc_offset_seconds"] = -12600;
    assert(parseOpenMeteoWeather(doc.as<JsonVariantConst>(), weather));
    assert(weather.utcOffsetSeconds == -12600);

    for (const char *field : {"time", "temperature_2m", "relative_humidity_2m", "pressure_msl", "weather_code"}) {
        assert(!deserializeJson(doc, fixture));
        doc["current"][field] = nullptr;
        assert(!parseOpenMeteoWeather(doc.as<JsonVariantConst>(), weather));
    }
    assert(!deserializeJson(doc, fixture));
    doc["current"]["relative_humidity_2m"] = 101;
    assert(!parseOpenMeteoWeather(doc.as<JsonVariantConst>(), weather));
    assert(!deserializeJson(doc, R"({"error":true,"reason":"Invalid coordinates"})"));
    assert(!parseOpenMeteoWeather(doc.as<JsonVariantConst>(), weather));
    assert(deserializeJson(doc, "not json"));
    assert(!parseOpenMeteoWeather(doc.as<JsonVariantConst>(), weather));

    if (argc == 2) {
        std::ifstream file(argv[1]);
        assert(file.good());
        std::string payload((std::istreambuf_iterator<char>(file)), {});
        assert(!deserializeJson(doc, payload));
        assert(parseOpenMeteoWeather(doc.as<JsonVariantConst>(), weather));
        std::cout << "Live API: " << weather.temperature << " C, "
                  << openMeteoDescription(weather.code) << ", UTC offset "
                  << weather.utcOffsetSeconds << " seconds\n";
    }
    std::cout << "Open-Meteo parser tests passed\n";
}
