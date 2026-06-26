#pragma once

// Provide a mock implementation when building off-device (no Arduino)
#ifdef ARDUINO

#include <ArduinoGraphics.h>
#include <Arduino_LED_Matrix.h>

class MatrixDisplay {
public:
    void begin();
    void printScroll(const char* message);
    void printStatic(const char* message);

private:
    ArduinoLEDMatrix matrix;
};

#else

#include <cstdio>

class MatrixDisplay {
public:
    inline void begin() {}

    inline void printScroll(const char* message)
    {
        std::printf("[MATRIX SCROLL] %s\n", message);
    }

    inline void printStatic(const char* message)
    {
        std::printf("[MATRIX STATIC] %s\n", message);
    }
};

#endif
