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
"- A different strategy is to do a \"Breadth-first\" search\n"
"- This time, we check all neighboring cells first\n"
"   - ... before we go deeper\n"
"   - To do that, we use a queuing system\n"
"   - The highlighted cells are currently queued\n"
"   - We continue queuing and dequeuing ...\n"
"   - until we reach the end point\n"
"- When we reach the end point, we can reconstruct the path\n"
"   - From the end point, we follow the \"from\" pointers\n"
"   - ... until we reach the start point\n"
"- For equal travel costs, the found path is also the shortest\n"
"- Since it is not a directed search, it is quite inefficient\n"
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

void page_3_onInit(TutorialPage *page) {}
void page_3_onEnter(TutorialPage *page) 
{
    subStep = 0;
}
void page_3_onLeave(TutorialPage *page) {}
void page_3_onUpdate(TutorialPage *page, float dt) 
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
static Node* queue[MAP_WIDTH * MAP_HEIGHT] = {0};
static int visitCounter = 0;
static int foundEndPointCounter = 0;
static int queueCount = 0;

static void Enqueue(int x, int y, int fromX, int fromY)
{
    if (x < 0 || y < 0 || x >= MAP_WIDTH || y >= MAP_HEIGHT || 
        visited[y * MAP_WIDTH + x].visitDepth != -1) return;
    
    bool isValid = map[y * MAP_WIDTH + x] == 0;
    
    visited[y * MAP_WIDTH + x] = (Node){
        .x = x,
        .y = y,
        .fromX = fromX,
        .fromY = fromY,
        .visitDepth = visited[fromY * MAP_WIDTH + fromX].visitDepth + 1,
        .isValid = isValid,
        .counter = visitCounter++
    };
    if (isValid)
    {
        queue[queueCount++] = &visited[y * MAP_WIDTH + x];
    }
}

static void SearchPath(int fromX, int fromY, int toX, int toY)
{
    queueCount = 0;
    visitCounter = 0;
    foundEndPointCounter = 0;
    for (int i=0;i<MAP_WIDTH * MAP_HEIGHT;i++)
    {
        visited[i].visitDepth = -1;
        queue[i] = NULL;
    }

    queue[queueCount++] = &visited[fromY * MAP_WIDTH + fromX];
    visited[fromY * MAP_WIDTH + fromX] = (Node) {
        .fromX = fromX,
        .fromY = fromY,
        .x = fromX,
        .y = fromY,
        .isValid = true,
        .visitDepth = 0,
        .counter = visitCounter++
    };

    while (queueCount > 0)
    {
        Node *node = queue[0];
        for (int i=1;i<queueCount;i++)
        {
            queue[i - 1] = queue[i];
        }
        queueCount--;
        int atX = node->x;
        int atY = node->y;
        bool valid = map[atY * MAP_WIDTH + atX] == 0;

        if (!valid) continue;

        if (atX == floorf(end.x) && atY == floorf(end.y))
        {
            foundEndPointCounter = node->visitDepth + 1;
        }

        Enqueue(atX - 1, atY, atX, atY);
        Enqueue(atX + 1, atY, atX, atY);
        Enqueue(atX, atY - 1, atX, atY);
        Enqueue(atX, atY + 1, atX, atY);
    }
}

static void StepDrawVisited(int offsetX, int offsetY, float stepTime, int depthStart, int depthEnd, int highlightQueued)
{
    float progress = EaseInOut(stepTime);
    float depthProgress = progress * (depthEnd - depthStart) + depthStart;
    for (int i=0;i<MAP_WIDTH * MAP_HEIGHT;i++)
    {
        Node node = visited[i];
        float lineLength = depthProgress - node.visitDepth;
        if (lineLength <= 0.0f) continue;
        if (lineLength > 1.0f) lineLength = 1.0f;
        Vector2 from = {
            offsetX + (node.fromX + 0.5f) * mapCellSize,
            offsetY + (node.fromY + 0.5f) * mapCellSize
        };
        Vector2 to = {
            offsetX + (node.x + 0.5f) * mapCellSize,
            offsetY + (node.y + 0.5f) * mapCellSize
        };
        DrawLineEx(from, Vector2Lerp(from, to, lineLength), 5, node.isValid ? BLACK : MAROON);
        if (highlightQueued && node.visitDepth == depthEnd - 1 && lineLength == 1.0f)
        {
            DrawCircleLines(offsetX + (node.x + 0.5f) * mapCellSize, offsetY + (node.y + 0.5f) * mapCellSize, 5, BLACK);
        }
    }
}

static void StepDrawBacktracking(int offsetX, int offsetY, float stepTime, int pathStart, int pathEnd)
{
    float progress = EaseInOut(stepTime);
    float depthProgress = progress * (pathEnd - pathStart) + pathStart;
    int x = floorf(end.x);
    int y = floorf(end.y);
    Node* node = &visited[y * MAP_WIDTH + x];
    int drawLength = 0;
    while (node->visitDepth > 0)
    {
        float lineLength = depthProgress - drawLength;
        drawLength += 1;
        if (lineLength <= 0.0f) break;
        if (lineLength > 1.0f) lineLength = 1.0f;
        
        Vector2 to = {
            offsetX + (node->fromX + 0.5f) * mapCellSize,
            offsetY + (node->fromY + 0.5f) * mapCellSize
        };
        Vector2 from = {
            offsetX + (node->x + 0.5f) * mapCellSize,
            offsetY + (node->y + 0.5f) * mapCellSize
        };
        DrawLineEx(from, Vector2Lerp(from, to, lineLength), 3, YELLOW);

        int newX = node->fromX;
        int newY = node->fromY;
        node = &visited[newY * MAP_WIDTH + newX];
    }
}


void page_3_onDraw(TutorialPage *page, float dt)
{
    Rectangle titleRect = DrawStyledTextBox((StyledTextBox){
        .text = "Breadth first search", 
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

    StepDrawMapGrid(offsetX, offsetY, 1.0f);
    StepDrawMapContent(offsetX, offsetY, 1.0f);
    StepDrawStartEndPoints(offsetX, offsetY, 1.0f);

    SearchPath(floorf(start.x), floorf(start.y), floorf(start.x), floorf(start.y));
    int fromDepth = 0, toDepth = 2;
    float countDrawProgress = StepTime(1, 1.0f);
    if (subStep == 2)
    {
        fromDepth = 2;
        toDepth = 3;
        countDrawProgress = StepTime(2, 1.0f);
    }
    if (subStep >= 3  && subStep < 5)
    {
        fromDepth = 3;
        toDepth = 4;
        countDrawProgress = StepTime(3, 1.0f);
    }
    if (subStep == 5)
    {
        fromDepth = 4;
        toDepth = 8;
        countDrawProgress = StepTime(5, 2.0f);
    }
    if (subStep >= 6)
    {
        fromDepth = 8;
        toDepth = foundEndPointCounter;
        countDrawProgress = StepTime(6, 4.0f);
    }
    StepDrawVisited(offsetX, offsetY, countDrawProgress, fromDepth, toDepth, subStep > 3);
    if (subStep == 7)
        StepDrawBacktracking(offsetX, offsetY, StepTime(7, 1), 0, 1);
    else if (subStep == 8)
        StepDrawBacktracking(offsetX, offsetY, StepTime(8, 2), 1, 8);
    else if (subStep > 8)
        StepDrawBacktracking(offsetX, offsetY, StepTime(9, 2), 8, foundEndPointCounter);
}

bool page_3_onNextSubstep(TutorialPage *page)
{
    subStepTime = 0.0f;
    TraceLog(LOG_INFO, "substep %d", subStep + 1);
    return subStep++ < CountLines() - 2;
}
