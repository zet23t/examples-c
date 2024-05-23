#include "resources.h"

Resources resources;

void Resources_load()
{
    resources.tileset = LoadTexture("resources/tileset.png");
}