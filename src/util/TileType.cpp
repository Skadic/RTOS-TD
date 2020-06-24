
#include "TileType.h"

#include <map>
#include "GlobalConsts.h"
#include "../components/Sprites.h"

std::map<TileType, std::shared_ptr<Sprite>> initTileSprites() {
    std::map<TileType, std::shared_ptr<Sprite>> map;
    map[EMPTY] = std::make_shared<EmptySprite>();
    map[WALL] = std::make_shared<RectangleSprite>(TILE_SIZE, TILE_SIZE, 0xFFFFFF, false);
    map[TOWER] = std::make_shared<RectangleSprite>(TILE_SIZE, TILE_SIZE, 0x00FFFF, false);
    map[GOAL] = std::make_shared<RectangleSprite>(TILE_SIZE, TILE_SIZE, 0x00FF00, true);
    return map;
}

std::map<TileType, std::shared_ptr<Sprite>> TILE_SPRITE_MAP = initTileSprites();

std::shared_ptr<Sprite> &getSpriteForType(TileType type) {
    return TILE_SPRITE_MAP[type];
}

bool isSolid(TileType type) {
    return type != EMPTY && type != GOAL;
}