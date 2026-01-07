#ifndef PIXEL_HPP
#define PIXEL_HPP

// Required to use uint8_t
#include <cstdint>

namespace hardware {
    // Represents a 24-bit pixel (3 bytes total)
    struct RGBPixel {
        uint8_t r, g, b;
        
        RGBPixel() : r(0), g(0), b(0) {}
        RGBPixel(uint8_t red, uint8_t green, uint8_t blue) 
            : r(red), g(green), b(blue) {}
    };
    
    // Represents an 8-bit pixel
    struct GrayPixel {
        uint8_t value;

        GrayPixel() : value(0) {}
        GrayPixel(uint8_t v) : value(v) {}
    };
}

#endif
