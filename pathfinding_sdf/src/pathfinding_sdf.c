/**********************************************************************************************
*
*   raylib-extras, examples-c * examples for Raylib in C
*
*   pathfinding in combination with signed distance fields
*
*   LICENSE: ZLib
*
*   Copyright (c) 2024 Eike Decker
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy
*   of this software and associated documentation files (the "Software"), to deal
*   in the Software without restriction, including without limitation the rights
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*   copies of the Software, and to permit persons to whom the Software is
*   furnished to do so, subject to the following conditions:
*
*   The above copyright notice and this permission notice shall be included in all
*   copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*   SOFTWARE.
*
**********************************************************************************************
*
*   This example demonstrates how to use signed distance fields (SDF) for improving
*   pathfinding.
*
*   1) Unit size: A path may have a requirement for a minimum width to allow passage.
*      By using the SDF values, we can block paths that are too narrow for the unit.
*
*   2) Path preferences: A unit may prefer to stay close to walls or avoid them.
*      The example demonstrates how to influence the pathfinding by using the SDF values.
*
*   3) Varying step distances: Using SDF values to adjust step distances during path 
*      finding, resulting in curved paths.
*
*  TODO: optimize the path after it was found: nodes could be removed to smooth the path
*        this could again be done using the SDF values to determine if a node can be removed
*        without clipping through walls
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include "pathfinding_tutorial.h"
#include "resources.h"

#include <stddef.h> // Required for: NULL
#include <math.h> // Required for: abs

typedef struct PathfindingNode
{
    int x, y;
    int fromX;
    int fromY;
    int score;
} PathfindingNode;

typedef struct NeighborOffset
{
    int x, y;
    int distance;
} NeighborOffset;

typedef struct Agent
{
    int startX, startY;
    int targetX, targetY;
    int wallFactor;
    int unitSize;
    PathfindingNode *path;
    int pathCount;
    PathfindingNode *map;
    Vector2* icon;
    Color color;
    int iconCount;
    float walkedPathDistance;
} Agent;

typedef struct AppState
{
    int visualizeMode;
    int randomizeBlocks;
    char paintMode;
    int updateSDF;
    int sdfFunction;
    int jumpingEnabled;
    int cellX, cellY;
    Agent rat;
    Agent cat;
} AppState;

// a simple cat face that can be drawn as a triangle fan
Vector2 catFace[] = {
    {0.0f, 1.0f},
    {0.7f, 1.0f},
    {1.0f, 0.7f},
    {1.0f, -1.0f},
    {0.5f, -0.6f},
    {-0.5f, -0.6f},
    {-1.0f, -1.0f},
    {-1.0f, 0.7f},
    {-0.7f, 1.0f},
};

// a simple rat face that can be drawn as a triangle fan
Vector2 ratFace[] = {
    {0.0f, 1.0f},
    {0.3f, 0.9f},
    {0.8f, -0.2f},
    {1.0f, -0.8f},
    {0.8f, -1.0f},
    {0.5f, -1.0f},
    {0.3f, -0.7f},
    {0.0f, -0.8f},
    {-0.3f, -0.7f},
    {-0.5f, -1.0f},
    {-0.8f, -1.0f},
    {-1.0f, -0.8f},
    {-0.8f, -0.2f},
    {-0.3f, 0.9f},
};


const Color gridColor = { 200, 200, 200, 40 };
const Color cellHighlightColor = { 200, 0, 0, 80 };
const float movementSpeed = 3.0f;

const int gridWidth = 80;
const int gridHeight = 45;
const int cellSize = 10;

char* blockedCells = NULL;
char* sdfCells = NULL;

// lookup table for cheap square root calculation
int isqrt[256] = {0};

// various offsets and distances for jumping nodes during pathfinding
NeighborOffset neighborOffsets[20*20] = {0};
int neighborOffsetCount = 0;

Agent Agent_init(int x, int y, int size, int targetX, int targetY, int wallFactor, Vector2* icon, int iconCount, Color color)
{
    Agent agent;
    agent.unitSize = size;
    agent.startX = x;
    agent.startY = y;
    agent.targetX = targetX;
    agent.targetY = targetY;
    agent.pathCount = 0;
    agent.wallFactor = wallFactor;
    agent.path = (PathfindingNode *)MemAlloc(gridWidth * gridHeight * sizeof(PathfindingNode) * 4);
    agent.map = (PathfindingNode *)MemAlloc(gridWidth * gridHeight * sizeof(PathfindingNode));
    agent.icon = icon;
    agent.color = color;
    agent.iconCount = iconCount;
    agent.walkedPathDistance = 0.0f;
    return agent;

}

void Agent_findPath(Agent *agent, int enableJumping)
{
    // the queue should in theory not be longer than the number of map cells
    PathfindingNode *queue = (PathfindingNode *)MemAlloc(gridWidth * gridHeight * sizeof(PathfindingNode));
    PathfindingNode *map = agent->map;
    PathfindingNode *path = agent->path;
    int unitSize = agent->unitSize;
    int sdfFactor = agent->wallFactor;
    // we swap the start and end points to get the path in the right order without reversing it
    // so it searches from the target to the start and not the other way round, but in this case,
    // this doesn't matter
    int toX = agent->startX;
    int toY = agent->startY;
    int startX = agent->targetX;
    int startY = agent->targetY;
    for (int i = 0; i < gridWidth * gridHeight; i++)
    {
        map[i].score = 0;
    }

    // initialize queue and map with start position data
    int queueLength = 1;
    queue[0].fromX = startX;
    queue[0].fromY = startY;
    queue[0].x = startX;
    queue[0].y = startY;
    queue[0].score = 1;
    map[startY * gridWidth + startX].fromX = -1;
    map[startY * gridWidth + startX].fromY = -1;
    map[startY * gridWidth + startX].x = startX;
    map[startY * gridWidth + startX].y = startY;
    map[startY * gridWidth + startX].score = 1;

    while (queueLength > 0)
    {
        // find and dequeue node with lowest score
        int lowestScoreIndex = 0;
        for (int i=1;i<queueLength;i++)
        {
            if (queue[i].score < queue[lowestScoreIndex].score)
            {
                lowestScoreIndex = i;
            }
        }
        PathfindingNode node = queue[lowestScoreIndex];
        for (int i=lowestScoreIndex+1;i<queueLength;i++)
        {
            queue[i-1] = queue[i];
        }
        queueLength--;

        // we can determine how far we can safely jump away from this cell by 
        // taking the SDF value of the current cell. If our unit size is 2 and 
        // the SDF value is 5, we can safely jump 3 cells away from this cell, knowing
        // that we can't clip through walls at this distance.
        int cellSdf = sdfCells[node.y * gridWidth + node.x];
        int maxDistance = cellSdf - agent->unitSize;
        if (maxDistance < 1)
        {
            maxDistance = 1;
        }

        // The neighbor offsets are used to check various directions of different distances
        for (int i=0; i<neighborOffsetCount; i++)
        {
            // step distance is the distance the offset is away from the current cell
            // if it exceeds the max distance, we skip this offset
            int stepDistance = neighborOffsets[i].distance;
            if (stepDistance > maxDistance || (!enableJumping && stepDistance > 1))
            {
                continue;
            }

            // rejecting first cells that are outside the map
            int x = node.x + neighborOffsets[i].x;
            int y = node.y + neighborOffsets[i].y;
            if (x < 0 || x >= gridWidth || y < 0 || y >= gridHeight)
            {
                continue;
            }

            // nextSdf is the SDF value of the next cell where we would land
            int nextSdf = sdfCells[y * gridWidth + x];

            // skip if the next cell is closer to a wall than the unit size (wall clipping)
            if (nextSdf < unitSize)
            {
                continue;
            }

            // calculate the score of the next cell
            int score = node.score + stepDistance;
            int sdfValue = sdfCells[y * gridWidth + x];
            // assuming a linear interpolation between the SDF values of the current and next cell,
            // we can estimate the integral of the SDF values between the two cells - this is
            // only a rough approximation and since it's integers, we cheat a bit to favor longer jumps
            int integratedSdfValue = (sdfValue + cellSdf) * (stepDistance + 1) / 2;
            score = score + integratedSdfValue * sdfFactor / 6;

            // if the cell is not yet visited or the score is lower than the previous score, 
            // we update the cell and queue the cell for evaluation (one optimization would be to
            // not queue the cell if it is already queued, but this complexity is omitted here)
            if (map[y * gridWidth + x].score == 0 || score < map[y * gridWidth + x].score)
            {
                map[y * gridWidth + x] = (PathfindingNode){
                    .fromX = node.x,
                    .fromY = node.y,
                    .x = x,
                    .y = y,
                    .score = score
                };
                // queue the cell for evaluation
                queue[queueLength] = map[y * gridWidth + x];
                queueLength++;

                // prevent queue overflow - should not happen with the chosen queue lengths, but
                // could (maybe) still happen for extreme worst case scenarios
                if (queueLength >= gridWidth * gridHeight)
                {
                    TraceLog(LOG_ERROR, "queue overflow\n");
                    queueLength = gridWidth * gridHeight - 1;
                }
            }
        }
    }

    if (map[toY * gridWidth + toX].score > 0)
    {
        // path found
        int x = toX;
        int y = toY;
        int length = 0;
        // reconstruct path by following the from pointers to previous cells - the list is reversed
        // but we handle this with swapping the start / end points
        while (map[y * gridWidth + x].score > 0 && (x != startX || y != startY) && length < gridWidth * gridHeight)
        {
            path[length] = map[y * gridWidth + x];
            x = path[length].fromX;
            y = path[length].fromY;
            length++;
        }
        path[length++] = map[startY * gridWidth + startX];
        
        agent->pathCount = length;
    }
    else
    {
        // no path found
        agent->pathCount = 0;
    }

    MemFree(queue);
}

void Agent_drawPath(Agent *agent)
{
    int rectSize = agent->unitSize * 2;
    int rectOffset = (cellSize - rectSize) / 2;
    for (int i = 0; i < agent->pathCount; i++)
    {
        DrawRectangle(
            agent->path[i].x*cellSize + rectOffset, 
            agent->path[i].y * cellSize + rectOffset, 
            rectSize, rectSize, agent->color);
    }
    for (int i = 1; i < agent->pathCount; i++)
    {
        DrawLine(agent->path[i-1].x*cellSize + cellSize / 2, agent->path[i-1].y * cellSize + cellSize / 2, agent->path[i].x*cellSize + cellSize / 2, agent->path[i].y * cellSize + cellSize / 2, agent->color);
    }
}

void Agent_drawPathMovement(Agent *agent)
{
    int pathCount = agent->pathCount;
    if (pathCount == 0)
    {
        return;
    }

    PathfindingNode* path = agent->path;
    int pointCount = agent->iconCount;
    Vector2* points = agent->icon;
    float radius = agent->unitSize * cellSize / 2.0f;
    Color color = agent->color;

    float dt = GetFrameTime();
    agent->walkedPathDistance += dt * movementSpeed;
    float pointDistance = 0.0f;
    for (int i=1;i<pathCount;i++)
    {
        PathfindingNode p1 = path[i-1];
        PathfindingNode p2 = path[i];
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        float d = sqrtf(dx * dx + dy * dy);
        if (pointDistance + d >= agent->walkedPathDistance)
        {
            float t = (agent->walkedPathDistance - pointDistance) / d;
            float x = p1.x + dx * t + 0.5f;
            float y = p1.y + dy * t + 0.5f;
            if (points != NULL && pointCount > 0)
            {
                Vector2 fan[32];
                for (int j=0;j<pointCount;j++)
                {
                    float px = points[j].x * radius + x * cellSize;
                    float py = points[j].y * radius + y * cellSize;
                    fan[j] = (Vector2){ px, py };
                }
                DrawTriangleFan(fan, pointCount, color);
            }
            else
                DrawCircle(x * cellSize, y * cellSize, radius, color);
            
            return;
        }
        pointDistance += d;
    }

    agent->walkedPathDistance = 0.0f;
} 

float CalcPathLength(PathfindingNode* path, int pathCount)
{
    float length = 0.0f;
    for (int i=1;i<pathCount;i++)
    {
        PathfindingNode p1 = path[i-1];
        PathfindingNode p2 = path[i];
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        length += sqrtf(dx * dx + dy * dy);
    }
    return length;
}

void AppState_handleInput(AppState *appState)
{
    Vector2 mousePos = GetMousePosition();
    int cellX = mousePos.x / cellSize;
    int cellY = mousePos.y / cellSize;
    appState->cellX = cellX;
    appState->cellY = cellY;

    //----------------------------------------------------------------------------------
    // mouse input handling
    //----------------------------------------------------------------------------------
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        appState->paintMode = blockedCells[cellY * gridWidth + cellX] == 1 ? 0 : 1;
    }
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        blockedCells[cellY * gridWidth + cellX] = appState->paintMode;
        appState->updateSDF = 1;
    }

    //----------------------------------------------------------------------------------
    // keyboard input handling
    //----------------------------------------------------------------------------------
    if (IsKeyDown(KEY_C)) 
    {
        for (int i = 0; i < gridWidth * gridHeight; i++)
        {
            blockedCells[i] = 0;
        }
        appState->updateSDF = 1;
    }

    if (IsKeyPressed(KEY_V))
    {
        appState->visualizeMode++;
    }

    if (IsKeyPressed(KEY_Q))
    {
        appState->rat.wallFactor = (appState->rat.wallFactor + 1) % 8;
        appState->updateSDF = 1;
    }

    if (IsKeyPressed(KEY_R))
    {
        appState->randomizeBlocks = 1;
    }

    if (IsKeyPressed(KEY_S))
    {
        appState->sdfFunction = (appState->sdfFunction + 1) % 3;
        appState->updateSDF = 1;
    }

    if (IsKeyPressed(KEY_J))
    {
        appState->jumpingEnabled = !appState->jumpingEnabled;
        appState->updateSDF = 1;
    }
}

void AppState_randomizeBlocks(AppState *appState)
{
    appState->updateSDF = 1;
    for (int i = 0; i < gridWidth * gridHeight; i++)
    {
        blockedCells[i] = 0;
    }
    for (int i=0;i<40;i++)
    {
        int x = GetRandomValue(15, gridWidth-15);
        int y = GetRandomValue(15, gridHeight-15);
        int size = GetRandomValue(1, 2);
        int blockValue = GetRandomValue(0, 1);
        for (int j=-size;j<=size;j++)
        {
            for (int k=-size;k<=size;k++)
            {
                blockedCells[(y+j)*gridWidth + x+k] = blockValue;
            }
        }
    }
}

int clamp(int value, int min, int max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

void AppState_updateSDF(AppState *appState)
{
    // calculate sdf values
    // update sdf values to max distance we want to consider
    for (int i = 0; i < gridWidth * gridHeight; i++)
    {
        sdfCells[i] = 10;
    }
    
    int sdfFunction = appState->sdfFunction;

    for (int y = 0; y < gridHeight; y++)
    {
        for (int x = 0; x < gridWidth; x++)
        {
            // for each cell, we update the surrounding cells with the distance to this wall
            // doing this brutally simple, for big maps this is inefficient
            if (blockedCells[y * gridWidth + x] == 1)
            {
                sdfCells[y * gridWidth + x] = 0;
                int minX = clamp(x - 10, 0, gridWidth - 1);
                int minY = clamp(y - 10, 0, gridHeight - 1);
                int maxX = clamp(x + 10, 0, gridWidth - 1);
                int maxY = clamp(y + 10, 0, gridHeight - 1);
                // update surrounding cells up to max distance we want to consider
                for (int j = minY; j <= maxY; j++)
                {
                    for (int i = minX; i <= maxX; i++)
                    {
                        int dx = x - i;
                        int dy = y - j;
                        int d = 0;
                        if (sdfFunction == 0)
                        {
                            // euclidean distance
                            d = isqrt[dx * dx + dy * dy];
                        }
                        else if (sdfFunction == 1)
                        {
                            // chebyshev distance
                            d = (abs(dx) < abs(dy)) ? abs(dy) : abs(dx);
                        }
                        else if (sdfFunction == 2)
                        {
                            // manhattan distance
                            d = abs(dx) + abs(dy);
                        }

                        if (d < sdfCells[j*gridWidth + i] && d < 10)
                        {
                            sdfCells[j*gridWidth + i] = d;
                        }
                    }
                }
            }
        }
    }

    // trigger path finding for both agents
    Agent_findPath(&appState->rat, appState->jumpingEnabled);
    Agent_findPath(&appState->cat, appState->jumpingEnabled);
}

void DrawMapContent()
{
    for (int y = 0; y < gridHeight; y++)
    {
        for (int x = 0; x < gridWidth; x++)
        {
            // blocked cells black
            if (blockedCells[y * gridWidth + x] == 1) 
            {
                DrawRectangle(x * cellSize, y * cellSize, cellSize, cellSize, BLACK);
            }

            // sdf values as transparent blue (the further away from wals, the brighter)
            int sdf = sdfCells[y * gridWidth + x];
            DrawRectangle(x*cellSize, y * cellSize, cellSize, cellSize, (Color){ 32, 32, 32, 230-sdf * 20});
        }
    }
}

void DrawPathMapVisualization(PathfindingNode* pathToDraw)
{
    int scoreMax = 0;
    for (int i = 0; i<gridWidth*gridHeight; i++)
    {
        if (pathToDraw[i].score > scoreMax)
        {
            scoreMax = pathToDraw[i].score;
        }
    }
    for (int y = 0; y < gridHeight; y++)
    {
        for (int x = 0; x < gridWidth; x++)
        {
            if (pathToDraw[y * gridWidth + x].score > 0)
            {
                int score = pathToDraw[y * gridWidth + x].score;
                int c = score % 64 * 4;
                DrawRectangle(x*cellSize, y * cellSize, cellSize, cellSize, (Color){ c, c, 0, 128 });
            }
        }
    }
}

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

static int msghandler (lua_State *L) {
  const char *msg = lua_tostring(L, 1);
  if (msg == NULL) {  /* is error object not a string? */
    if (luaL_callmeta(L, 1, "__tostring") &&  /* does it have a metamethod */
        lua_type(L, -1) == LUA_TSTRING)  /* that produces a string? */
      return 1;  /* that is the message */
    else
      msg = lua_pushfstring(L, "(error object is a %s value)",
                               luaL_typename(L, 1));
  }
  luaL_traceback(L, L, msg, 1);  /* append a standard traceback */
  return 1;  /* return the traceback */
}


/*
** Interface to 'lua_pcall', which sets appropriate message function
** and C-signal handler. Used to run all chunks.
*/
static int docall (lua_State *L, int narg, int nres) {
  int status;
  int base = lua_gettop(L) - narg;  /* function index */
  lua_pushcfunction(L, msghandler);  /* push message handler */
  lua_insert(L, base);  /* put it under function and args */
  status = lua_pcall(L, narg, nres, base);
  lua_remove(L, base);  /* remove message handler from the stack */
  return status;
}

#include "tilemap.h"
#include "util.h"

int lua_Tilemap_new(lua_State *L)
{
    int width = luaL_checkinteger(L, 1);
    int height = luaL_checkinteger(L, 2);
    int tileWidth = luaL_optinteger(L, 3, 16);
    int tileHeight = luaL_optinteger(L, 4, 16);
    Tilemap *tilemap = lua_newuserdata(L, sizeof(Tilemap));
    tilemap->tiles = NULL;
    tilemap->filepath = NULL;
    tilemap->width = width;
    tilemap->height = height;
    tilemap->tileWidth = tileWidth;
    tilemap->tileHeight = tileHeight;
    luaL_setmetatable(L, "Tilemap");
    return 1;
}

int lua_Tilemap_draw(lua_State *L)
{
    Tilemap *tilemap = luaL_checkudata(L, 1, "Tilemap");
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    if (tilemap->tiles != NULL)
    {
        Tilemap_draw(tilemap, (Vector2){x, y}, (Vector2){2, 2}, WHITE);
    }
    else
    {
        TraceLog(LOG_ERROR, "tilemap not initialized\n");
    }
    lua_pushvalue(L, 1);
    return 1;
}

int lua_Tilemap_parse(lua_State *L)
{
    Tilemap *tilemap = luaL_checkudata(L, 1, "Tilemap");
    Tilemap_parse(tilemap, luaL_checkstring(L, 2));
    lua_pushvalue(L, 1);
    return 1;
}

int lua_Tilemap_getValue(lua_State *L)
{
    Tilemap *tilemap = luaL_checkudata(L, 1, "Tilemap");
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    if (x < 0 || y < 0 || x >= tilemap->width || y >= tilemap->height)
    {
        lua_pushinteger(L, 0);
        return 1;
    }
    lua_pushinteger(L, tilemap->tiles[y * tilemap->width + x]);
    return 1;
}

int lua_Tilemap_getSize(lua_State *L)
{
    Tilemap *tilemap = luaL_checkudata(L, 1, "Tilemap");
    lua_pushinteger(L, tilemap->width);
    lua_pushinteger(L, tilemap->height);
    return 2;
}

int luaopen_Tilemap(lua_State *L)
{
    luaL_Reg functions[] = {
        {"new", lua_Tilemap_new},
        {"draw", lua_Tilemap_draw},
        {"parse", lua_Tilemap_parse},
        {"getValue", lua_Tilemap_getValue},
        {"getSize", lua_Tilemap_getSize},
        {NULL, NULL}
    };

    luaL_newlib(L, functions);
    luaL_newmetatable(L, "Tilemap");
    lua_pushvalue(L, -2);
    lua_setfield(L, -2, "__index");
    lua_pop(L, 1);

    return 1;
}

static Color luaDrawColor = {255, 255, 255, 255};
static int luaDrawColorAlpha = 255;
static Color luaClearColor = {0, 0, 0, 0};
static int luaCurrentStepIndex = 0;

static Color getCurrentColor()
{
    int a = luaDrawColorAlpha * luaDrawColor.a / 255;
    return (Color){luaDrawColor.r, luaDrawColor.g, luaDrawColor.b, a};
}

int lua_SetCurrentStepIndex(lua_State *L)
{
    luaCurrentStepIndex = luaL_checkinteger(L, 1);
    return 0;
}

int lua_GetCurrentStepIndex(lua_State *L)
{
    lua_pushinteger(L, luaCurrentStepIndex);
    return 1;
}

int lua_SetClearColor(lua_State *L)
{
    int r = luaL_checkinteger(L, 1);
    int g = luaL_checkinteger(L, 2);
    int b = luaL_checkinteger(L, 3);
    int a = luaL_optinteger(L, 4, 255);
    luaClearColor = (Color){r, g, b, a};
    return 0;
}

int lua_SetColor(lua_State *L)
{
    int r = luaL_checkinteger(L, 1);
    int g = luaL_checkinteger(L, 2);
    int b = luaL_checkinteger(L, 3);
    int a = luaL_optinteger(L, 4, 255);
    luaDrawColor = (Color){r, g, b, a};
    return 0;
}

int lua_SetColorAlpha(lua_State *L)
{
    int a = luaL_checkinteger(L, 1);
    luaDrawColorAlpha = a;
    return 0;
}

int lua_SetLineSpacing(lua_State *L)
{
    SetTextLineSpacingEx(luaL_checkinteger(L, 1));
    return 0;
}

int lua_BeginScissorMode(lua_State *L)
{
    BeginScissorMode(luaL_checkinteger(L, 1), luaL_checkinteger(L, 2),
        luaL_checkinteger(L,3), luaL_checkinteger(L, 4));
    return 0;
}

int lua_EndScissorMode(lua_State *L)
{
    EndScissorMode();
    return 0;
}

int lua_DrawRectangle(lua_State *L)
{
    int x = (int)luaL_checknumber(L, 1);
    int y = (int)luaL_checknumber(L, 2);
    int w = (int)luaL_checknumber(L, 3);
    int h = (int)luaL_checknumber(L, 4);
    DrawRectangle(x, y, w, h, getCurrentColor(luaDrawColor));
    return 0;
}

int lua_GetScreenSize(lua_State *L)
{
    lua_pushinteger(L, GetScreenWidth());
    lua_pushinteger(L, GetScreenHeight());
    return 2;
}

int lua_DrawBubble(lua_State *L)
{
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int w = luaL_checkinteger(L, 3);
    int h = luaL_checkinteger(L, 4);
    float angle = luaL_checknumber(L, 5);
    int arrowX = luaL_checkinteger(L, 6);
    int arrowY = luaL_checkinteger(L, 7);
    DrawBubble(x, y, w, h, angle, arrowX, arrowY, getCurrentColor(luaDrawColor));
    return 0;
}

int lua_DrawTextBoxAligned(lua_State *L)
{
    const char *text = luaL_checkstring(L, 1);
    int fontSize = luaL_checkinteger(L, 2);
    int x = luaL_checkinteger(L, 3);
    int y = luaL_checkinteger(L, 4);
    int w = luaL_checkinteger(L, 5);
    int h = luaL_checkinteger(L, 6);
    float alignX = luaL_checknumber(L, 7);
    float alignY = luaL_checknumber(L, 8);
    Rectangle rect = DrawTextBoxAligned(text, fontSize, x, y, w, h, alignX, alignY, getCurrentColor(luaDrawColor));
    lua_pushnumber(L, rect.x);
    lua_pushnumber(L, rect.y);
    lua_pushnumber(L, rect.width);
    lua_pushnumber(L, rect.height);
    return 4;
}

int lua_DrawTriangle(lua_State *L)
{
    float x1 = (float)luaL_checknumber(L, 1);
    float y1 = (float)luaL_checknumber(L, 2);
    float x2 = (float)luaL_checknumber(L, 3);
    float y2 = (float)luaL_checknumber(L, 4);
    float x3 = (float)luaL_checknumber(L, 5);
    float y3 = (float)luaL_checknumber(L, 6);
    DrawTriangle((Vector2){x1, y1}, (Vector2){x2, y2}, (Vector2){x3, y3}, getCurrentColor(luaDrawColor));
    return 0;
}

int lua_DrawLine(lua_State *L)
{
    float x1 = (float)luaL_checknumber(L, 1);
    float y1 = (float)luaL_checknumber(L, 2);
    float x2 = (float)luaL_checknumber(L, 3);
    float y2 = (float)luaL_checknumber(L, 4);
    float thickness = (float)luaL_optnumber(L, 5, 1.0f);

    DrawLineEx((Vector2){x1, y1}, (Vector2){x2, y2}, thickness, getCurrentColor(luaDrawColor));
    return 0;
}

int lua_Sprite(lua_State *L)
{
    float srcX = (float)luaL_checknumber(L, 1);
    float srcY = (float)luaL_checknumber(L, 2);
    float srcWidth = (float)luaL_checknumber(L, 3);
    float srcHeight = (float)luaL_checknumber(L, 4);
    float dstX = (float)luaL_checknumber(L, 5);
    float dstY = (float)luaL_checknumber(L, 6);
    float dstWidth = (float)luaL_optnumber(L, 7, srcWidth * 2);
    float dstHeight = (float)luaL_optnumber(L, 8, srcHeight * 2);
    Texture2D texture = resources.tileset;
    Rectangle srcRec = (Rectangle){ srcX, srcY, srcWidth, srcHeight };
    Rectangle dstRec = (Rectangle){ dstX, dstY, dstWidth, dstHeight };
    DrawTexturePro(texture, srcRec, dstRec, (Vector2){0, 0}, 0.0f, getCurrentColor(luaDrawColor));
    return 0;
}

int lua_GetFrameTime(lua_State *L)
{
    lua_pushnumber(L, GetFrameTime());
    return 1;
}

int lua_GetTime(lua_State *L)
{
    lua_pushnumber(L, GetTime());
    return 1;
}

int lua_IsNumberKeyPressed(lua_State *L)
{
    int key = luaL_checkinteger(L, 1);
    lua_pushboolean(L, IsKeyPressed(key + KEY_ZERO));
    return 1;
}

int lua_IsNextPagePressed(lua_State *L)
{
    lua_pushboolean(L, IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_RIGHT));
    return 1;
}

int lua_IsPreviousPagePressed(lua_State *L)
{
    lua_pushboolean(L, IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_LEFT));
    return 1;
}

int lua_IsMenuKeyPressed(lua_State*L)
{
    lua_pushboolean(L, IsKeyPressed(KEY_SPACE));
    return 1;
}

void init_lua(lua_State *L)
{
    luaL_openlibs(L);
    
    luaL_requiref(L, "Tilemap", luaopen_Tilemap, 0);

    luaL_Reg functions[] = {
        {"SetClearColor", lua_SetClearColor},
        {"SetColor", lua_SetColor},
        {"SetColorAlpha", lua_SetColorAlpha},
        {"SetLineSpacing", lua_SetLineSpacing},
        {"SetCurrentStepIndex", lua_SetCurrentStepIndex},
        {"GetCurrentStepIndex", lua_GetCurrentStepIndex},
        {"DrawTextBoxAligned", lua_DrawTextBoxAligned},
        {"BeginScissorMode", lua_BeginScissorMode},
        {"EndScissorMode", lua_EndScissorMode},
        {"DrawRectangle", lua_DrawRectangle},
        {"GetScreenSize", lua_GetScreenSize},
        {"DrawBubble", lua_DrawBubble},
        {"DrawTriangle", lua_DrawTriangle},
        {"DrawLine", lua_DrawLine},
        {"Sprite", lua_Sprite},
        {"GetTime", lua_GetTime},
        {"GetFrameTime", lua_GetFrameTime},
        {"IsNumberKeyPressed", lua_IsNumberKeyPressed},
        {"IsNextPagePressed", lua_IsNextPagePressed},
        {"IsPreviousPagePressed", lua_IsPreviousPagePressed},
        {"IsMenuKeyPressed", lua_IsMenuKeyPressed},
        {NULL, NULL}
    };
    lua_pushglobaltable(L);
    luaL_setfuncs(L, functions, 0);
    lua_pop(L, 1);

    if (luaL_loadfile(L, "resources/script.lua") != 0) {
        luaL_traceback(L, L, lua_tostring(L, -1), 1);
        TraceLog(LOG_ERROR, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
        return;
    }
    
    if (docall(L, 0, 0) != 0) {
        luaL_traceback(L, L, lua_tostring(L, -1), 1);
        TraceLog(LOG_ERROR, "%s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
        return;
    }
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    lua_State *L = luaL_newstate();
    SetTraceLogLevel(LOG_ALL);
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [sdf pathfinding] example");

    SetTargetFPS(60);

    // initialize square root lookup table for cheap square root calculation
    for (int i=0;i<256;i++)
    {
        isqrt[i] = (int)ceilf(sqrtf(i));
    }

    for (int x = -10; x <= 10; x++)
    {
        for (int y = -10; y <= 10; y++)
        {
            int d = isqrt[x * x + y * y];
            if (d <= 10 && d > 0)
            {
                neighborOffsets[neighborOffsetCount] = (NeighborOffset){ x, y, d };
                neighborOffsetCount++;
            }
        }
    }

    blockedCells = (char *)MemAlloc(gridWidth * gridHeight * sizeof(char));
    sdfCells = (char *)MemAlloc(gridWidth * gridHeight * sizeof(char));

    AppState appState = {
        .visualizeMode = 0,
        .randomizeBlocks = 1,
        .paintMode = 0,
        .updateSDF = 1,
        .sdfFunction = 0,
        .jumpingEnabled = 1,
        .rat = Agent_init(5, 25, 1, 75, 25, 2, ratFace, sizeof(ratFace) / sizeof(ratFace[0]), RED),
        .cat = Agent_init(5, 25, 2, 75, 25, 0, catFace, sizeof(catFace) / sizeof(catFace[0]), BLUE),
    };

    Resources_load();
    
    int codePoints[256];
    for (int i=0;i<256;i++)
    {
        codePoints[i] = i + 32;
    }
    codePoints[200] = 0x25CF;
    SetDefaultFonts((Font[]){
        LoadFontEx("resources/Roboto-Bold.ttf", 15, codePoints, 256),
        LoadFontEx("resources/Roboto-Bold.ttf", 20, codePoints, 256),
        LoadFontEx("resources/Roboto-Bold.ttf", 30, codePoints, 256),
        LoadFontEx("resources/Roboto-Bold.ttf", 40, codePoints, 256),
        {0}
    });
    SetDefaultMonoFonts((Font[]){
        LoadFontEx("resources/RobotoMono-Medium.ttf", 10, codePoints, 256),
        LoadFontEx("resources/RobotoMono-Medium.ttf", 15, codePoints, 256),
        LoadFontEx("resources/RobotoMono-Medium.ttf", 20, codePoints, 256),
        LoadFontEx("resources/RobotoMono-Medium.ttf", 30, codePoints, 256),
        LoadFontEx("resources/RobotoMono-Medium.ttf", 40, codePoints, 256),
        {0}
    });

    // Tutorial_init();
    init_lua(L);
    
    //--------------------------------------------------------------------------------------
    long scriptModTime = GetFileModTime("resources/script.lua");
    float reloadTimeOut = 0.0f;
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        Resources_update();
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_RIGHT)) luaCurrentStepIndex++;
        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_LEFT)) luaCurrentStepIndex--;
        if (IsKeyPressed(KEY_R)) luaCurrentStepIndex = 0;

        if (GetFileModTime("resources/script.lua") > scriptModTime)
        {
            scriptModTime = GetFileModTime("resources/script.lua");
            reloadTimeOut = 0.5f + GetTime();
        }
        if (IsKeyPressed(KEY_F5) || (GetTime() > reloadTimeOut && reloadTimeOut > 0.0f))
        {
            reloadTimeOut = 0.0f;
            TraceLog(LOG_INFO, "closing old lua state\n");
            // lua_close(L);
            TraceLog(LOG_INFO, "reloading script\n");
            L = luaL_newstate();
            init_lua(L);
        }
        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(luaClearColor);

            lua_getglobal(L, "draw");
            if (lua_isfunction(L, -1))
            {
                lua_pushnumber(L, GetFrameTime());
                if (docall(L, 1, 0) != 0) {
                    luaL_traceback(L, L, lua_tostring(L, -1), 1);
                    TraceLog(LOG_ERROR, "%s\n", lua_tostring(L, -1));
                    lua_pop(L, 1);
                    lua_pushnil(L);
                    lua_setglobal(L, "draw");
                }
            }

            // if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_RIGHT)) Tutorial_nextSubstep(IsKeyDown(KEY_LEFT_SHIFT));
            // if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_RIGHT)) Tutorial_prevPage();

            // Tutorial_update(GetFrameTime());
            // Tutorial_draw(GetFrameTime());


            // AppState_handleInput(&appState);

            // //----------------------------------------------------------------------------------
            // // initialize map with random blocks
            // //----------------------------------------------------------------------------------
            // if (appState.randomizeBlocks)
            // {
            //     appState.randomizeBlocks = 0;
            //     AppState_randomizeBlocks(&appState);
            // }

            // //----------------------------------------------------------------------------------
            // // update sdf values and execute pathfinding
            // //----------------------------------------------------------------------------------
            // if (appState.updateSDF)
            // {
            //     appState.updateSDF = 0;
            //     AppState_updateSDF(&appState);
            // }

            // //----------------------------------------------------------------------------------
            // // draw cell content of walls and sdf values
            // //----------------------------------------------------------------------------------
            // DrawMapContent();

            // //----------------------------------------------------------------------------------
            // // draw rat pathfinding score data for visualization
            // //----------------------------------------------------------------------------------
            // PathfindingNode* pathToDraw = NULL;
            // switch (appState.visualizeMode % 3)
            // {
            //     case 1: // visualize rat map
            //         pathToDraw = appState.rat.map;
            //         break;
            //     case 2: // visualize cat map
            //         pathToDraw = appState.cat.map;
            //         break;
            // }

            // if (pathToDraw != NULL)
            // {
            //     DrawPathMapVisualization(pathToDraw);
            // }

            // //----------------------------------------------------------------------------------
            // // draw grid lines
            // //----------------------------------------------------------------------------------
            // for (int y = 0; y < gridHeight; y++)
            // {
            //     DrawRectangle(0, y * cellSize, gridWidth*cellSize, 1, gridColor);
            // }
            // for (int x = 0; x < gridWidth; x++)
            // {
            //     DrawRectangle(x*cellSize, 0, 1, gridHeight*cellSize, gridColor);
            // }

            // // highlight current cell the mouse is over
            // DrawRectangle(appState.cellX * cellSize, appState.cellY * cellSize, cellSize, cellSize, cellHighlightColor);


            // //----------------------------------------------------------------------------------
            // // draw paths of cat and rat
            // //----------------------------------------------------------------------------------
            // Agent_drawPath(&appState.rat);
            // Agent_drawPath(&appState.cat);

            // //----------------------------------------------------------------------------------
            // // draw animated movement of rat and cat
            // //----------------------------------------------------------------------------------
            // Agent_drawPathMovement(&appState.rat);
            // Agent_drawPathMovement(&appState.cat);
            
            // //----------------------------------------------------------------------------------
            // // description and status
            // //----------------------------------------------------------------------------------
            // DrawText("Left click to toggle blocked cells, C: clear, Left mouse: toggle cell", 10, 10, 20, BLACK);
            // DrawText("The red rat is small and likes to run close to walls", 10, 30, 20, RED);
            // DrawText("The blue cat is big and can't fit through narrow paths and\nprefers the short path", 10, 50, 20, BLUE);
            // DrawText(TextFormat("Rat path length: %.2f, Cat path length: %.2f", 
            //     CalcPathLength(appState.rat.path, appState.rat.pathCount), 
            //     CalcPathLength(appState.cat.path, appState.rat.pathCount)), 
            //     10, GetScreenHeight() - 100, 20, BLACK);
            // DrawText(TextFormat("R: randomize blocks, J: jumping enabled (current: %s)", appState.jumpingEnabled ? "yes" : "no"), 10, GetScreenHeight() - 80, 20, BLACK);
            // DrawText(TextFormat("S: switch SDF function (current: %s)", appState.sdfFunction == 0 ? "euclidean" : (appState.sdfFunction == 1 ? "chebyshev" : "manhattan")), 10, GetScreenHeight() - 60, 20, BLACK);
            // DrawText(TextFormat("Q: Rat wall factor (how much the rat wants to stay close to walls): %d", appState.rat.wallFactor), 10, GetScreenHeight() - 40, 20, BLACK);
            // DrawText(TextFormat("V: switch visualization mode (current: %s)", appState.visualizeMode % 3 == 0 ? "none" : (appState.visualizeMode % 3 == 1 ? "map rat" : "map cat")), 10, GetScreenHeight() - 20, 20, BLACK);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();
    //--------------------------------------------------------------------------------------

    return 0;
}

