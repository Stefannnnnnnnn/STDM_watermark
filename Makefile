# Compiler
CXX = g++
CXXFLAGS = -Iinclude -Wall -Wextra -std=c++11

# Source files
SRC = src/bitmap_image.cpp src/dct_watermark.cpp src/hdc_graphics.cpp main.cpp

# Output executable
TARGET = watermark_app

# Build target
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Clean up
clean:
	rm -f $(TARGET)