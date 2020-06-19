
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

    std::optional<entt::entity> getMapTileAtScreenPos(short x, short y, Renderer &renderer);

    void updateTile(int x, int y, entt::registry &registry, TileType type);
    void updateTileAtScreenPos(short x, short y, entt::registry &registry, TileType type, Renderer &renderer);

    entt::entity &getMapTile(int x, int y);
    TileType getTileType(int x, int y, entt::registry &registry);
};

std::shared_ptr<Sprite> getSpriteForType(TileType type);