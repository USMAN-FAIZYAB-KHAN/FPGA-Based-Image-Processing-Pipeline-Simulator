#ifndef COLOR_CONVERTER_HPP
#define COLOR_CONVERTER_HPP

#include "pixel.hpp"
#include <cstdint>
#include <iostream>

namespace hardware {

    struct ColorConverter {   
        // Converts RGB pixel to Grayscale using either fixed-point or float math
        static GrayPixel convert(const RGBPixel& p) {
        
            uint8_t resultValue;

            #ifdef FPGA_MODE
                // Fixed-point: Y = (77R + 150G + 29B) / 256
                uint32_t rTerm = static_cast<uint32_t>(p.r) * 77;
                uint32_t gTerm = static_cast<uint32_t>(p.g) * 150;
                uint32_t bTerm = static_cast<uint32_t>(p.b) * 29;
                // Division by 256
                resultValue = static_cast<uint8_t>((rTerm + gTerm + bTerm) >> 8);
            #else
                // Standard floating-point weighted average (luminosity method)
                // Accounts for human vision
                float result = (0.299f * p.r) + (0.587f * p.g) + (0.114f * p.b);
                resultValue = static_cast<uint8_t>(result);
            #endif

            return GrayPixel(resultValue);
        }
    };
}

#endif
