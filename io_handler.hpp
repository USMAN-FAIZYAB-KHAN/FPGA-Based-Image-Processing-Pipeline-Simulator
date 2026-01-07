#ifndef IO_HANDLER_HPP
#define IO_HANDLER_HPP

#include "frame_buffer.hpp"
#include "pixel.hpp"
#include <fstream>
#include <string>
#include <iostream>

namespace hardware {

    struct IOHandler {
        
        // Loads a P3 (ASCII) PPM color image into a FrameBuffer
        static FrameBuffer<RGBPixel>* loadPPM(const std::string& filePath) {
            std::ifstream file(filePath); // Open file for reading
            if (!file.is_open()) {
                std::cerr << "[ERROR] Could not open file: " << filePath << std::endl;
                return nullptr;
            }

            std::string magicNumber; // Format identifier (e.g., P3)
            uint32_t width, height, maxVal; // Header metadata
            file >> magicNumber >> width >> height >> maxVal;

            // Ensure file is ASCII PPM format before proceeding
            if (magicNumber != "P3") {
                std::cerr << "[ERROR] Format not supported. Use ASCII PPM (P3)." << std::endl;
                return nullptr;
            }

            // Create new buffer for loaded pixel data
            FrameBuffer<RGBPixel>* buffer = new FrameBuffer<RGBPixel>(width, height);

            // Read RGB values and cast to 8-bit hardware pixels
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

        // Saves grayscale data to a P2 (ASCII) PGM file
        static bool savePGM(const std::string& filePath, const FrameBuffer<GrayPixel>& buffer) {
            std::ofstream file(filePath); // Open file for writing
            if (!file.is_open()) return false;

            // Write PGM header: Magic Number, Dimensions, and Max Brightness
            file << "P2\n" << buffer.width << " " << buffer.height << "\n255\n";

            // Write pixel values and insert newlines after each row of image
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
