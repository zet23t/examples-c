#ifndef __PATHFINDING_TUTORIAL_H__
#define __PATHFINDING_TUTORIAL_H__

#include "raylib.h"
#include "raymath.h"


typedef struct TutorialPage
{
    void *pageData;
    void (*onInit)(struct TutorialPage* page);
    void (*onEnter)(struct TutorialPage* page);
    void (*onLeave)(struct TutorialPage* page);
    void (*onUpdate)(struct TutorialPage* page, float dt);
    void (*onDraw)(struct TutorialPage* page, float dt);
    bool (*onNextSubstep)(struct TutorialPage* page);
} TutorialPage;

typedef struct TutorialState
{
    TutorialPage *pages;
    int pageCount;
    int currentPage;
} TutorialState;

void Tutorial_init();
void Tutorial_update(float dt);
void Tutorial_draw(float dt);
void Tutorial_nextPage(bool skipSteps);
void Tutorial_prevPage();
void Tutorial_nextSubstep(bool skipSteps);

#define PAGE_EMPTY_INIT(n)  

#endif