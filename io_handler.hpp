#ifndef IO_HANDLER_HPP
#define IO_HANDLER_HPP

#include "frame_buffer.hpp"
#include "pixel.hpp"
#include <fstream>
#include <string>
#include <iostream>

namespace hardware {

    struct IOHandler {
        
        static FrameBuffer<RGBPixel>* loadPPM(const std::string& filePath) {
            std::ifstream file(filePath);
            if (!file.is_open()) {
                std::cerr << "[ERROR] Could not open file: " << filePath << std::endl;
                return nullptr;
            }

            std::string magicNumber;
            uint32_t width, height, maxVal;
            file >> magicNumber >> width >> height >> maxVal;

            if (magicNumber != "P3") {
                std::cerr << "[ERROR] Format not supported. Use ASCII PPM (P3)." << std::endl;
                return nullptr;
            }

            FrameBuffer<RGBPixel>* buffer = new FrameBuffer<RGBPixel>(width, height);

            for (uint32_t i = 0; i < width * height; ++i) {
                int r, g, b;
                file >> r >> g >> b;
                buffer->data[i].r = static_cast<uint8_t>(r);
                buffer->data[i].g = static_cast<uint8_t>(g);
                buffer->data[i].b = static_cast<uint8_t>(b);
            }

            file.close();
            return buffer;
        }

        static bool savePGM(const std::string& filePath, const FrameBuffer<GrayPixel>& buffer) {
            std::ofstream file(filePath);
            if (!file.is_open()) return false;

            file << "P2\n" << buffer.width << " " << buffer.height << "\n255\n";

            for (uint32_t i = 0; i < buffer.width * buffer.height; ++i) {
                file << static_cast<int>(buffer.data[i].value) << " ";
                if ((i + 1) % buffer.width == 0) file << "\n";
            }

            file.close();
            return true;
        }
    };
}
#endif
