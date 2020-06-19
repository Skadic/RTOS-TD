
#include <vector>
#include <entity/registry.hpp>
#include "LockGuard.h"
#include "TileType.h"
#include "../components/Sprites.h"
#include "Renderer.h"



class Map {
    std::vector<std::vector<entt::entity>> tiles;
    int mapWidth;
    int mapHeight;


public:
    Map(entt::registry &regMutex, int width, int height);
    Map(entt::registry &regMutex, std::string path);
    int getWidth() const;
    int getHeight() const;

    std::optional<entt::entity> getMapTileFromScreenPos(short x, short y, Renderer &renderer);

    entt::entity &getMapTile(int x, int y);
};

std::shared_ptr<Sprite> getSpriteForType(TileType type);