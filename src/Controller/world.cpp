//
// Created by xerxe on 3/6/2025.
//

#include "world.h"

int world::initialize() {

    AddEventHandler("SDL::OnUpdate", [this](float deltaMs) {
        if (!gameRunning) {return;}
    });

    gameRunning = true;
    return 1;
}

void world::update(float deltaMs) {

}

void world::loadWorld() {
    worldData.update();
    wallList.clear();
    if (worldData["rooms"].empty() == 0) {
        for (auto& room : worldData["rooms"]) {
            roomList.push_back(wallList_t());
            for (auto& rWall : room["walls"]) {
                wallList.push_back(new wall(rWall["coords"].get<vector2>(), rWall["l"].get<int>(), rWall["w"].get<int>(), rWall["h"].get<int>()));
                roomList.back().push_back(wallList.back());
            }
        }
    }
}

void world::saveWorld() {
    worldData.save();
}

jsonLoader world::getWorldData() {
    return worldData;
}

wallList_t world::getWallList() {
    return wallList;
}

roomList_t world::getRoomList() {
    return roomList;
}

bool world::isPointInWall(vector2 vec) {
    for (auto& w : wallList) {
        if (w->isPointInWall(vec)) {
            return true;
        }
    }
    return false;
}
