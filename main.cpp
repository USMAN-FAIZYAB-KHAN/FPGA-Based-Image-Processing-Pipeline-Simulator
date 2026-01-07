#include <iostream>
#include <string>
#include "io_handler.hpp"
#include "color_converter.hpp"
#include "pipeline.hpp"

using namespace hardware;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <input.ppm> <output.pgm> <filter_mode>" << std::endl;
        std::cout << "Modes: none, blur, sharpen, sobel, both" << std::endl;
        return -1;
    }

    std::string inputPath  = argv[1];
    std::string outputPath = argv[2];
    std::string modeStr    = (argc > 3) ? argv[3] : "none";

    FrameBuffer<RGBPixel>* input = IOHandler::loadPPM(inputPath);
    if (!input) return -1;

    Pipeline pipe(input->width, input->height, 3);
    
    if (modeStr == "blur")           pipe.setMode(FilterType::BLUR);
    else if (modeStr == "sharpen")   pipe.setMode(FilterType::SHARPEN);
    else if (modeStr == "sobel")     pipe.setMode(FilterType::SOBEL);
    else if (modeStr == "both")      pipe.setMode(FilterType::BLUR_SOBEL);
    else                             pipe.setMode(FilterType::NONE);

    FrameBuffer<GrayPixel> output(input->width, input->height);
    uint32_t outIdx = 0;
    uint8_t outPixel;
    
    uint32_t totalCycles = (input->width * input->height) + (input->width * 5);
    
    #ifdef DEBUG
    std::cout << "[DEBUG] Starting Hardware Stream. Total Cycles: " << totalCycles << std::endl;
    #endif

    for (uint32_t i = 0; i < totalCycles; ++i) {
        GrayPixel gray(0); // Initialize with 0 for flushing phase
        
        if (i < input->width * input->height) {
            gray = ColorConverter::convert(input->data[i]);
        }

        // Now passing the GrayPixel 'gray' directly
        if (pipe.process(gray, outPixel)) {
            if (outIdx < input->width * input->height) {
                output.data[outIdx++].value = outPixel;
            }
        }
    }

    if (IOHandler::savePGM(outputPath, output)) {
        std::cout << "[SUCCESS] Applied mode: " << modeStr << " -> " << outputPath << std::endl;
    } else {
        std::cerr << "[ERROR] Failed to save output image." << std::endl;
    }

    delete input;
    return 0;
}
  
