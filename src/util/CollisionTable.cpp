//
// Created by skadic on 29.07.20.
//

#include "CollisionTable.h"
#include "GlobalConsts.h"

CollisionTable::CollisionTable(int w, int h) : width{w}, height{h} {
    buckets.resize(height);
    for (int i = 0; i < height; ++i) {
        buckets[i].resize(width);
    }
}

void CollisionTable::reset() {
    for (int i = 0; i < height; ++i) {
        buckets[i].clear();
        buckets[i].resize(width);
    }
}

void CollisionTable::refresh(entt::registry &registry) {
    reset();
    auto view = registry.view<Position, Hitbox>();

    for(auto &entity : view) {
        Position &pos = view.get<Position>(entity);
        Hitbox &hitbox = view.get<Hitbox>(entity);
        auto intersecting = getIntersectingTiles(pos, hitbox);

        for(TilePosition tPos : intersecting) {
            if (tPos.x >= 0 && tPos.x < width && tPos.y >= 0 && tPos.y < height) {
                buckets[tPos.y][tPos.x].push_back(entity);
            }
        }
    }
}



std::vector<TilePosition> CollisionTable::getIntersectingTiles(Position pos, Hitbox hitbox) {
    std::vector<TilePosition> posSet;

    TilePosition topLeft = {static_cast<short>(pos.x / TILE_SIZE), static_cast<short>(pos.y / TILE_SIZE)};
    TilePosition bottomRight = {static_cast<short>((pos.x + hitbox.width) / TILE_SIZE), static_cast<short>((pos.y + hitbox.height) / TILE_SIZE)};

    for (short y = topLeft.y; y <= bottomRight.y; ++y) {
        for (short x = topLeft.x; x <= bottomRight.x; ++x) {
            if (x >= 0 && x < width && y >= 0 && y < height) {
                posSet.push_back({x, y});
            }
        }
    }

    return posSet;
}

std::vector<entt::entity>& CollisionTable::getIntersecting(int x, int y) {
    return buckets[y][x];
}

std::vector<entt::entity>& CollisionTable::getIntersecting(TilePosition pos) {
    return getIntersecting(pos.x, pos.y);
}


