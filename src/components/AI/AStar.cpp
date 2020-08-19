//
// Created by skadic on 24.06.20.
//

#include "AStar.h"

#include <cmath>
#include <map>
#include <queue>
#include <set>
#include <iostream>

/**
 * Calculates the euclidian distance (in the unit "tiles") between two TilePosition objects. This is used as the heuristic for the A* algorithm
 * @param a A TilePosition
 * @param b A TilePosition
 * @return The euclidian distance between the two TilePositions
 */
double heuristicDistance(TilePosition a, TilePosition b) {
    auto xDist = a.x - b.x;
    auto yDist = a.y - b.y;
    return sqrt(xDist * xDist + yDist * yDist);
}

/**
 * This is used to store additional data besides the mere position.
 * Used for the A* algorithm only
 */
struct TileData {
    /**
     * The tile position this data object is associated with
     */
    TilePosition pos{};

    /**
     * The already calculated exact length of the shortest path from the start tile to this tile
     */
    double g{};

    /**
     * The estimated length of the path from this tile to the end tile, calculated with the heuristic distance function
     */
    double h{};

    /**
     * The previous tile in the path from the start to this tile
     * This can be used to easily backtrack from the end to the start in order to create a path
     */
    TileData *pred;

    TileData(const TilePosition &pos, TileData *pred = nullptr) : pos(pos), pred(pred) {}

    /**
     * Gets the score required by the algorithm. This represents the estimated length of the shortest path from start to end,
     * that uses this tile. It consists of the length of the shortest path from the start to this tile (g), and the estimated
     * length of the remaining path (h), simply summed up.
     */
    double getScore() {
        return g + h;
    }
};

/**
 * An ordering for TileData objects, so that they can be used in ordered sets, to be used as a priority queue.
 * Since we always want to pick the TileData objects with the lowest score (as these appear to deliver the shortest path to the goal),
 * the main important part is that the prioritised value by which the objects are ordered, is the score.
 *
 * The other two conditions (sorting by x and y coordinate respectively) are entirely artificial and only exist here,
 * in order to make two TileData objects with the same score, but different positions, also seem distinct to the set, in which they are stored.
 * If we were not to do this, the set would consider two objects with the same score (but potentially different position) as the same.
 * And since duplicate objects are disallowed in sets, it would not insert the new value.
 * But obviously, we would want to insert them, if the positions are unequal.
 */
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

/**
 * Finds a TileData associated with a position in a set
 * @param pos The position to search for
 * @param s The set to search in
 * @return A pointer to the TileData object. nullptr if it doesn't exist
 */
TileData* findData(TilePosition pos, std::set<TileData*, ScoreCompare> &s) {
    for(auto &ptr : s) {
        if(ptr->pos == pos) {
            return ptr;
        }
    }
    return nullptr;
}

/**
 * Deletes all the data that the pointers in the set point to
 * @param set The set to erase
 */
void dealloc(std::set<TileData*, ScoreCompare> set) {
    for (auto data = set.begin(); data != set.end();) {
        delete *data;
        data = set.erase(data);
    }
}

/**
 * Checks whether the position is outside of the map
 * @param pos The position to check
 * @param map The map whose bounds to check
 * @return true, if the position is outside of the map bounds, false otherwise
 */
bool outOfRange(TilePosition &pos, Map &map) {
    return
        std::clamp((int) pos.x, 0, map.getWidth() - 1) != pos.x ||
        std::clamp((int) pos.y, 0, map.getHeight() - 1) != pos.y;
}

// A* Pathfinding algorithm
// Code inspired by https://github.com/daancode/a-star
std::vector<TilePosition> AStar::pathfind(TilePosition start, TilePosition end, Map &map, entt::registry &registry) {
    // currently handled tile
    TileData *current = nullptr;

    // Set of known, but not fully explored tiles
    std::set<TileData*, ScoreCompare> openSet {};

    // Set of known and fully explored tiles
    std::set<TileData*, ScoreCompare> closedSet {};

    // We want to begin with the start tile. It is known but obviously not explored yet
    openSet.insert(new TileData(start));

    // If there are no more tiles to explore, the algorithm must terminate
    // If the algorithm breaks because of this condition, there is no path to the end
    while (!openSet.empty()) {

        current = *openSet.begin();

        // If we found the end, we can stop, since we have found a path
        if(current->pos == end) { break; }

        // After this iteration of the while loop we will have fully explored this tile, so we can add it to the closed set
        // and remove it from the open set now.
        closedSet.insert(current);
        openSet.erase(openSet.find(current));

        // Iterate through all the directions. Since these are an enum, we can iterate through them using an integer
        // and for loop
        for (int i = 0; i < LAST_DIRECTION; ++i) {
            // Get the neighboring tile in the specified direction
            TilePosition newPos = current->pos.neighbor((Direction) i);
            /**
             * There are multiple reasons why we would want to skip a neighboring tile:
             * 1. If the tile is solid, we cannot move there and therefore the path cannot use that tile
             * 2. If the tile is in the closed set, we already have fully explored it, and need not do so again
             * 3. If the tile is out of range of the map we don't want to consider it. If there was no path to the goal,
             * the algorithm would keep searching infinitely outside of the map, so we restrict the search space.
             */
            if(isSolid(map.getTileTypeAt(newPos.x, newPos.y, registry)) || findData(newPos, closedSet) || outOfRange(newPos, map)) {
                continue;
            }

            // Since the tile at newPos is a neighboring tile of current, and since the g member is the shortest path to
            // the respective tile, the length of the path to the tile at newPos is just 1 tile longer than to current.
            auto totalDistance = current->g + 1;

            /**
             * Check if we have already seen (but not fully explored!) the tile at newPos yet, by searching for it in the open set.
             * If so, we check whether the distance of the path to this tile is more optimal than the path fould earlier.
             * If that is the case, we remove the tile from the open set, update the data accordingly, and then re-insert it.
             * This is to make sure, that the position of the TileData object in the set is corrected, since the score changed
             * (remember, the score is the primary value by which the TileData objects are ordered in the set)
             *
             * If the object is not found in the open set, create a new TileData object with the given data, and insert
             * it into the open set.
             */
            if(TileData *newTile = findData(newPos, openSet)) {
                if(totalDistance < newTile->g) {
                    openSet.erase(newTile);
                    newTile->pred = current;
                    newTile->g = totalDistance;
                    openSet.insert(newTile);
                }
            } else {
                newTile = new TileData(newPos, current);
                newTile->g = totalDistance;
                newTile->h = heuristicDistance(newPos, end);
                openSet.insert(newTile);
            }
        }
    }

    // Return empty path, if the end wasn't reached
    if(current->pos != end) {
        return std::vector<TilePosition>();
    }

    // Otherwise, backtrack through the predecessors stored in the TileData objects
    std::vector<TilePosition> path;
    while (current != nullptr) {
        path.push_back(current->pos);
        current = current->pred;
    }
    // Since we extract the path in reverse order, we need to reverse the path
    std::reverse(path.begin(), path.end());

    // Delete the data in both sets.
    dealloc(openSet);
    dealloc(closedSet);

    return path;
}
