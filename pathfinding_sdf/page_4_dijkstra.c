#include "pathfinding_tutorial.h"
#include "util.h"

#define MAP_WIDTH 10
#define MAP_HEIGHT 10
static const int mapCellSize = 20;

static Vector2 start = {1, 2};
static Vector2 end = {8, 6};

static unsigned char map[] = {
    0,1,1,1,1, 1,4,0,0,1,
    0,1,1,3,3, 1,4,4,0,1,
    0,1,0,0,3, 1,1,3,3,0,
    0,1,0,0,0, 1,1,1,0,0,
    0,3,0,0,0, 3,1,1,0,0,

    0,4,2,0,0, 3,3,0,0,0,
    4,5,2,0,1, 3,0,0,1,0,
    3,4,0,0,1, 1,0,0,1,0,
    3,4,4,1,1, 1,3,3,1,0,
    0,4,1,1,1, 1,2,2,1,0,
};

static float subStepTime = 0.0f;
static int subStep = 0;
static const char* bulletPoints = 
"- When the costs per cell are not constant, we need a different strategy:\n"
"   - Each cell has now travel costs\n"
"   - When queuing adjacent tiles, we accumulate the travel costs\n"
"   - Instead of dequeuing the first element, ...\n"
"   - ... we dequeue the element with the lowest costs\n"
"   - When we hit the destination, ...\n"
"   - ... we found the cheapest path\n"
"- Dijkstra is a greedy algorithm, \n"
"    - but it is not directed\n"
"    - it is there not much different from the broad search\n"
"    - It will visit each location only once\n"
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

void page_4_onInit(TutorialPage *page) {}
void page_4_onEnter(TutorialPage *page) 
{
    subStep = 0;
}
void page_4_onLeave(TutorialPage *page) {}
void page_4_onUpdate(TutorialPage *page, float dt) 
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
    float cellSize = (mapCellSize - 1) * animateGridState;
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            int cell = map[y * MAP_WIDTH + x];
            if (cell == 0)
            {
                DrawRectangle(offsetX + x * mapCellSize, offsetY + y * mapCellSize, 
                    mapCellSize + 1, mapCellSize + 1, GRAY);
            }
            else
            {
                float costs = (float) cell * 0.5f;
                DrawRectangle(
                    offsetX + (x + 0.5f) * mapCellSize + 1 - cellSize * 0.5f, 
                    offsetY + (y + 0.5f) * mapCellSize + 1 - cellSize * 0.5f, 
                    cellSize, cellSize, 
                    (Color){128 + costs * 40, 150, 200 - costs * 50, 255});
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
    int accumulatedCosts;
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
    bool isValid = map[y * MAP_WIDTH + x] > 0;
    Node *from = &visited[fromY * MAP_WIDTH + fromX];
    visited[y * MAP_WIDTH + x] = (Node){
        .accumulatedCosts = from->accumulatedCosts + map[y * MAP_WIDTH + x],
        .x = x,
        .y = y,
        .fromX = fromX,
        .fromY = fromY,
        .visitDepth = from->visitDepth + 1,
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
        .accumulatedCosts = 0,
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
        int lowestQueueIndex = 0;
        for (int i=1;i<queueCount;i++)
        {
            if (queue[i]->accumulatedCosts < node->accumulatedCosts)
            {
                node = queue[i];
                lowestQueueIndex = i;
            }
        }
        for (int i=lowestQueueIndex;i<queueCount - 1;i++)
        {
            queue[i] = queue[i + 1];
        }
        queueCount--;
        int atX = node->x;
        int atY = node->y;
        bool valid = map[atY * MAP_WIDTH + atX] > 0;

        if (!valid) continue;

        if (atX == floorf(end.x) && atY == floorf(end.y))
        {
            foundEndPointCounter = visitCounter;
        }

        Enqueue(atX - 1, atY, atX, atY);
        Enqueue(atX + 1, atY, atX, atY);
        Enqueue(atX, atY - 1, atX, atY);
        Enqueue(atX, atY + 1, atX, atY);
    }
}

static void StepDrawVisited(int offsetX, int offsetY, float stepTime, int visitCountStart, int visitCountEnd, int highlightQueued)
{
    float progress = EaseInOut(stepTime);
    float visitCountProgress = progress * (visitCountEnd - visitCountStart) + visitCountStart;
    for (int i=0;i<MAP_WIDTH * MAP_HEIGHT;i++)
    {
        Node node = visited[i];
        float lineLength = visitCountProgress - node.counter;
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
        if (highlightQueued && node.visitDepth == visitCountEnd - 1 && lineLength == 1.0f)
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


void page_4_onDraw(TutorialPage *page, float dt)
{
    Rectangle titleRect = DrawStyledTextBox((StyledTextBox){
        .text = "Dijkstra's algorithm", 
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
    StepDrawMapContent(offsetX, offsetY, StepTime(1, 1.0f));
    StepDrawStartEndPoints(offsetX, offsetY, 1.0f);

    SearchPath(floorf(start.x), floorf(start.y), floorf(start.x), floorf(start.y));
    int fromDepth = 0, toDepth = 4;
    float countDrawProgress = StepTime(2, 1.0f);
    if (subStep == 3)
    {
        fromDepth = 4;
        toDepth = 16;
        countDrawProgress = StepTime(3, 1.0f);
    }
    if (subStep >= 4  && subStep < 5)
    {
        fromDepth = 16;
        toDepth = 50;
        countDrawProgress = StepTime(4, 1.0f);
    }
    if (subStep >= 5)
    {
        fromDepth = 50;
        toDepth = foundEndPointCounter;
        countDrawProgress = StepTime(5, 2.0f);
    }
    StepDrawVisited(offsetX, offsetY, countDrawProgress, fromDepth, toDepth, subStep > 3);
    if (subStep >= 6)
        StepDrawBacktracking(offsetX, offsetY, StepTime(6, 1), 0, 19);
}

bool page_4_onNextSubstep(TutorialPage *page)
{
    subStepTime = 0.0f;
    TraceLog(LOG_INFO, "substep %d", subStep + 1);
    return subStep++ < CountLines() - 2;
}
