//
// Created by skadic on 24.06.20.
//

#include "AStar.h"

#include <cmath>
#include <map>
#include <queue>
#include <set>
#include <iostream>

double heuristicDistance(TilePosition start, TilePosition end) {
    auto xDist = start.x - end.x;
    auto yDist = start.y - end.y;
    return sqrt(xDist * xDist + yDist * yDist);
}

struct TileData {
    TilePosition pos{};
    double g{};
    double h{};
    TileData *pred;

    TileData(const TilePosition &pos, TileData *pred = nullptr) : pos(pos), pred(pred) {}

    double getScore() {
        return g + h;
    }
};

struct ScoreCompare {
    bool operator() (TileData *lhs, TileData *rhs) const {
        if(lhs->getScore() != rhs->getScore())
            return lhs->getScore() < rhs->getScore();
        else if(lhs->pos.x != rhs->pos.x) {
            return lhs->pos.x < rhs->pos.x;
        } else {
            return lhs->pos.y < rhs->pos.y;
        }
    }
};

TileData* findPos(TilePosition pos, std::set<TileData*, ScoreCompare> &s) {
    for(auto &ptr : s) {
        if(ptr->pos == pos) {
            return ptr;
        }
    }
    return nullptr;
}


void dealloc(std::set<TileData*, ScoreCompare> set) {
    for (auto data = set.begin(); data != set.end();) {
        delete *data;
        data = set.erase(data);
    }
}


// A* Pathfinding algorithm
// Code inspired by https://github.com/daancode/a-star
std::vector<TilePosition> aStarPathfinding(TilePosition start, TilePosition end, Map &map, entt::registry &registry) {
    TileData *current = nullptr;
    std::set<TileData*, ScoreCompare> openSet {};
    std::set<TileData*, ScoreCompare> closedSet {};
    openSet.insert(new TileData(start));

    while (!openSet.empty()) {
        current = *openSet.begin();

        if(current->pos == end) { break; }

        closedSet.insert(current);
        openSet.erase(openSet.find(current));

        for (int i = 0; i < 4; ++i) {
            TilePosition newPos = current->pos.neighbor((Direction) i);
            if(isSolid(map.getTileType(newPos.x, newPos.y, registry)) || findPos(newPos, closedSet)) {
                continue;
            }

            auto totalCost = current->g + 1;

            if(TileData *newTile = findPos(newPos, openSet)) {
                if(totalCost < newTile->g) {
                    openSet.erase(newTile);
                    newTile->pred = current;
                    newTile->g = totalCost;
                    openSet.insert(newTile);
                }
            } else {
                newTile = new TileData(newPos, current);
                newTile->g = totalCost;
                newTile->h = heuristicDistance(newPos, end);
                openSet.insert(newTile);
            }
        }
    }

    std::vector<TilePosition> path;
    while (current != nullptr) {
        path.push_back(current->pos);
        current = current->pred;
    }
    std::reverse(path.begin(), path.end());

    dealloc(openSet);
    dealloc(closedSet);

    return path;
}


/*std::vector<TilePosition> aStarPathfinding(TilePosition start, TilePosition end, Map &map, entt::registry &registry) {

    struct TileData {
        TilePosition pos{};
        double startDist{};
        double estimatedCost{};
    };

    std::map<TilePosition, TileData> data;

    struct TPCompare {
        const auto sortFunction = [&data](const TilePosition &a, const TilePosition &b) {
            return (data[a].estimatedCost) < (data[b].estimatedCost);
        };
    }

    const auto solid = [&map, &registry] (const TilePosition &pos) {
        return isSolid(map.getTileType(pos.x, pos.y, registry));
    };

    std::vector<TilePosition> path;
    std::set<TilePosition, decltype(sortFunction)> openList(sortFunction);
    std::set<TilePosition> closedList;
    std::map<TilePosition, TilePosition> pred;

    auto startTileData = TileData{start, 0, heuristicDistance(start, end)};
    data[start] = startTileData;
    openList.insert(start);

    while (!openList.empty()) {
        auto current = *openList.begin();

        // If the end is found
        if(current == end) { break; }

        closedList.insert(current);
        openList.erase(*openList.begin());

        auto newStartDist = data[current].startDist + 1;
        TilePosition neighbors[4] = {current.neighbor(LEFT), current.neighbor(UP), current.neighbor(RIGHT), current.neighbor(DOWN)};

        for(auto &neighbor : neighbors) {
            // Der Knoten wurde schon bearbeitet
            if(closedList.find(neighbor) == closedList.end()) continue;

            auto neighborEstimatedDistance = newStartDist + heuristicDistance(neighbor, end);

            // Falls der Nachbar schon in der open List ist, und der neu berechnete Weg vom Start zu diesem Nachbar nicht kürzer ist,
            // als der schon gespeicherte weg, überspringe
            if(openList.find(neighbor) != openList.end() && newStartDist >= data[neighbor].startDist) continue;


            auto newTileData = TileData{neighbor, newStartDist, neighborEstimatedDistance};
            data[neighbor] = newTileData;
            openList.insert(neighbor);
            pred[neighbor] = current;

            if(openList.find(neighbor) == openList.end()) {
                openList.insert(neighbor);
            } else {
                openList.erase(neighbor);
                openList.insert(neighbor);
            }
        }
    }

    std::cout << data[end].startDist << std::endl;

    return std::vector<TilePosition>();
}*/
