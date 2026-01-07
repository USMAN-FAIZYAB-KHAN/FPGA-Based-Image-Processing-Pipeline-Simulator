#ifndef LINE_BUFFER_HPP
#define LINE_BUFFER_HPP

#include <cstdint>
#include "pixel.hpp"

namespace hardware {
    struct LineBuffer {
        uint32_t width;        // Number of pixels per horizontal line
        uint32_t kernel_size;
        GrayPixel** data;      // Internal memory simulating hardware BRAM lines
        uint32_t write_ptr;    // Current horizontal position
        uint32_t rows_filled;  // Track startup latency until buffer is ready

        // Initializes buffer memory to store (K-1) rows
        LineBuffer(uint32_t w, uint32_t k) 
            : width(w), kernel_size(k), write_ptr(0), rows_filled(0) {  
            data = new GrayPixel*[kernel_size - 1];
            for (uint32_t i = 0; i < kernel_size - 1; i++) {
                data[i] = new GrayPixel[width]{}; // Allocate individual line memories
            }
        }

        // Frees allocated row memory
        ~LineBuffer() {
            for (uint32_t i = 0; i < kernel_size - 1; i++) delete[] data[i];
            delete[] data;
        }

        
        // Simulates one clock cycle of a sliding window line buffer.
        // pixelIn: New pixel arriving from the external memory not line buffer
        // column: Output array containing a column of pixels
        bool update(GrayPixel pixelIn, GrayPixel* column) {
            #ifdef DEBUG
            std::cout << "\n[DEBUG] --- LINE_BUFFER_TICK ---" << std::endl;
            #endif

            // Newest pixel always goes to the bottom of the column
            column[kernel_size - 1] = pixelIn; 
            
            // Read "old" pixels from BRAM to fill the rest of the vertical column
            for (uint32_t i = 0; i < kernel_size - 1; i++) {
                column[i] = data[i][write_ptr];
            }

            // Shift internal BRAM data: Move pixel from row N+1 to row N
            for (uint32_t i = 0; i < kernel_size - 2; i++) {
                data[i][write_ptr] = data[i + 1][write_ptr];
            }
            // Update the last BRAM row with the newest incoming pixel
            data[kernel_size - 2][write_ptr] = pixelIn;

            // Increment write pointer and handle line wraparound
            if (++write_ptr >= width) {
                write_ptr = 0;
                if (rows_filled < kernel_size) rows_filled++;
            }
            
            bool valid = (rows_filled >= kernel_size - 1);            
            return valid; // Returns HIGH when a full vertical column is ready
        }
    };
}

#endif
