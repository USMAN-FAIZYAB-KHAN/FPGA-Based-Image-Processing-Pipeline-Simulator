#include <iostream>
#include <string>
#include "io_handler.hpp"
#include "color_converter.hpp"
#include "pipeline.hpp"

using namespace hardware;

int main(int argc, char* argv[]) {
    // Check if the user provided the necessary file paths and filter choice
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <input.ppm> <output.pgm> <filter_mode>" << std::endl;
        std::cout << "Modes: none, blur, sharpen, sobel, both" << std::endl;
        return -1;
    }

    std::string inputPath  = argv[1];
    std::string outputPath = argv[2];
    std::string modeStr    = (argc > 3) ? argv[3] : "none";

    // Load the colored PPM image into our input buffer
    FrameBuffer<RGBPixel>* input = IOHandler::loadPPM(inputPath);
    if (!input) return -1;

    // Set up the hardware pipeline with image dimensions and a 3x3 kernel size
    Pipeline pipe(input->width, input->height, 3);
    
    if (modeStr == "blur")           pipe.setMode(FilterType::BLUR);
    else if (modeStr == "sharpen")   pipe.setMode(FilterType::SHARPEN);
    else if (modeStr == "sobel")     pipe.setMode(FilterType::SOBEL);
    else if (modeStr == "both")      pipe.setMode(FilterType::BLUR_SOBEL);
    else                             pipe.setMode(FilterType::NONE);

    // Grayscale buffer to store our final processed image
    FrameBuffer<GrayPixel> output(input->width, input->height);
    uint32_t outIdx = 0; // Tracks which pixel we are currently saving in the output
    uint8_t outPixel;    // Container for the single pixel result from the pipeline
    
    // Calculate extra clock cycles needed to "flush" the last pixels out of the buffer
    uint32_t totalCycles = (input->width * input->height) + (input->width * 5);
    
    #ifdef DEBUG
    std::cout << "[DEBUG] Starting Hardware Stream. Total Cycles: " << totalCycles << std::endl;
    #endif

    // Each iteration represents one hardware clock cycle
    for (uint32_t i = 0; i < totalCycles; ++i) {
        GrayPixel gray(0);
        
        // As long as we have input data, convert the RGB color to Grayscale
        if (i < input->width * input->height) {
            gray = ColorConverter::convert(input->data[i]);
        }

        // Push the pixel into the pipeline
        if (pipe.process(gray, outPixel)) {
            // If the pipeline has a valid result, save it into our output buffer
            if (outIdx < input->width * input->height) {
                output.data[outIdx++].value = outPixel;
            }
        }
    }

    // Save the final result to a grayscale PGM file
    if (IOHandler::savePGM(outputPath, output)) {
        std::cout << "[SUCCESS] Applied mode: " << modeStr << " -> " << outputPath << std::endl;
    } else {
        std::cerr << "[ERROR] Failed to save output image." << std::endl;
    }

    delete input;
    return 0;
}
