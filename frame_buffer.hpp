#ifndef FRAME_BUFFER_HPP
#define FRAME_BUFFER_HPP

#include "pixel.hpp"
#include <iostream>

namespace hardware {

template <typename T>
    struct FrameBuffer {
        int width;
        int height;
        T* data;

        FrameBuffer(int w, int h) : width(w), height(h) {
            data = new T[width * height];
            
            #ifdef DEBUG
                // typeid(T).name() returns the internal name of the type
                std::cout << "[DEBUG] BUFFER_ALLOC: " << width << "x" << height 
                          << " | Size: " << (sizeof(T) * width * height) << " Bytes" 
                          << std::endl;
            #endif
        }

        ~FrameBuffer() {
            if (data != nullptr) {
                delete[] data;
                #ifdef DEBUG
                    std::cout << "[DEBUG] BUFFER_FREE: Released" << std::endl;
                #endif
            }
        }

        T getPixel(uint32_t x, uint32_t y) const {
            return data[y * width + x];
        }

        void setPixel(uint32_t x, uint32_t y, T value) {
            data[y * width + x] = value;
        }
    };

}

#endif
