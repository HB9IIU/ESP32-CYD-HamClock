#pragma once

#include <ArduinoJson.h>
#include <cmath>

// Open-Meteo returns UTC epochs with timeformat=unixtime. Apply the offset
// exactly once when displaying local times, including half/quarter-hour zones.
struct OpenMeteoWeather {
    float temperature;
    float humidity;
    float pressure;
    int code;
    long timestamp;
    int utcOffsetSeconds;
    long sunrise; // Zero means unavailable (e.g. polar day/night).
    long sunset;
};

inline bool parseOpenMeteoWeather(JsonVariantConst root, OpenMeteoWeather &result) {
    JsonVariantConst current = root["current"];
    if (root["error"].as<bool>() ||
        !current["temperature_2m"].is<float>() ||
        !current["relative_humidity_2m"].is<float>() ||
        !current["pressure_msl"].is<float>() ||
        !current["weather_code"].is<int>() ||
        !current["time"].is<long>() ||
        !root["utc_offset_seconds"].is<int>()) {
        return false;
    }
    OpenMeteoWeather parsed{
        current["temperature_2m"].as<float>(),
        current["relative_humidity_2m"].as<float>(),
        current["pressure_msl"].as<float>(),
        current["weather_code"].as<int>(),
        current["time"].as<long>(),
        root["utc_offset_seconds"].as<int>(),
        root["daily"]["sunrise"][0].as<long>(),
        root["daily"]["sunset"][0].as<long>()
    };
    if (!std::isfinite(parsed.temperature) || !std::isfinite(parsed.humidity) ||
        !std::isfinite(parsed.pressure) || parsed.humidity < 0 ||
        parsed.humidity > 100 || parsed.pressure <= 0 || parsed.timestamp <= 0 ||
        parsed.utcOffsetSeconds < -12 * 3600 || parsed.utcOffsetSeconds > 14 * 3600) {
        return false;
    }
    result = parsed;
    return true;
}

inline const char *openMeteoDescription(int code) {
    switch (code) {
        case 0: return "Clear sky";
        case 1: return "Mainly clear";
        case 2: return "Partly cloudy";
        case 3: return "Overcast";
        case 45: return "Fog";
        case 48: return "Rime fog";
        case 51: return "Light drizzle";
        case 53: return "Moderate drizzle";
        case 55: return "Dense drizzle";
        case 56: return "Light freezing drizzle";
        case 57: return "Dense freezing drizzle";
        case 61: return "Light rain";
        case 63: return "Moderate rain";
        case 65: return "Heavy rain";
        case 66: return "Light freezing rain";
        case 67: return "Heavy freezing rain";
        case 71: return "Light snow";
        case 73: return "Moderate snow";
        case 75: return "Heavy snow";
        case 77: return "Snow grains";
        case 80: return "Light rain showers";
        case 81: return "Moderate rain showers";
        case 82: return "Violent rain showers";
        case 85: return "Light snow showers";
        case 86: return "Heavy snow showers";
        case 95: return "Thunderstorm";
        case 96: return "Thunderstorm with light hail";
        case 99: return "Thunderstorm with heavy hail";
        default: return "Unknown conditions";
    }
}
