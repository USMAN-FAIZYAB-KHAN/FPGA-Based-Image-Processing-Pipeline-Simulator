#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "filters.hpp"
#include "pixel.hpp"
#include <cstdint>
#include <iostream>

namespace hardware {

    // Defines the available processing paths for the pixel stream
    enum class FilterType { NONE, BLUR, SHARPEN, SOBEL, BLUR_SOBEL };

    struct Pipeline {
        BlurFilter* blurStage;      // Hardware unit for smoothing
        SharpenFilter* sharpenStage; // Hardware unit for edge enhancement
        SobelFilter* sobelStage;    // Hardware unit for edge detection
        FilterType currentMode;      // current active filter mode

        // Initialize all hardware stages based on image dimensions
        Pipeline(uint32_t w, uint32_t h, int kSize) {
            blurStage = new BlurFilter(w, h);
            sharpenStage = new SharpenFilter(w, h);
            sobelStage = new SobelFilter(w, h);
            currentMode = FilterType::NONE;
        }

        ~Pipeline() {
            delete blurStage;
            delete sharpenStage;
            delete sobelStage;
        }

        void setMode(FilterType mode) {
            currentMode = mode;
        }

        // Streams a single pixel through the selected hardware logic.
        // pIn: Input pixel
        // pixelOut: Processed pixel result
        // Returns: true if the pipeline stage is ready to output data
        bool process(GrayPixel pIn, uint8_t& pixelOut) {
            GrayPixel pOut(0);
            GrayPixel temp(0);

            switch (currentMode) {
                case FilterType::BLUR:
                    if (blurStage->process(pIn, pOut)) {
                        pixelOut = pOut.value;
                        return true; // Valid blur output ready
                    }
                    return false;

                case FilterType::SHARPEN:
                    if (sharpenStage->process(pIn, pOut)) {
                        pixelOut = pOut.value;
                        return true; // Valid sharpen output ready
                    }
                    return false;

                case FilterType::SOBEL:
                    if (sobelStage->process(pIn, pOut)) {
                        pixelOut = pOut.value;
                        return true; // Valid sobel output ready
                    }
                    return false;

                case FilterType::BLUR_SOBEL:
                    // Multi-stage pipeline: Blur output feeds directly into Sobel input
                    if (blurStage->process(pIn, temp)) {
                        if (sobelStage->process(temp, pOut)) {
                            pixelOut = pOut.value;
                            return true; // Both stages full; output valid
                        }
                    }
                    return false;

                case FilterType::NONE:
                default:
                    pixelOut = pIn.value; // Bypass mode: data passes through unchanged
                    return true;
            }
        }
    };
}
#endif
