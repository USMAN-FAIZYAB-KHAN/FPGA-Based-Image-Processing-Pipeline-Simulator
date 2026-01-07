# FPGA Based Image Processing Pipeline Simulator
This project is a high-level C++ simulation of a streaming hardware image processing pipeline. The design is architected to mimic the techniques used by industrial embedded vision teams (such as Xilinx, Intel, and NVIDIA) to accelerate image processing on FPGA and SoC systems.

Instead of processing an image as a single block in memory, this engine treats data as a continuous pixel stream. It uses hardware-centric logic to perform convolutions while minimizing memory bandwidth and power consumption.

## 📂 Project Directory Structure
```text
├── main.cpp              # Entry point
├── pixel.hpp             # Data structures for RGB and Grayscale pixels
├── frame_buffer.hpp      # Template class for image memory management
├── line_buffer.hpp       # Simulated BRAM row-caching logic
├── filters.hpp           # 3x3 Convolution kernels (Blur, Sharpen, Sobel)
├── pipeline.hpp          # Manages data flow between hardware stages
├── color_converter.hpp   # RGB to Grayscale logic (Fixed vs. Float)
├── io_handler.hpp        # PPM/PGM file read/write utilities
├── Makefile              # Build system with various simulation modes
└── README.md             # Project documentation
```
## 🔄 Project Data Flow
The system simulates a hardware streaming architecture where data moves through the following stages:

### 1. Input Stage
- __Load__: ```IOHandler``` reads an ASCII PPM image from disk.
- __Storage__: Data is placed into a ```FrameBuffer<RGBPixel>``` (simulating External RAM).

### 2. Processing Pipeline
The ```main``` function executes a loop where each iteration represents one hardware clock cycle:
- __Stream__: One RGB pixel is pulled from memory.
- __Convert__: ```ColorConverter``` transforms RGB to Grayscale (Float or Fixed-Point).
- __Buffer__: The pixel enters the ```LineBuffer```, which stores previous rows to create a vertical column.
- __Slide__: The column is pushed into a 3x3 Sliding Window.
- __Compute__: The active ```Filter``` (Blur, Sharpen, or Sobel) performs a matrix multiplication on the 3x3 window.
- __Cascade__: In "Both" mode, the output of the Blur filter is fed directly into the Sobel filter input.

### 3. Output Stage
- __Valid Check__: The pipeline returns a ```valid``` signal only when the buffers are full (latency).
- __Save__: ```IOHandler``` writes the final grayscale FrameBuffer to a ```.pgm``` file.

## ⚙️ Compilation & Usage

### Compilation Modes
```bash
# Standard compilation
make

# Compile with Debug prints
make debug

# Compile with Fixed-Point math enabled (FPGA simulation)
make fixed
```

### Running the Simulation
```bash
./mega_project <input.ppm> <output.pgm> <filter_mode>
```

### Filter Modes
1. ```none```: Pass-through grayscale.
2. ```blur```: Noise reduction.
3. ```sharpen```: Detail enhancement.
4. ```sobel```: Edge detection.
5. ```both```: Combined Blur + Sobel (Cascaded Pipeline).
