CXX = g++
TARGET = mega_project
SRC = main.cpp

HEADERS = pixel.hpp frame_buffer.hpp line_buffer.hpp filters.hpp pipeline.hpp io_handler.hpp color_converter.hpp

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CXX) $(SRC) -o $(TARGET)

debug:
	$(CXX) -DDEBUG $(SRC) -o $(TARGET)

fixed:
	$(CXX) -DFIXED_POINT $(SRC) -o $(TARGET)

hardware_mode:
	$(CXX) -DDEBUG -DFIXED_POINT $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
