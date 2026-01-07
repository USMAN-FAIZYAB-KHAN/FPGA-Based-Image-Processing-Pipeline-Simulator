#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "filters.hpp"
#include "pixel.hpp"
#include <cstdint>
#include <iostream>

namespace hardware {

    enum class FilterType { NONE, BLUR, SHARPEN, SOBEL, BLUR_SOBEL };

    struct Pipeline {
        BlurFilter* blurStage;
        SharpenFilter* sharpenStage;
        SobelFilter* sobelStage;
        FilterType currentMode;

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

        bool process(GrayPixel pIn, uint8_t& pixelOut) {
            GrayPixel pOut(0);
            GrayPixel temp(0);

            switch (currentMode) {
                case FilterType::BLUR:
                    if (blurStage->process(pIn, pOut)) {
                        pixelOut = pOut.value;
                        return true;
                    }
                    return false;

                case FilterType::SHARPEN:
                    if (sharpenStage->process(pIn, pOut)) {
                        pixelOut = pOut.value;
                        return true;
                    }
                    return false;

                case FilterType::SOBEL:
                    if (sobelStage->process(pIn, pOut)) {
                        pixelOut = pOut.value;
                        return true;
                    }
                    return false;

                case FilterType::BLUR_SOBEL:
                    if (blurStage->process(pIn, temp)) {
                        if (sobelStage->process(temp, pOut)) {
                            pixelOut = pOut.value;
                            return true;
                        }
                    }
                    return false;

                case FilterType::NONE:
                default:
                    pixelOut = pIn.value;
                    return true;
            }
        }
    };
}
#endif
