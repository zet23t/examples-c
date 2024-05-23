#include "util.h"
#include "resources.h"
#include "rlgl.h"

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

void DrawHouse(int x, int y)
{
    DrawTexturePro(resources.tileset, (Rectangle){80, 464, 48, 48}, (Rectangle){x * 2, y * 2, 96, 96}, (Vector2){0, 0}, 0, WHITE);
}

void DrawGuy(int x, int y)
{
    int animationTime = (int)(GetTime() * 2.0f) % 4;
    DrawTexturePro(resources.tileset, 
        (Rectangle){0, 464, 16, 16}, (Rectangle){x - 16, y - 28, 32, 32}, (Vector2){0, 0}, 0, WHITE);
    // face
    DrawTexturePro(resources.tileset, 
        (Rectangle){16 * animationTime, 448, 16, 16}, (Rectangle){x - 14, y - 48, 32, 32}, (Vector2){0, 0}, 0, WHITE);

}

void DrawBubble(int x, int y, int w, int h, float arrowAngle, int arrowX, int arrowY, Color color)
{
    const NPatchInfo ninePatch = {
        .source = {0, 480, 32, 32},
        .left = 12,
        .top = 12,
        .right = 12,
        .bottom = 12,
        .layout = NPATCH_NINE_PATCH
    };


    rlPushMatrix();
    rlScalef(2.0f, 2.0f, 1.0f);
    
    x /= 2;
    y /= 2;
    w /= 2;
    h /= 2;
   
    if (arrowAngle >= 0.0f)
        DrawTexturePro(resources.tileset, 
            (Rectangle){32, 496, 32, 16}, 
            (Rectangle){x + arrowX + 2, y + arrowY + 2, 32, 16}, (Vector2){2, 8}, arrowAngle, (Color){0, 0, 0, 128});
    DrawTextureNPatch(resources.tileset, ninePatch, (Rectangle){x + 2, y + 2, w, h}, (Vector2){0, 0}, 0, (Color){0, 0, 0, 128});
    
    
    if (arrowAngle >= 0.0f)
        DrawTexturePro(resources.tileset, 
            (Rectangle){32, 496, 32, 16}, 
            (Rectangle){x + arrowX + 2, y + arrowY + 2, 32, 16}, (Vector2){2, 8}, arrowAngle, color);
    DrawTextureNPatch(resources.tileset, ninePatch, (Rectangle){x, y, w, h}, (Vector2){0, 0}, 0, color);

    rlPopMatrix();
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