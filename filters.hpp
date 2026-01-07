#ifndef FILTERS_HPP
#define FILTERS_HPP

#include "line_buffer.hpp"
#include "pixel.hpp"
#include <cmath>
#include <iostream>
#include <iomanip>

namespace hardware {

    // Base class for all filters
    struct BaseFilter {
        LineBuffer lb; // Line buffer to store previous rows for convolution
        GrayPixel** window; // The 3x3 local neighborhood of pixels
        GrayPixel* col; // Vertical slice received from the line buffer
        uint32_t kernel_size, kernel_radius; // Filter dimensions (e.g., 3 and 1)
        uint32_t width, height, x_cnt, y_cnt;

        // Allocates memory for the sliding window and vertical column
        BaseFilter(uint32_t w, uint32_t h, uint32_t k) 
            : lb(w, k), kernel_size(k), width(w), height(h), x_cnt(0), y_cnt(0) {
            kernel_radius = k / 2;
            window = new GrayPixel*[kernel_size];
            for (uint32_t i = 0; i < kernel_size; i++) {
                window[i] = new GrayPixel[kernel_size]{}; 
            }
            col = new GrayPixel[kernel_size]{};
        }

        // Cleanup allocated window memory
        virtual ~BaseFilter() {
            for (uint32_t i = 0; i < kernel_size; i++) delete[] window[i];
            delete[] window;
            delete[] col;
        }

        // Slides the 3x3 window to the right by one pixel
        void shift_window() {
            for (uint32_t i = 0; i < kernel_size; i++) {
                for (uint32_t j = 0; j < kernel_size - 1; j++) {
                    window[i][j] = window[i][j + 1]; // Move existing pixels left
                }
                window[i][kernel_size - 1] = col[i]; // Insert new column on the right
            }
        }
    };

    // Smoothens the image by averaging the 3x3 neighborhood
    struct BlurFilter : public BaseFilter {
        BlurFilter(uint32_t w, uint32_t h) : BaseFilter(w, h, 3) {}

        bool process(GrayPixel pixelIn, GrayPixel& pixelOut) {
            bool lb_valid = lb.update(pixelIn, col); // Update line buffer with new pixel
            shift_window(); // Update 3x3 window with the new column

            uint32_t sum = 0;
            for (uint32_t i = 0; i < 3; i++)
                for (uint32_t j = 0; j < 3; j++)
                    sum += window[i][j].value; // Sum all 9 pixels
            
            #ifdef FIXED_POINT
                uint32_t avg = (sum * 7281) >> 16; // Multiply by (1/9) in fixed-point
                pixelOut = GrayPixel(static_cast<uint8_t>(avg));
            #else
                pixelOut = GrayPixel(static_cast<uint8_t>(sum / 9)); // Standard division
            #endif

            if (++x_cnt >= width) { x_cnt = 0; y_cnt++; }
            return lb_valid && (y_cnt > kernel_radius); // Valid only when window is centered
        }
    };

    // Enhances edges by amplifying center-pixel contrast
    struct SharpenFilter : public BaseFilter {
        SharpenFilter(uint32_t w, uint32_t h) : BaseFilter(w, h, 3) {}

        bool process(GrayPixel pixelIn, GrayPixel& pixelOut) {
            bool lb_valid = lb.update(pixelIn, col);
            shift_window();

            int32_t result = 0;
            const int weights[3][3] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}}; // Sharpening kernel

            for (uint32_t i = 0; i < 3; i++) {
                for (uint32_t j = 0; j < 3; j++) {
                    result += (int32_t)window[i][j].value * weights[i][j];
                }
            }

            // Clamp results to 0-255 range to prevent overflow
            if (result > 255) result = 255;
            else if (result < 0) result = 0;
            pixelOut = GrayPixel(static_cast<uint8_t>(result));

            if (++x_cnt >= width) { x_cnt = 0; y_cnt++; }
            return lb_valid && (y_cnt > kernel_radius);
        }
    };

    // Detects edges using Gx and Gy gradient operators
    struct SobelFilter : public BaseFilter {
        SobelFilter(uint32_t w, uint32_t h) : BaseFilter(w, h, 3) {}

        bool process(GrayPixel pixelIn, GrayPixel& pixelOut) {
            bool lb_valid = lb.update(pixelIn, col);
            shift_window();

            int32_t gx = 0, gy = 0;
            const int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}}; // Horizontal edges
            const int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}}; // Vertical edges

            for (uint32_t i = 0; i < 3; i++) {
                for (uint32_t j = 0; j < 3; j++) {
                    gx += (int32_t)window[i][j].value * Gx[i][j];
                    gy += (int32_t)window[i][j].value * Gy[i][j];
                }
            }

            // Approximate gradient magnitude: |Gx| + |Gy|
            uint32_t mag = std::abs(gx) + std::abs(gy);
            pixelOut = GrayPixel((mag > 255) ? 255 : static_cast<uint8_t>(mag));

            if (++x_cnt >= width) { x_cnt = 0; y_cnt++; }
            return lb_valid && (y_cnt > kernel_radius);
        }
    };
}
#endif
