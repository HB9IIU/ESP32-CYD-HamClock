#include "DisplayColor.h"
#include <cassert>
#include <iostream>

int main() {
    // ILI9341 MADCTL: MY=0x80, MX=0x40, MV=0x20, BGR=0x08 (matches TFT_eSPI non-M5 rotations).
    assert(displayMadctl(0, false) == 0x40);
    assert(displayMadctl(1, false) == 0x20);
    assert(displayMadctl(2, false) == 0x80);
    assert(displayMadctl(3, false) == 0xE0);

    // Swap red/blue sets only the BGR bit.
    assert(displayMadctl(1, true) == 0x28);
    assert(displayMadctl(3, true) == 0xE8);

    // Rotation wraps like TFT_eSPI's setRotation (m % 4 for the base orientations).
    assert(displayMadctl(5, false) == displayMadctl(1, false));

    std::cout << "Display color tests passed\n";
    return 0;
}
