#ifndef __TILEMAP_H__
#define __TILEMAP_H__

#include "raylib.h"

typedef struct Tilemap
{
    unsigned char *tiles;
    int width;
    int height;
    int tileWidth;
    int tileHeight;
} Tilemap;

Tilemap Tilemap_draw(Tilemap *tilemap, Vector2 position, Vector2 scale, Color color);

#endif