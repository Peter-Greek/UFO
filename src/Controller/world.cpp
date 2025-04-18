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
    print("Loading World", worldData.get().dump(4));
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

// Returns a random available door from the placed rooms
//json* pickAvailableDoor(std::vector<json>& doors) {
//    std::vector<json*> available;
//    for (auto& door : doors) {
//        if (!door["isTaken"].get<bool>()) {
//            available.push_back(&door);
//        }
//    }
//
//    if (available.empty()) return nullptr;
//
//    return available[random(0, (int)available.size() - 1)];
//}

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

bool isHallway(const json& room) {
    return room["type"] == "hall";
}

/*
bool world::isRoomTypeUsed(const std::string& type) {
    return usedRoomTypes.count(type) > 0;
}

bool world::isRoomTypeRequired(const std::string& type) {
    return std::find(requiredRoomTypes.begin(), requiredRoomTypes.end(), type) != requiredRoomTypes.end();
}

json world::pickNextRoomTemplate(const json& roomTemplates, int placedCount, int maxCount) {
    std::vector<json> options;

    for (const auto& room : roomTemplates["rooms"]) {
        std::string type = room["type"];
        if (placedCount < maxCount - requiredRoomTypes.size()) {
            if (isHallway(room) || !isRoomTypeUsed(type) || !isRoomTypeRequired(type)) {
                options.push_back(room);
            }
        } else {
            if (!isRoomTypeUsed(type) && isRoomTypeRequired(type)) {
                options.push_back(room);
            }
        }
    }

    if (options.empty()) return roomTemplates["rooms"][0]; // fallback
    return options[random(0, static_cast<int>(options.size()) - 1)];
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
    usedRoomTypes.clear();
    roomTypeCount.clear();
    requiredRoomTypes = {"lab", "storage", "generator"}; // or load dynamically

    std::vector<json> placedDoors;
    std::vector<json> placedRooms;

    for (int i = 0; i < count; ++i) {
        json room;

        if (i == 0) {
            room = roomTemplates["rooms"][0];
            if (room["id"] != "sp1") error("First room in JSON must be spawn room (id 'sp1')");

            vector2 spawnPoint = worldData["spawnPoint"].get<vector2>();
            vector2 roomCenter = room["center"].get<vector2>();
            vector2 offset = spawnPoint - roomCenter;

            for (auto& w : room["walls"]) w["coords"] = w["coords"].get<vector2>() + offset;
            for (auto& d : room["doors"]) {
                d["coords"] = d["coords"].get<vector2>() + offset;
                d["isTaken"] = false;
                placedDoors.push_back(d);
            }
            if (room.contains("entities")) {
                for (auto& e : room["entities"])
                    e["coords"] = e["coords"].get<vector2>() + offset;
            }

            room["center"] = spawnPoint;
            usedRoomTypes.insert(room["type"]);
            roomTypeCount[room["type"]]++;
            placedRooms.push_back(room);
        } else {
            json* openDoor = pickAvailableDoor(placedDoors);
            if (!openDoor) break;

            json templateRoom = pickNextRoomTemplate(roomTemplates, i, count);
            json newRoom = templateRoom;
            json* newDoor = findUnusedDoor(newRoom);
            if (!newDoor) continue;

            for (auto& d : newRoom["doors"]) d["isTaken"] = false;

            int openHeading = openDoor->at("h").get<int>();
            int newHeading  = newDoor->at("h").get<int>();
            float desiredHeading = fmod((openHeading + 180), 360);
            float rotationAmount = fmod((desiredHeading - newHeading + 360), 360);
            vector2 center = newRoom["center"].get<vector2>();

            for (auto& w : newRoom["walls"]) {
                w["coords"] = rotatePoint(w["coords"].get<vector2>(), center, rotationAmount);
                w["h"] = static_cast<int>(fmod(w["h"].get<int>() + rotationAmount, 360));
            }
            for (auto& d : newRoom["doors"]) {
                d["coords"] = rotatePoint(d["coords"].get<vector2>(), center, rotationAmount);
                d["h"] = static_cast<int>(fmod(d["h"].get<int>() + rotationAmount, 360));
            }
            if (newRoom.contains("entities")) {
                for (auto& e : newRoom["entities"]) {
                    e["coords"] = rotatePoint(e["coords"].get<vector2>(), center, rotationAmount);
                }
            }

            newRoom["center"] = rotatePoint(center, center, rotationAmount);

            vector2 openCenter = getDoorCenter(*openDoor);
            vector2 newCenter  = getDoorCenter(*newDoor);
            vector2 offset = openCenter - newCenter;

            for (auto& w : newRoom["walls"]) w["coords"] = w["coords"].get<vector2>() + offset;
            for (auto& d : newRoom["doors"]) d["coords"] = d["coords"].get<vector2>() + offset;
            if (newRoom.contains("entities")) {
                for (auto& e : newRoom["entities"])
                    e["coords"] = e["coords"].get<vector2>() + offset;
            }
            newRoom["center"] = newRoom["center"].get<vector2>() + offset;

            openDoor->at("isTaken") = true;
            newDoor->at("isTaken") = true;
            for (auto& d : newRoom["doors"]) {
                if (!d["isTaken"].get<bool>()) placedDoors.push_back(d);
            }

            usedRoomTypes.insert(newRoom["type"]);
            roomTypeCount[newRoom["type"]]++;
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


*/


json world::getRoomById(const json& templates, const std::string& id) {
    for (const auto& room : templates["rooms"]) {
        if (room["id"] == id) return room;
    }
    error("Room ID not found: " + id);
    return templates["rooms"][0];
}

void world::offsetRoom(json& room, const vector2& offset) {
    for (auto& w : room["walls"]) w["coords"] = w["coords"].get<vector2>() + offset;
    for (auto& d : room["doors"]) d["coords"] = d["coords"].get<vector2>() + offset;
    if (room.contains("entities")) {
        for (auto& e : room["entities"]) e["coords"] = e["coords"].get<vector2>() + offset;
    }
    room["center"] = room["center"].get<vector2>() + offset;
}

void world::rotateRoom(json& room, float angle) {
    vector2 center = room["center"].get<vector2>();
    for (auto& w : room["walls"]) {
        w["coords"] = rotatePoint(w["coords"].get<vector2>(), center, angle);
        w["h"] = static_cast<int>(fmod(w["h"].get<int>() + angle, 360));
    }
    for (auto& d : room["doors"]) {
        d["coords"] = rotatePoint(d["coords"].get<vector2>(), center, angle);
        d["h"] = static_cast<int>(fmod(d["h"].get<int>() + angle, 360));
    }
    if (room.contains("entities")) {
        for (auto& e : room["entities"]) {
            e["coords"] = rotatePoint(e["coords"].get<vector2>(), center, angle);
        }
    }
    room["center"] = rotatePoint(center, center, angle);
}

void world::markDoors(json& room, std::vector<json>& doorPool, json* skip) {
    for (auto& d : room["doors"]) {
        if (&d == skip) {
            d["isTaken"] = true;
        } else {
            d["isTaken"] = false;
            doorPool.push_back(d);
        }
    }
}

float world::getRequiredRotation(const json& openDoor, const json& newDoor) {
    int openH = openDoor["h"].get<int>();
    int newH = newDoor["h"].get<int>();
    float desiredHeading = fmod((openH + 180), 360);
    return fmod((desiredHeading - newH + 360), 360);
}

bool world::shouldAddHallway(int specialPlaced, int maxCount, int placed, const std::string& lastType) {
    if (specialPlaced < requiredRoomTypes.size()) return true;
    if (placed < maxCount - 1) return true;
    return false;
}

bool world::shouldAddSpecialRoom(int specialPlaced) {
    return specialPlaced < requiredRoomTypes.size();
}

json world::pickHallway(const json& templates) {
    std::vector<json> halls;
    for (const auto& r : templates["rooms"]) {
        if (r["type"] == "hall") halls.push_back(r);
    }
    return halls[random(0, (int)halls.size() - 1)];
}

json world::pickUnusedSpecialRoom(const json& templates) {
    std::vector<json> specials;
    for (const auto& r : templates["rooms"]) {
        if (r.contains("type") && r["type"] != "hall" && usedRoomTypes.find(r["type"]) == usedRoomTypes.end()) {
            specials.push_back(r);
        }
    }
    if (specials.size() == 0) {
        return pickUsedSpecialRoom(templates);
    }
    return specials[random(0, (int)specials.size() - 1)];
}

std::string world::getConnectingRoomType(const json& door) {
    for (const auto& r : worldData["rooms"]) {
        for (const auto& d : r["doors"]) {
            if (d == door) return r["type"];
        }
    }
    return "hall";
}

json* world::pickAvailableDoor(std::vector<json>& doors) {
    std::vector<json*> available;
    for (auto& d : doors) {
        if (!d["isTaken"].get<bool>()) available.push_back(&d);
    }
    json* j = nullptr;
    if (available.empty()) return j;
    int i = random(0, (int)available.size() - 1);
    return available[i];
}

json* world::findUnusedDoor(json& room) {
    for (auto& d : room["doors"]) {
        if (!d.contains("isTaken") || !d["isTaken"].get<bool>()) return &d;
    }
    return nullptr;
}

// Assumes: vector2, rotatePoint, getDoorCenter, Heading, angleToVector2 are implemented

// Assumes: vector2, rotatePoint, getDoorCenter, Heading, angleToVector2 are implemented

void world::generateLayout(int count) {
    json roomTemplates;
    std::ifstream file("../resource/json/world.json");
    if (!file.is_open()) {
        error("Failed to open world template file.");
        return;
    }
    file >> roomTemplates;

    wallList.clear();
    roomList.clear();
    worldData["rooms"].clear();
    usedRoomTypes.clear();
    roomTypeCount.clear();
    requiredRoomTypes.clear();

    // Dynamically build list of required room types from JSON (all non-hallway types)
    std::unordered_set<std::string> uniqueTypes;
    for (auto& room : roomTemplates["rooms"]){
        std::string type = room["type"];
        if (type != "hall" && uniqueTypes.insert(type).second) {
            requiredRoomTypes.push_back(type);
        }
        for (int i = 0; i < room["doors"].size(); i++) {
            room["doors"][i]["id"] = room["id"].get<std::string>() + "_" + std::to_string(i);
        }
    }

    std::vector<json> placedDoors;
    std::vector<json> placedRooms;
    int specialRoomsPlaced = 0;

    json lastHallway;
    std::vector<json> lastHallwayDoors;

    for (int i = 0; i < count;) {
        json room;

        if (i == 0) {
            room = getRoomById(roomTemplates, "sp1");
            vector2 spawnPoint = worldData["spawnPoint"].get<vector2>();
            vector2 offset = spawnPoint - room["center"].get<vector2>();
            offsetRoom(room, offset);
            markDoors(room, placedDoors);
            room["center"] = spawnPoint;
            usedRoomTypes.insert(room["type"]);
            placedRooms.push_back(room);
            worldData["rooms"].push_back(room);
            i++;
            continue;
        }

        json* openDoor = pickAvailableDoor(placedDoors);
        if (!openDoor) break;
        print(openDoor, *openDoor);


        bool attachNewHallway = false;
        if (!lastHallwayDoors.empty()) {
            print("Before error");
            if ((*openDoor)["id"] == (lastHallwayDoors.back())["id"])
            {
                print("New Hallway", i);
                attachNewHallway = true;
            }
        }else {
            attachNewHallway = true;
        }

        print("after error");

        json nextRoom;
        if (attachNewHallway) {
            nextRoom = pickHallway(roomTemplates);
        } else {
            if (specialRoomsPlaced < requiredRoomTypes.size()) {
                nextRoom = pickUnusedSpecialRoom(roomTemplates);
                specialRoomsPlaced++;
                if (!nextRoom.contains("walls")) {
                    nextRoom = pickUsedSpecialRoom(roomTemplates);
                }
            } else {
                nextRoom = pickUsedSpecialRoom(roomTemplates);
            }
        }

        json* nextDoor = findUnusedDoor(nextRoom);
        if (!nextDoor) continue;

        float rotationAmount = getRequiredRotation(*openDoor, *nextDoor);
        rotateRoom(nextRoom, rotationAmount);

        vector2 offset = getDoorCenter(*openDoor) - getDoorCenter(*nextDoor);
        offsetRoom(nextRoom, offset);

        markDoors(nextRoom, placedDoors, nextDoor);
        usedRoomTypes.insert(nextRoom["type"]);
        roomTypeCount[nextRoom["type"]]++;
        worldData["rooms"].push_back(nextRoom);
        placedRooms.push_back(nextRoom);

        if (nextRoom["type"] == "hall") {
            lastHallway = nextRoom;
            lastHallwayDoors.clear();
            for (auto& d : nextRoom["doors"]) {
                if (!d["isTaken"].get<bool>()) {
                    lastHallwayDoors.push_back(d);
                }
            }
        }

        i++;
    }

    for (auto& room : worldData["rooms"]) {
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
    }
}

json world::pickUsedSpecialRoom(const json& templates) {

    std::vector<json> options;
    for (const auto& r : templates["rooms"]) {
        if (r["type"] != "hall") {
            options.push_back(r);
        }
    }
    return options[random(0, (int)options.size() - 1)];
}


