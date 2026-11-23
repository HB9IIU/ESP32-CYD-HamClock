#pragma once
#include <stdint.h>

// ILI9341 memory-access control (MADCTL) byte for a rotation, with the panel's
// color order chosen at runtime. Mirrors TFT_eSPI's non-M5 rotation table so
// it can replace the value setRotation() wrote. bgr=true swaps red and blue.
inline uint8_t displayMadctl(uint8_t rotation, bool bgr) {
    static const uint8_t orientation[4] = {0x40, 0x20, 0x80, 0xE0}; // MX, MV, MY, MX|MY|MV
    return orientation[rotation % 4] | (bgr ? 0x08 : 0x00);
}
