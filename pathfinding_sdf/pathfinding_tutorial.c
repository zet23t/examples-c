#include "pathfinding_tutorial.h"
#include "util.h"

static TutorialState tutorialState;

#define PAGE(n) \
    void page_##n##_onInit(TutorialPage *page); \
    void page_##n##_onEnter(TutorialPage *page); \
    void page_##n##_onLeave(TutorialPage *page); \
    void page_##n##_onUpdate(TutorialPage *page, float dt); \
    void page_##n##_onDraw(TutorialPage *page, float dt); \
    bool page_##n##_onNextSubstep(TutorialPage *page);
#include "tutorial_page_list.h"
#undef PAGE

void Tutorial_init()
{
    static TutorialPage pages[] = {
        #define PAGE(n) { \
            .onInit = page_##n##_onInit, \
            .onEnter = page_##n##_onEnter, \
            .onLeave = page_##n##_onLeave, \
            .onUpdate = page_##n##_onUpdate, \
            .onDraw = page_##n##_onDraw, \
            .onNextSubstep = page_##n##_onNextSubstep, \
        },
        #include "tutorial_page_list.h"
        #undef PAGE
    };
    tutorialState.pages = pages;
    tutorialState.pageCount = sizeof(pages)/sizeof(pages[0]);
    tutorialState.currentPage = 0;

    for (int i = 0; i < tutorialState.pageCount; i++)
    {
        tutorialState.pages[i].onInit(&tutorialState.pages[i]);
    }

    tutorialState.pages[tutorialState.currentPage].onEnter(&tutorialState.pages[tutorialState.currentPage]);

    int codePoints[256];
    for (int i=0;i<256;i++)
    {
        codePoints[i] = i + 32;
    }
    codePoints[200] = 0x25CF;
    SetDefaultFonts((Font[]){
        LoadFontEx("Roboto-Bold.ttf", 20, codePoints, 256),
        LoadFontEx("Roboto-Bold.ttf", 30, codePoints, 256),
        LoadFontEx("Roboto-Bold.ttf", 40, codePoints, 256),
        {0}
    });
}

void Tutorial_update(float dt)
{
    tutorialState.pages[tutorialState.currentPage].onUpdate(&tutorialState.pages[tutorialState.currentPage], dt);
}

void Tutorial_draw(float dt)
{
    tutorialState.pages[tutorialState.currentPage].onDraw(&tutorialState.pages[tutorialState.currentPage], dt);

    DrawRectangle(0,GetScreenHeight()-30, GetScreenWidth(), 30, (Color){200, 200, 200, 128});
    DrawStyledTextBox((StyledTextBox){
        .text = "[Enter] >",
        .fontSize = 20,
        .box.x = 0,
        .box.y = 0,
        .box.width = GetScreenWidth() - 10,
        .box.height = GetScreenHeight() - 5,
        .align.x = 1.0f,
        .align.y = 1.0f,
        .color = DARKBLUE,
    });

    DrawStyledTextBox((StyledTextBox){
        .text = "< [Backspace]",
        .fontSize = 20,
        .box.x = 10,
        .box.y = 0,
        .box.width = GetScreenWidth() - 10,
        .box.height = GetScreenHeight() - 5,
        .align.x = 0.0f,
        .align.y = 1.0f,
        .color = DARKBLUE,
    });

    DrawStyledTextBox((StyledTextBox){
        .text = TextFormat("%d / %d", tutorialState.currentPage + 1, tutorialState.pageCount),
        .fontSize = 20,
        .box.x = 0,
        .box.y = 0,
        .box.width = GetScreenWidth(),
        .box.height = GetScreenHeight() - 5,
        .align.x = 0.5f,
        .align.y = 1.0f,
        .color = DARKBLUE,
    });
}

void Tutorial_nextPage(bool skipSteps)
{
    tutorialState.pages[tutorialState.currentPage].onLeave(&tutorialState.pages[tutorialState.currentPage]);
    tutorialState.currentPage = (tutorialState.currentPage + 1) % tutorialState.pageCount;
    tutorialState.pages[tutorialState.currentPage].onEnter(&tutorialState.pages[tutorialState.currentPage]);
    TutorialPage *page = &tutorialState.pages[tutorialState.currentPage];
    while (skipSteps && page->onNextSubstep(page)) {}
}

void Tutorial_prevPage()
{
    tutorialState.pages[tutorialState.currentPage].onLeave(&tutorialState.pages[tutorialState.currentPage]);
    tutorialState.currentPage = (tutorialState.currentPage - 1 + tutorialState.pageCount) % tutorialState.pageCount;
    tutorialState.pages[tutorialState.currentPage].onEnter(&tutorialState.pages[tutorialState.currentPage]);
    TutorialPage *page = &tutorialState.pages[tutorialState.currentPage];
    while (page->onNextSubstep(page)) {}
}

void Tutorial_nextSubstep(bool skipSteps)
{
    TutorialPage *page = &tutorialState.pages[tutorialState.currentPage];
    if (!page->onNextSubstep(page)) Tutorial_nextPage(skipSteps);
}