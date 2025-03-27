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

bool world::isRectInWall(vectorList_t& rect) {
    for (auto& w : wallList) {
        if (w->isRectangleInWall(rect)) {
            return true;
        }
    }
    return false;
}

vector2 world::getSpawnPoint() {
    return worldData["spawnPoint"].get<vector2>();
}

// Returns a random available door from the placed rooms
json* pickAvailableDoor(std::vector<json>& doors) {
    std::vector<json*> available;
    for (auto& door : doors) {
        if (!door["isTaken"].get<bool>()) {
            available.push_back(&door);
        }
    }

    if (available.empty()) return nullptr;

    return available[random(0, (int)available.size() - 1)];
}

// Returns a room template that has at least one unused door
json pickRoomWithUnusedDoor(const json& roomTemplates) {
    std::vector<json> validRooms;
    for (auto& room : roomTemplates["rooms"]) {
        if (!room.contains("doors")) continue;
        for (auto& door : room["doors"]) {
            if (!door.contains("isTaken") || door["isTaken"] == false) {
                validRooms.push_back(room);
                break;
            }
        }
    }

    if (validRooms.empty()) {
        error("No valid room with unused doors available.");
    }

    return validRooms[random(0, (int)validRooms.size() - 1)];
}

// Returns a pointer to the first unused door in the room
json* findUnusedDoor(json& room) {
    for (auto& door : room["doors"]) {
        if (!door.contains("isTaken") || door["isTaken"] == false) {
            return &door;
        }
    }
    return nullptr;
}

vector2 rotatePoint(vector2 point, vector2 center, float angleDegrees) {
    float angleRad = angleDegrees * (M_PI / 180.0f);
    float s = sin(angleRad);
    float c = cos(angleRad);
    point -= center;
    float newX = point.x * c - point.y * s;
    float newY = point.x * s + point.y * c;
    return {newX + center.x, newY + center.y};
}

vector2 getDoorCenter(const json& door) {
    vector2 base = door["coords"].get<vector2>();
    float heading = door["h"].get<float>();
    float length = door["l"].get<float>();
    vector2 dir = angleToVector2(Heading(heading)).normalize();
    return base + dir * (length / 2.0f);
}


void world::generateLayout(int count) {
    json roomTemplates;
    std::ifstream file("../resource/world.json");
    if (!file.is_open()) {
        error("Failed to open world template file.");
        return;
    }
    file >> roomTemplates;

    wallList.clear();
    roomList.clear();
    worldData["rooms"].clear();

    std::vector<json> placedDoors;
    std::vector<json> placedRooms;

    for (int i = 0; i < count; ++i) {
        json room;

        if (i == 0) {
            room = roomTemplates["rooms"][0];
            vector2 spawnPoint = worldData["spawnPoint"].get<vector2>();
            vector2 roomCenter = room["center"].get<vector2>();
            vector2 offset = spawnPoint - roomCenter;

            // shift walls
            for (auto& w : room["walls"]) {
                w["coords"] = w["coords"].get<vector2>() + offset;
            }

            // shift doors
            for (auto& d : room["doors"]) {
                d["coords"] = d["coords"].get<vector2>() + offset;
                d["isTaken"] = false;
                placedDoors.push_back(d);
            }

            // shift entities
            if (room.contains("entities")) {
                for (auto& e : room["entities"]) {
                    e["coords"] = e["coords"].get<vector2>() + offset;
                }
            }

            room["center"] = spawnPoint;
            placedRooms.push_back(room);
        } else {
            json* openDoor = pickAvailableDoor(placedDoors);
            if (!openDoor) break;

            json templateRoom = pickRoomWithUnusedDoor(roomTemplates);
            json newRoom = templateRoom;
            json* newDoor = findUnusedDoor(newRoom);
            if (!newDoor) continue;

            for (auto& d : newRoom["doors"]) {
                d["isTaken"] = false;
            }

            int openHeading = openDoor->at("h").get<int>();
            int newHeading  = newDoor->at("h").get<int>();
            float desiredHeading = fmod((openHeading + 180), 360);
            float rotationAmount = fmod((desiredHeading - newHeading + 360), 360);

            vector2 newRoomCenter = newRoom["center"].get<vector2>();

            // rotate walls
            for (auto& w : newRoom["walls"]) {
                vector2 coord = w["coords"].get<vector2>();
                w["coords"] = rotatePoint(coord, newRoomCenter, rotationAmount);
                w["h"] = static_cast<int>(fmod(w["h"].get<int>() + rotationAmount, 360));
            }

            // rotate doors
            for (auto& d : newRoom["doors"]) {
                vector2 coord = d["coords"].get<vector2>();
                d["coords"] = rotatePoint(coord, newRoomCenter, rotationAmount);
                d["h"] = static_cast<int>(fmod(d["h"].get<int>() + rotationAmount, 360));
            }

            // rotate entities
            if (newRoom.contains("entities")) {
                for (auto& e : newRoom["entities"]) {
                    vector2 coord = e["coords"].get<vector2>();
                    e["coords"] = rotatePoint(coord, newRoomCenter, rotationAmount);
                }
            }

            newRoom["center"] = rotatePoint(newRoomCenter, newRoomCenter, rotationAmount);

            // door alignment
            vector2 openDoorCenter = getDoorCenter(*openDoor);
            vector2 newDoorCenter  = getDoorCenter(*newDoor);
            vector2 offset = openDoorCenter - newDoorCenter;

            for (auto& w : newRoom["walls"]) {
                w["coords"] = w["coords"].get<vector2>() + offset;
            }
            for (auto& d : newRoom["doors"]) {
                d["coords"] = d["coords"].get<vector2>() + offset;
            }
            if (newRoom.contains("entities")) {
                for (auto& e : newRoom["entities"]) {
                    e["coords"] = e["coords"].get<vector2>() + offset;
                }
            }
            newRoom["center"] = newRoom["center"].get<vector2>() + offset;

            openDoor->at("isTaken") = true;
            newDoor->at("isTaken") = true;

            for (auto& d : newRoom["doors"]) {
                if (!d["isTaken"].get<bool>()) {
                    placedDoors.push_back(d);
                }
            }

            placedRooms.push_back(newRoom);
            room = newRoom;
        }

        wallList_t newWallList;
        for (auto& wallJson : room["walls"]) {
            auto* wallPtr = new wall(
                    wallJson["coords"].get<vector2>(),
                    wallJson["l"].get<int>(),
                    wallJson["w"].get<int>(),
                    wallJson["h"].get<int>()
            );
            wallList.push_back(wallPtr);
            newWallList.push_back(wallPtr);
        }

        roomList.push_back(newWallList);
        worldData["rooms"].push_back(room);
    }
}


json world::getAllEntities() {
    json entities = json::array();
    if (worldData["rooms"].empty() == 0) {
        for (auto &room: worldData["rooms"]) {
            for (auto &rEntity: room["entities"]) {
                entities.push_back(rEntity);
            }
        }
    }
    return entities;
}





