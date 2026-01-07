#ifndef LINE_BUFFER_HPP
#define LINE_BUFFER_HPP

#include <cstdint>
#include "pixel.hpp"

namespace hardware {
    struct LineBuffer {
        uint32_t width;
        uint32_t kernel_size;
        GrayPixel** data;
        uint32_t write_ptr;
        uint32_t rows_filled;

        LineBuffer(uint32_t w, uint32_t k) 
            : width(w), kernel_size(k), write_ptr(0), rows_filled(0) {
            
            data = new GrayPixel*[kernel_size - 1];
            for (uint32_t i = 0; i < kernel_size - 1; i++) {
                data[i] = new GrayPixel[width]{};
            }
        }

        ~LineBuffer() {
            for (uint32_t i = 0; i < kernel_size - 1; i++) delete[] data[i];
            delete[] data;
        }

        bool update(GrayPixel pixelIn, GrayPixel* column) {
            #ifdef DEBUG
            std::cout << "\n[DEBUG] --- LINE_BUFFER_TICK ---" << std::endl;
            std::cout << "[REG] write_ptr:   " << write_ptr << std::endl;
            std::cout << "[REG] rows_filled: " << rows_filled << std::endl;
            std::cout << "[IN]  pixelIn:     " << static_cast<int>(pixelIn.value) << std::endl;
            #endif

            column[kernel_size - 1] = pixelIn; 
            
            for (uint32_t i = 0; i < kernel_size - 1; i++) {
                column[i] = data[i][write_ptr];
            }

            #ifdef DEBUG
            std::cout << "[OUT] column:      [";
            for(uint32_t i = 0; i < kernel_size; ++i) {
                std::cout << static_cast<int>(column[i].value) << (i == kernel_size-1 ? "" : ", ");
            }
            std::cout << "]" << std::endl;
            #endif

            for (uint32_t i = 0; i < kernel_size - 2; i++) {
                data[i][write_ptr] = data[i + 1][write_ptr];
            }
            data[kernel_size - 2][write_ptr] = pixelIn;

            if (++write_ptr >= width) {
                write_ptr = 0;
                if (rows_filled < kernel_size) rows_filled++;
                
                #ifdef DEBUG
                std::cout << "[REG] EVENT: H-Sync (End of Line reached)" << std::endl;
                #endif
            }
            
            bool valid = (rows_filled >= kernel_size - 1);
            
            #ifdef DEBUG
            std::cout << "[OUT] valid_sig:   " << (valid ? "HIGH" : "LOW") << std::endl;
            #endif

            return valid;
        }
    };
}

#endif
