
#include "TileType.h"

#include "../GlobalConsts.h"
#include "../../components/Sprites.h"

std::map<TileType, std::shared_ptr<Sprite>> initTileSprites() {
    std::map<TileType, std::shared_ptr<Sprite>> map;
    map[EMPTY] = std::make_shared<EmptySprite>();
    map[WALL] = std::make_shared<RectangleSprite>(TILE_SIZE, TILE_SIZE, 0xFFFFFF, false);
    map[TOWER_PROJECTILE] = std::make_shared<RectangleSprite>(TILE_SIZE, TILE_SIZE, 0x00FFFF, true);
    map[TOWER_LASER] = std::make_shared<RectangleSprite>(TILE_SIZE, TILE_SIZE, 0x4000FF, true);
    map[GOAL] = std::make_shared<RectangleSprite>(TILE_SIZE, TILE_SIZE, 0x00FF00, true);
    map[ENEMY_SPAWN] = std::make_shared<Texture2ColorSprite>("test.png", 0xFF0000, 0x00FF00);
    return map;
}


std::map<TileType, int> initCostMap() {
    std::map<TileType, int> map;
    map[EMPTY] = 0;
    map[WALL] = 1;
    map[TOWER_LASER] = 6;
    map[TOWER_PROJECTILE] = 5;
    return map;
}

std::map<SDL_Scancode, TileType> initScancodeMap() {
    std::map<SDL_Scancode, TileType> map;
    map[SDL_SCANCODE_1] = WALL;
    map[SDL_SCANCODE_2] = TOWER_PROJECTILE;
    map[SDL_SCANCODE_3] = TOWER_LASER;
    return map;
}

std::map<TileType, char*> initNameMap() {
    std::map<TileType, char*> map;
    map[WALL] = "Wall";
    map[TOWER_PROJECTILE] = "Gun Turret";
    map[TOWER_LASER] = "Laser Turret";
    return map;
}

std::map<TileType, std::shared_ptr<Sprite>> TILE_SPRITE_MAP = initTileSprites();
std::map<TileType, int> TILE_COST_MAP = initCostMap();
std::map<SDL_Scancode, TileType> SCANCODE_MAP = initScancodeMap();
std::map<TileType, char*> NAME_MAP = initNameMap();



int getCostForType(TileType type) {
    return TILE_COST_MAP[type];
}

std::shared_ptr<Sprite> &getSpriteForType(TileType type) {
    return TILE_SPRITE_MAP[type];
}

bool isSolid(TileType type) {
    return type != EMPTY && type != GOAL && type != ENEMY_SPAWN;
}

bool isSpecial(TileType type) {
    return type == GOAL || type == ENEMY_SPAWN;
}

const std::map<SDL_Scancode, TileType> &getScancodeMap() {
    return SCANCODE_MAP;
}

char* &getName(TileType type) {
    return NAME_MAP[type];
}
