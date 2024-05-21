#include "pathfinding_tutorial.h"
#include "util.h"

#define MAP_WIDTH 10
#define MAP_HEIGHT 10
static const int mapCellSize = 20;

static Vector2 start = {1, 2};
static Vector2 end = {8, 6};

static char map[] = {
    1,0,0,0,0, 0,0,1,1,0,
    1,0,0,0,0, 0,0,0,1,0,
    1,0,1,1,0, 0,0,0,0,1,
    1,0,1,1,1, 0,0,0,1,1,
    1,0,1,1,1, 0,0,0,1,1,

    1,0,0,1,1, 0,0,1,1,1,
    0,0,0,1,0, 0,1,1,0,1,
    0,0,1,1,0, 0,1,1,0,1,
    0,0,0,0,0, 0,0,0,0,1,
    1,0,0,0,0, 0,0,0,0,1,
};

static float subStepTime = 0.0f;
static int subStep = 0;
static const char* bulletPoints = 
"- Let's say we have a map\n"
"- The map is a simple array describing a 10x10 grid\n"
"- Each 1 represents a wall, each 0 is an empty field we can travel\n"
"- We want to find a path from the red square to the blue square\n"
"- Beginning with the start node, we check the neigboring squares\n"
"   - ... one by one\n"
"   - ... on an empty cell, we repeat the same procedure\n"
"   - This search will go into depth first\n"
"   - Eventually, we find a connection to the end point\n"
"- This strategy is valid if we want to know if two points are connected\n"
"- But it isn't suitable for finding a short path, let alone the shortest\n"
;

static int CountLines()
{
    int lines = 1;
    for (int i=0;bulletPoints[i];i++)
    {
        if (bulletPoints[i] == '\n')
        {
            lines++;
        }
    }
    return lines;

}

void page_2_onInit(TutorialPage *page) {}
void page_2_onEnter(TutorialPage *page) 
{
    subStep = 0;
}
void page_2_onLeave(TutorialPage *page) {}
void page_2_onUpdate(TutorialPage *page, float dt) 
{
    subStepTime += dt;
}
static float StepTime(int targetStep, float duration)
{
    float t = subStepTime / duration;
    if (t > 1.0f)
    {
        t = 1.0f;
    }
    if (subStep == targetStep)
    {
        return t;
    }
    return subStep > targetStep ? 1.0f : 0.0f;
}

static float EaseInOut(float t)
{
    return t < 0.5f ? 2.0f * t * t : 1.0f - powf(-2.0f * t + 2.0f, 2.0f) / 2.0f;
}

static void StepDrawMapGrid(int offsetX, int offsetY, float animateGridState)
{
    animateGridState = EaseInOut(animateGridState);
    DrawRectangle(offsetX, offsetY, MAP_WIDTH * mapCellSize, MAP_HEIGHT * mapCellSize, LIGHTGRAY);
    for (int x = 0; x <= MAP_WIDTH; x++)
    {
        DrawRectangle(offsetX + x * mapCellSize, offsetY, 1, MAP_HEIGHT * mapCellSize * animateGridState, DARKGRAY);
    }
    for (int y = 0; y <= MAP_HEIGHT; y++)
    {
        DrawRectangle(offsetX, offsetY + y * mapCellSize, MAP_WIDTH * mapCellSize * animateGridState, 1, DARKGRAY);
    }
}

static void StepDrawMapContent(int offsetX, int offsetY, float animateGridState)
{
    animateGridState = EaseInOut(animateGridState);
    float cellSize = (mapCellSize + 1) * animateGridState;
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            int cell = map[y * MAP_WIDTH + x];
            if (cell == 1)
            {
                DrawRectangle(offsetX + x * mapCellSize, offsetY + y * mapCellSize, 
                    cellSize, cellSize, GRAY);
            }
        }
    }
}

static void StepDrawStartEndPoints(int offsetX, int offsetY, float animateGridState)
{
    animateGridState = EaseInOut(animateGridState);
    float pointSize = (mapCellSize - 5) * animateGridState;
    float offset = -pointSize * 0.5f + 1;
    DrawRectangle(offsetX + (start.x + 0.5f) * mapCellSize + offset, offsetY + (start.y + 0.5f) * mapCellSize + offset, 
        pointSize, pointSize, RED);
    DrawRectangle(offsetX + (end.x + 0.5f) * mapCellSize + offset, offsetY + (end.y + 0.5f) * mapCellSize + offset, 
        pointSize, pointSize, BLUE);
}

typedef struct Node
{
    int x, y;
    int fromX, fromY;
    int visitDepth;
    bool isValid;
    int counter;
} Node;
static Node visited[MAP_WIDTH * MAP_HEIGHT] = {0};
static int visitCounter = 0;
static int foundEndPointCounter = 0;
static void SearchPath(int atX, int atY, int fromX, int fromY, int depth)
{
    if (depth == 0)
    {
        visitCounter = 0;
        foundEndPointCounter = 0;
        for (int i=0;i<MAP_WIDTH * MAP_HEIGHT;i++)
        {
            visited[i].visitDepth = -1;
        }
    }

    if (atX < 0 || atY < 0 || atX >= MAP_WIDTH || atY >= MAP_HEIGHT || visited[atY * MAP_WIDTH + atX].visitDepth != -1)
    {
        return;
    }

    bool valid = map[atY * MAP_WIDTH + atX] == 0;
    visited[atY * MAP_WIDTH + atX] = (Node){
        .x = atX,
        .y = atY,
        .fromX = fromX,
        .fromY = fromY,
        .visitDepth = depth,
        .isValid = valid,
        .counter = visitCounter++
    };

    if (!valid) return;

    if (atX == floorf(end.x) && atY == floorf(end.y))
    {
        foundEndPointCounter = visitCounter;
        return;
    }

    SearchPath(atX - 1, atY, atX, atY, depth + 1);
    SearchPath(atX + 1, atY, atX, atY, depth + 1);
    SearchPath(atX, atY - 1, atX, atY, depth + 1);
    SearchPath(atX, atY + 1, atX, atY, depth + 1);
}

static void StepDrawVisited(int offsetX, int offsetY, float stepTime, int counterVisStart, int counterVisEnd)
{
    float progress = EaseInOut(stepTime);
    float counterProgress = progress * (counterVisEnd - counterVisStart) + counterVisStart;
    for (int i=0;i<MAP_WIDTH * MAP_HEIGHT;i++)
    {
        Node node = visited[i];
        float lineLength = counterProgress - node.counter;
        if (lineLength <= 0.0f) continue;
        if (lineLength > 1.0f) lineLength = 1.0f;
        Vector2 from = (Vector2){
            offsetX + (node.fromX + 0.5f) * mapCellSize,
            offsetY + (node.fromY + 0.5f) * mapCellSize
        };
        Vector2 to = (Vector2){
            offsetX + (node.x + 0.5f) * mapCellSize,
            offsetY + (node.y + 0.5f) * mapCellSize
        };
        DrawLineEx(from, Vector2Lerp(from, to, lineLength), 3, node.isValid ? BLACK : MAROON);
    }
}


void page_2_onDraw(TutorialPage *page, float dt)
{
    Rectangle titleRect = DrawStyledTextBox((StyledTextBox){
        .text = "Depth first search", 
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
        .text = bulletPoints, 
        .displayedLineCount = subStep + 1,
        .fontSize = 20, 
        .box.x = 20, 
        .box.y = 60, 
        .box.width = GetScreenWidth() - 40, 
        .box.height = GetScreenHeight() - 150, 
        .align.x = 0.0f, 
        .align.y = 0.0f, 
        .color = BLACK,
    });

    int offsetX = GetScreenWidth() - 80 - MAP_WIDTH * mapCellSize;
    int offsetY = 100;

    StepDrawMapGrid(offsetX, offsetY, StepTime(1, 1.0f));
    StepDrawMapContent(offsetX, offsetY, StepTime(2, 1.0f));
    StepDrawStartEndPoints(offsetX, offsetY, StepTime(3, 1.0f));

    SearchPath(floorf(start.x), floorf(start.y), floorf(start.x), floorf(start.y), 0);
    int fromCount = 0, toCount = 2;
    float countDrawProgress = StepTime(4, 1.0f);
    if (subStep == 5)
    {
        fromCount = 2;
        toCount = 4;
        countDrawProgress = StepTime(5, 1.0f);
    }
    if (subStep == 6)
    {
        fromCount = 4;
        toCount = 6;
        countDrawProgress = StepTime(6, 1.0f);
    }
    if (subStep == 7)
    {
        fromCount = 6;
        toCount = 20;
        countDrawProgress = StepTime(7, 4.0f);
    }
    if (subStep == 8)
    {
        fromCount = 20;
        toCount = foundEndPointCounter;
        countDrawProgress = StepTime(8, 4.0f);
    }
    if (subStep >= 9)
    {
        fromCount = 0;
        toCount = foundEndPointCounter;
        countDrawProgress = 1.0f;
    }
    StepDrawVisited(offsetX, offsetY, countDrawProgress, fromCount, toCount);
}

bool page_2_onNextSubstep(TutorialPage *page)
{
    subStepTime = 0.0f;
    return subStep++ < CountLines() - 2;
}
