#include "util.h"

static Font* defaultFonts;

void SetDefaultFonts(Font *fonts)
{
    int fontCount = 0;
    while (fonts[fontCount++].recs != NULL);
    defaultFonts = (Font*)MemAlloc(sizeof(Font) * (fontCount + 1));
    for (int i=0;i<=fontCount;i++)
    {
        defaultFonts[i] = fonts[i];
    }
}

Font GetFont(int size)
{
    for (int i=1; defaultFonts[i].recs; i++)
    {
        int baseSize = defaultFonts[i].baseSize;
        if (size <= baseSize)
        {
            Font previousFont = defaultFonts[i - 1];
            int diff1 = size - previousFont.baseSize;
            int diff2 = baseSize - size;
            return (diff1 < diff2) ? previousFont : defaultFonts[i];
        }
    }
    return defaultFonts[0];
}

Rectangle DrawTextBoxAligned(const char *text, int fontSize, int x, int y, int w, int h, float alignX, float alignY, Color color)
{
    float fontSpacing = floor(fontSize * 0.025f);
    Font defaultFont = GetFont(fontSize);
    Vector2 textSize = MeasureTextEx(defaultFont, text, fontSize, fontSpacing);
    int posX = x + (int)((w - textSize.x) * alignX);
    int posY = y + (int)((h - textSize.y) * alignY);
    DrawTextEx(defaultFont, text, (Vector2){posX, posY}, fontSize, fontSpacing, color);

    return (Rectangle) {
        .x = posX, .y = posY, .width = textSize.x, .height = textSize.y
    };
}

Rectangle DrawStyledTextBox(StyledTextBox styledTextBox)
{
    char text[1024] = {0};
    int linesToDisplay = styledTextBox.displayedLineCount;
    for (int i=0;styledTextBox.text[i];i++)
    {
        text[i] = styledTextBox.text[i];
        if (text[i] == '\n')
        {
            linesToDisplay--;
            if (linesToDisplay == 0)
            {
                text[i] = '\0';
                break;
            }
        }
    }

    Rectangle rect = DrawTextBoxAligned(text, styledTextBox.fontSize,
        styledTextBox.box.x, styledTextBox.box.y,
        styledTextBox.box.width, styledTextBox.box.height, 
        styledTextBox.align.x, styledTextBox.align.y,
        styledTextBox.color);
    if (styledTextBox.underScoreSize > 0)
    {
        DrawRectangle(rect.x, rect.y + rect.height + styledTextBox.underScoreOffset, 
            rect.width, styledTextBox.underScoreSize, styledTextBox.color);
    }
    return rect;
}