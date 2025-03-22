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
    print("Saving World", worldData.get().dump(4));

    worldData.save();
}

jLoader& world::getWorldData() {
    return worldData;
}

void world::updateWall(int roomId, int wallId, wall* w) {
    auto roomIt = std::next(roomList.begin(), roomId);
    auto wallIt = std::next(roomIt->begin(), wallId);
    *wallIt = w;
}

int world::addRoom() {
    roomList.push_back(wallList_t());
    worldData["rooms"].push_back(json::object());
    worldData["rooms"].back()["walls"] = json::array();
    saveWorld();
    return worldData["rooms"].size() - 1;
}

int world::addWall(int roomId, wall* w) {
    auto roomIt = std::next(roomList.begin(), roomId);
    roomIt->push_back(w);
    worldData["rooms"][roomId]["walls"].push_back(w);
    saveWorld();
    return worldData["rooms"][roomId]["walls"].size() - 1;
}

void world::deleteRoom(int roomId) {
    auto roomIt = std::next(roomList.begin(), roomId);
    roomList.erase(roomIt);
    worldData["rooms"].erase(worldData["rooms"].begin() + roomId);
    saveWorld();
}

void world::deleteWall(int roomId, int wallId) {
    auto roomIt = std::next(roomList.begin(), roomId);
    auto wallIt = std::next(roomIt->begin(), wallId);
    roomIt->erase(wallIt);
    worldData["rooms"][roomId]["walls"].erase(worldData["rooms"][roomId]["walls"].begin() + wallId);
    if (worldData["rooms"][roomId]["walls"].empty()) {
        deleteRoom(roomId);
    }
    saveWorld();
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
