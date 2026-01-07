#ifndef FRAME_BUFFER_HPP
#define FRAME_BUFFER_HPP

#include "pixel.hpp"
#include <iostream>

namespace hardware {

// Generic buffer supporting both RGBPixel and GrayPixel types
template <typename T>
    struct FrameBuffer {
        int width; // Image width in pixels
        int height; // Image height in pixels
        T* data;

        // Allocates dynamic memory based on dimensions and pixel type
        FrameBuffer(int w, int h) : width(w), height(h) {
            data = new T[width * height];      
            #ifdef DEBUG
                std::cout << "[DEBUG] BUFFER_ALLOC: " << width << "x" << height 
                          << " | Size: " << (sizeof(T) * width * height) << " Bytes" 
                          << std::endl;
            #endif
        }

        // Destructor to ensure allocated memory is released
        ~FrameBuffer() {
            if (data != nullptr) {
                delete[] data;
                #ifdef DEBUG
                    std::cout << "[DEBUG] BUFFER_FREE: Released" << std::endl;
                #endif
            }
        }

        // Retrieves pixel using indexing: (y * width) + x
        T getPixel(uint32_t x, uint32_t y) const {
            return data[y * width + x];
        }

        void setPixel(uint32_t x, uint32_t y, T value) {
            data[y * width + x] = value;
        }
    };
}

#endif
