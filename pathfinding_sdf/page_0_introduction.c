#include "pathfinding_tutorial.h"
#include "util.h"
#include "tilemap.h"

Tilemap tilemap = {0};
unsigned char tiles[] = {
0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
0, 0, 0, 0,  1, 1, 1, 1,  1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
0, 0, 0, 2,  2, 2, 2, 1,  1, 1, 1, 1,  1, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
0, 0, 2, 2,  2, 2, 2, 2,  2, 2, 2, 2,  2, 1, 1, 1,  1, 1, 1, 1,  1, 0, 0, 0,  0, 0, 0, 0,

0, 0, 0, 0,  2, 2, 1, 2,  3, 3, 3, 2,  2, 2, 1, 1,  1, 1, 1, 1,  1, 1, 1, 0,  0, 0, 0, 0,
0, 0, 2, 2,  1, 1, 1, 2,  2, 2, 3, 3,  2, 2, 2, 1,  1, 2, 3, 2,  2, 1, 1, 1,  0, 0, 0, 0,
0, 0, 2, 2,  1, 0, 1, 2,  2, 3, 3, 3,  2, 2, 2, 1,  2, 3, 3, 3,  2, 1, 1, 1,  0, 0, 0, 0,
0, 0, 3, 3,  1, 1, 1, 2,  2, 2, 2, 2,  2, 2, 1, 1,  2, 2, 3, 3,  3, 3, 3, 1,  1, 0, 0, 0,

0, 0, 3, 3,  2, 2, 1, 0,  2, 2, 2, 2,  3, 3, 2, 2,  2, 1, 1, 3,  3, 3, 3, 1,  0, 0, 0, 0,
0, 1, 2, 2,  2, 2, 2, 2,  2, 3, 3, 3,  3, 3, 3, 2,  2, 2, 0, 2,  1, 1, 1, 1,  0, 0, 0, 0,
0, 1, 2, 2,  2, 2, 2, 2,  2, 2, 3, 3,  2, 2, 2, 2,  2, 2, 0, 0,  0, 2, 2, 0,  0, 0, 0, 0,
0, 1, 1, 1,  2, 2, 2, 2,  2, 2, 3, 3,  2, 2, 2, 2,  2, 2, 1, 1,  0, 2, 2, 0,  0, 0, 0, 0,

0, 0, 0, 1,  2, 2, 2, 2,  1, 2, 2, 2,  2, 2, 2, 0,  1, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
0, 0, 0, 1,  1, 1, 2, 2,  1, 1, 1, 1,  1, 1, 1, 0,  3, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
0, 0, 0, 0,  0, 1, 1, 0,  0, 0, 1, 1,  0, 0, 0, 3,  3, 3, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 3,  3, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
};

void page_0_onInit(TutorialPage *page) {
    tilemap.tiles = tiles;
    tilemap.width = 28;
    tilemap.height = 16;
    tilemap.tileWidth = 16;
    tilemap.tileHeight = 16;
}
void page_0_onEnter(TutorialPage *page) {}
void page_0_onLeave(TutorialPage *page) {}
void page_0_onUpdate(TutorialPage *page, float dt) {}

void page_0_onDraw(TutorialPage *page, float dt)
{
    Tilemap_draw(&tilemap, (Vector2){0, 0}, (Vector2){2, 2}, WHITE);
    int centerX = GetScreenWidth() / 2;
    int centerY = GetScreenHeight() / 2 + 20;
    int rectH = 100;
    DrawRectangleRec((Rectangle){0, centerY - rectH / 2, GetScreenWidth(), rectH}, (Color){180, 200, 220, 200});
    DrawStyledTextBox((StyledTextBox){
        .text = "Pathfinding tutorial", 
        .fontSize = 30, 
        .box.x = 0, 
        .box.y = 0, 
        .box.width = GetScreenWidth(), 
        .box.height = GetScreenHeight(), 
        .align.x = 0.5f, 
        .align.y = 0.5f, 
        .color = BLACK,
        .underScoreSize = 3
    });

    DrawStyledTextBox((StyledTextBox){
        .text = "Pathfinding tips & tricks and how to apply them", 
        .fontSize = 20, 
        .box.x = 0, 
        .box.y = 40, 
        .box.width = GetScreenWidth(), 
        .box.height = GetScreenHeight(), 
        .align.x = 0.5f, 
        .align.y = 0.5f, 
        .color = BLACK,
    });
}

bool page_0_onNextSubstep(TutorialPage *page)
{
    return false;
}
