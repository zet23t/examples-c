#include "resources.h"

Resources resources;

void Resources_load()
{
    resources.tileset = LoadTexture("tileset.png");
}