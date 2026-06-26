// Only compile the real implementation on Arduino targets
#ifdef ARDUINO

#include "MatrixDisplay.h"

#include <Arduino.h>
#include "Config.h"

void MatrixDisplay::begin()
{
    matrix.begin();
}

void MatrixDisplay::printScroll(const char* message)
{
    matrix.clear();
    matrix.beginDraw();
    matrix.stroke(MATRIX_STROKE_COLOR);
    matrix.textScrollSpeed(MATRIX_TEXT_SCROLL_SPEED);
    matrix.textFont(MATRIX_TEXT_FONT);
    matrix.beginText(0, MATRIX_TEXT_Y, MATRIX_STROKE_COLOR);
    matrix.print(message);
    matrix.endText(SCROLL_LEFT);
    matrix.endDraw();
}

void MatrixDisplay::printStatic(const char* message)
{
    matrix.clear();
    matrix.beginDraw();
    matrix.stroke(0xFFFFFF);
    matrix.textFont(MATRIX_TEXT_FONT);
    int textWidth = strlen(message) * 6; // 5 px + 1 px spacing
    int x = (MATRIX_WIDTH - textWidth) / 2;
    if (x < 0) x = 0;
    matrix.text(message, x, MATRIX_TEXT_Y);
    matrix.endDraw();
}

#endif
