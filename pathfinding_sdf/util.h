#ifndef __UTIL_H__
#define __UTIL_H__

#include "raylib.h"
#include "stddef.h"
#include <math.h> // Required for: floor, abs

typedef struct StyledTextBox
{
    const char *text;
    int displayedLineCount;
    int fontSize;
    Rectangle box;
    Vector2 align;
    Color color;
    int underScoreOffset;
    int underScoreSize;
} StyledTextBox;

void SetDefaultFonts(Font *fonts);
Rectangle DrawTextBoxAligned(const char *text, int fontSize, int x, int y, int w, int h, float alignX, float alignY, Color color);
Rectangle DrawStyledTextBox(StyledTextBox styledTextBox);

#endif