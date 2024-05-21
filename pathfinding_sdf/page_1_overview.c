#include "pathfinding_tutorial.h"
#include "util.h"

void page_1_onInit(TutorialPage *page) {}
void page_1_onEnter(TutorialPage *page) {}
void page_1_onLeave(TutorialPage *page) {}
void page_1_onUpdate(TutorialPage *page, float dt) {}

void page_1_onDraw(TutorialPage *page, float dt)
{
    Rectangle titleRect = DrawStyledTextBox((StyledTextBox){
        .text = "Overview", 
        .fontSize = 30, 
        .box.x = 20, 
        .box.y = 20, 
        .box.width = GetScreenWidth() - 40, 
        .box.height = 30, 
        .align.x = 0.0f, 
        .align.y = 0.5f, 
        .color = BLACK,
    });

    DrawRectangle(20, titleRect.y + titleRect.height, GetScreenWidth() - 40, 1, BLACK);

    DrawStyledTextBox((StyledTextBox){
        .text = "- Depth first search\n"
                "- Breadth first search\n"
                "- Dijkstra's algorithm\n"
                "- A* algorithm\n"
                "- Distance fields\n"
                "- Different agent sizes\n"
                "- A* with jumps\n"
                "- Path optimizations\n"
                "- Different agent path finding behaviors\n"
                "- Path finding for formations\n"
                "- Path finding for vehicles\n"
                ,
        .fontSize = 20, 
        .box.x = 20, 
        .box.y = 60, 
        .box.width = GetScreenWidth() - 40, 
        .box.height = GetScreenHeight() - 150, 
        .align.x = 0.0f, 
        .align.y = 0.0f, 
        .color = BLACK,
    });
}

bool page_1_onNextSubstep(TutorialPage *page)
{
    return false;
}
