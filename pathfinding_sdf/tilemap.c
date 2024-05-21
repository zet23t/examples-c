#include "tilemap.h"
#include "resources.h"

typedef struct TileTypeSpriteLocation
{
    unsigned char x;
    unsigned char y;
} TileTypeSpriteLocation;

// a mapping of a pattern of 2x2 vertices to a sprite in the tileset
TileTypeSpriteLocation tileTypeSpriteLocations[] = {
    // 0 0 
    // 0 0 (does not exist)
    {0xff, 0xff},
    // 0 0
    // 0 1
    {0, 0},
    // 0 0
    // 1 0
    {3, 0},
    // 0 0
    // 1 1
    {1, 0},
    // 0 1
    // 0 0
    {0, 4},
    // 0 1
    // 0 1
    {0, 1},
    // 0 1
    // 1 0 (does not exist)
    {0xff, 0xff},
    // 0 1
    // 1 1
    {2, 3},
    // 1 0
    // 0 0
    {3, 4},
    // 1 0
    // 0 1 (does not exist)
    {0xff, 0xff},
    // 1 0
    // 1 0
    {3, 1},
    // 1 0
    // 1 1
    {1, 3},
    // 1 1
    // 0 0
    {1, 4},
    // 1 1
    // 0 1
    {2, 2},
    // 1 1
    // 1 0
    {1, 2},
    // 1 1
    // 1 1
    {1, 1}
};

// this is a naive and simple tile drawing implementation
Tilemap Tilemap_draw(Tilemap *tilemap, Vector2 position, Vector2 scale, Color color)
{
    // the tilemap sprites are stored in a single texture
    // Each tile type is expressed by a group of sprites in the tileset.
    // The pattern describes the tile itself and its transitions to transparent
    // borders. The following visual pattern describes the _vertices_ of each tile where 
    // 0 is transparent and 1 is solid 
    // 0 0 0 0 0
    // 0 1 1 1 0
    // 0 1 1 1 0
    // 0 1 0 1 0
    // 0 1 1 1 0
    // 0 0 0 0 0
    // Each 2x2 square is one tile with each corner representing a vertex. The pattern
    // 0 1
    // 0 1
    // represents a tile where the right side is solid and the left side is transparent.
    // The pattern
    // 0 1
    // 1 0
    // is not present and must be handled by the drawing logic by combining these two tiles
    // 0 1 | 0 0
    // 0 0 | 1 0
    // The tilemap is stored as a 1D array of unsigned chars where each byte represents a tile
    // type of a _vertice_ of a tile. 
    // The order of drawing tile types is determined by the ordinal value of the tile type.

    int maxY = tilemap->height - 1;
    int maxX = tilemap->width - 1;

    for (int y = 0; y < maxY; y++)
    {
        int tileIndex = y * tilemap->width;
        for (int x = 0; x < maxX; x++, tileIndex++)
        {
            unsigned char typeA = tilemap->tiles[tileIndex];
            unsigned char typeB = tilemap->tiles[tileIndex + 1];
            unsigned char typeC = tilemap->tiles[tileIndex + tilemap->width];
            unsigned char typeD = tilemap->tiles[tileIndex + tilemap->width + 1];

            // iterate over all possible types to determine which ones to draw 
            for (int tileTypeId = 0; tileTypeId < 4; tileTypeId++)
            {
                unsigned char typeAId = (typeA == tileTypeId) ? 1 : 0;
                unsigned char typeBId = (typeB == tileTypeId) ? 1 : 0;
                unsigned char typeCId = (typeC == tileTypeId) ? 1 : 0;
                unsigned char typeDId = (typeD == tileTypeId) ? 1 : 0;

                int tileTypePattern = typeAId << 3 | typeBId << 2 | typeCId << 1 | typeDId << 0;
                if (tileTypePattern == 0) continue;
                int hasDiagonalPattern = 0;
                if (tileTypePattern == 0b1001) // diagonal pattern
                {
                    hasDiagonalPattern = 0b0001;
                    tileTypePattern = 0b1000;
                }
                if (tileTypePattern == 0b0110)
                {
                    hasDiagonalPattern = 0b0010;
                    tileTypePattern = 0b0100;
                }

                paintTile:
                TileTypeSpriteLocation spriteLocation = tileTypeSpriteLocations[tileTypePattern];
                Vector2 tilePosition = (Vector2){
                    position.x + x * tilemap->tileWidth * scale.x, 
                    position.y + y * tilemap->tileHeight * scale.y};
                Rectangle source = (Rectangle){
                    (tileTypeId * 4 + spriteLocation.x) * tilemap->tileWidth, 
                    spriteLocation.y * tilemap->tileHeight, 
                    tilemap->tileWidth, tilemap->tileHeight
                };
                DrawTexturePro(resources.tileset, source, (Rectangle){tilePosition.x, tilePosition.y, tilemap->tileWidth * scale.x, tilemap->tileHeight * scale.y}, 
                    (Vector2){0, 0}, 0, color);
                
                if (hasDiagonalPattern)
                {
                    tileTypePattern = hasDiagonalPattern;
                    hasDiagonalPattern = 0;
                    goto paintTile;
                }
                
            }
        }
    }
    return *tilemap;
}