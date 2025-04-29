#include "World.h"
#include <fstream>
#include <cstdint>

bool readPNGSize(const std::string& filePath, int& width, int& height) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    file.seekg(16); // skip to width/height
    uint8_t buf[8];
    file.read(reinterpret_cast<char*>(buf), 8);

    width = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
    height = (buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7];

    file.close();
    return true;
}

World::World(passFunc_t func) : xProcess(false, func), worldData(worldPath) {
    print("World Constructor");
}

int World::initialize() {
    gameRunning = true;
    return 1;
}

void World::update(float deltaMs) {}

wallList_t World::getWallList() {
    return wallList;
}

roomList_t World::getRoomList() {
    return roomList;
}

json World::getAllEntities() {
    return worldData.get()["entities"];
}

jLoader& World::getWorldData() {
    return worldData;
}

vector2 World::getSpawnPoint() {
    return worldData.get()["spawnPoint"].get<vector2>();
}

RoomData World::parseRoomFromJson(const json& roomJson) {
    RoomData room;

    print("Parsing new room JSON...");

    if (!roomJson.contains("id")) {
        error("Room JSON missing 'id'", roomJson.dump());
    }
    room.id = roomJson["id"].get<std::string>();
    print("Parsed id:", room.id);

    if (!roomJson.contains("name")) {
        error("Room JSON missing 'name'", roomJson.dump());
    }
    room.name = roomJson["name"].get<std::string>();
    print("Parsed name:", room.name);

    if (!roomJson.contains("type")) {
        error("Room JSON missing 'type'", roomJson.dump());
    }
    room.type = roomJson["type"].get<std::string>();
    print("Parsed type:", room.type);

    if (!roomJson.contains("center")) {
        error("Room JSON missing 'center'", roomJson.dump());
    }
    room.center = roomJson["center"].get<vector2>();
    print("Parsed center:", room.center.x, room.center.y);

    room.imagePath = imgPath + room.id + ".png";
    if (roomJson.contains("bounds")) {
        print("Room has bounds...");
        room.bounds = BoundsData{
                .position = { roomJson["bounds"]["X"].get<float>(), roomJson["bounds"]["Y"].get<float>() },
                .size = { roomJson["bounds"]["W"].get<float>(), roomJson["bounds"]["H"].get<float>() }
        };
        room.valid_zone = room.bounds;
    }


    // open image
    std::ifstream file(room.imagePath);
    if (!file.is_open()) {
        error("Failed to open room image", room.imagePath);
    }

    // get image size
    int width, height;
    if (readPNGSize(room.imagePath, width, height)) {
        room.bounds = BoundsData{
                .position = { 0, 0 },
                .size = { (float)width, (float)height }
        };
    } else {
        error("Failed to read PNG size:", room.imagePath);
    }

    file.close();




    if (!roomJson.contains("walls") || !roomJson["walls"].is_array()) {
        error("Room JSON missing or invalid walls array", roomJson.dump());
    }
    print("Parsing walls...");

    for (const auto& w : roomJson["walls"]) {
        if (!w.contains("coords") || !w.contains("l") || !w.contains("w") || !w.contains("h")) {
            error("Wall JSON missing fields", roomJson.dump());
        }
        WallData wall;
        wall.coords = w["coords"].get<vector2>();
        wall.length = w["l"].get<int>();
        wall.width = w["w"].get<int>();
        wall.heading = w["h"].get<int>();
        room.walls.push_back(wall);
    }
    print("Parsed", (int)room.walls.size(), "walls.");

    if (!roomJson.contains("doors") || !roomJson["doors"].is_array()) {
        error("Room JSON missing or invalid doors array", roomJson.dump());
    }
    print("Parsing doors...");

    int doorId = 0;
    for (const auto& d : roomJson["doors"]) {
        doorId++;
        if (!d.contains("coords") || !d.contains("h") || !d.contains("l") || !d.contains("w") || !d.contains("closed") || !d.contains("locked")) {
            error("Door JSON missing fields");
        }
        DoorData door;
        door.id = roomJson["id"].get<std::string>() + "_" + std::to_string(doorId);
        door.attached = false;
        door.coords = d["coords"].get<vector2>();
        door.heading = static_cast<Heading>(d["h"].get<int>());
        door.length = d["l"].get<int>();
        door.width = d["w"].get<int>();
        door.closed = d["closed"].get<bool>();
        door.locked = d["locked"].get<bool>();
        room.doors.push_back(door);
    }
    print("Parsed", (int)room.doors.size(), "doors.");




    print("Finished parsing room:", room.id);

    return room;
}

bool World::isRoomRequired(const std::string& roomId) {
    for (auto& r : roomTypes) {
        if (r.name == roomId) {
            return r.required;
        }
    }
    return false;
}

bool World::generateWorld(int roomCount) {
    print("Generating world with", roomCount, "rooms.");
    if (roomCount < minRooms) {
        error("generateWorld: too few rooms requested.");
        return false;
    }

    wallList.clear();
    roomList.clear();
    worldData.get()["rooms"].clear();
    print("Cleared world data.");

    json templateJson;
    std::ifstream file(worldPath);
    if (!file.is_open()) {
        error("Failed to open world.json");
        return false;
    }
    file >> templateJson;
    file.close();
    print("Loaded world.json");

    roomTemplates.clear();
    for (auto& r : templateJson["rooms"]) {
        roomTemplates.push_back(parseRoomFromJson(r));
    }
    print("Parsed", (int)roomTemplates.size(), "room templates.");

    if (!placeStartRoom()) {
        error("Failed to place start room.");
        return false;
    }else {
        print("Placed start room.");
    }

    if (1) return true; // debug purpose

//    int placed = 1; // start room counts as 1
//    int tot = 1;
//    while (placed < roomCount) {
//
//        int lastIndex = (int) roomList.size() - 1;
//        RoomData* lastRoom = roomList[lastIndex]; // last room in the list
//        nextRoomData_t res_data = chooseNextRoom(lastRoom, roomCount - placed);
//
//        if (res_data.first.first == nullptr) {
//            print("ERROR: chooseNextRoom returned nullptr, stopping generation.");
//            return false;
//        }
//
//
//        RoomData* nextRoom = placeRoom(res_data.first.second); // place the new room template
//        attachRooms(res_data.first.first, nextRoom, res_data.second);
//
//        if (doesRoomOverlap(nextRoom)) {
//            print("Room overlaps, removing it.");
//
//            if (isDebug()) {
//                if (seed != -1) {
//                    seed++;
//                    srand(seed);
//                }
//            }
//
//            for (auto& d : nextRoom->real_doors) {
//                if (d.attached) {
//                    d.attached = false;
//
//                    auto* door = d.targetDoor;
//                    if (door != nullptr) {
//                        door->attached = false;
//                        door->targetDoor = nullptr;
//                    }
//
//                    d.targetDoor = nullptr;
//                }
//            }
//
//            roomList.pop_back();
//            continue;
//        }
//
//
//        tot += 1;
////        if (tot > 12) break;
//        if (nextRoom->type == "room") { placed++; }
////        break; // debug purpose
//    }

    finalizeWorld();
//    worldData.save();
    return true;
}

bool World::placeStartRoom() {
    for (auto& r : roomTemplates) {
        if (r.id == "fullMap1") {
            placeRoom(r);
            return true;
        }
    }
    return false;
}

nextRoomData_t World::chooseNextRoom(RoomData* lastRoom, int roomsLeft) {
    RoomData nextRoom; // TEMP copy for returning
    std::string doorAttachId;

    print("Choosing next room from", lastRoom->id, "of type", lastRoom->type);

    if (lastRoom->type == "room") {
        print("Last room is a room, checking doors.");
        int doorCount = lastRoom->real_doors.size();
        if (doorCount > 1) {
            int startDoor = rand() % doorCount;
            print("Start door:", startDoor);

            for (int i = 0; i < doorCount; ++i) {
                int doorIndex = (startDoor + i) % doorCount;
                DoorData& d = lastRoom->real_doors[doorIndex];
                print("Checking door:", d.id, "heading:", d.heading);
                if (d.attached) continue;

                if (d.heading == 90 || d.heading == 270) {
                    for (auto& r : roomTemplates) {
                        if (r.id == "verticalHallway") {
                            nextRoom = r;
                            doorAttachId = d.id;
                            break;
                        }
                    }
                    break;
                } else {
                    for (auto& r : roomTemplates) {
                        if (r.id == "horizontalHallway") {
                            nextRoom = r;
                            doorAttachId = d.id;
                            break;
                        }
                    }
                    break;
                }
            }
        }else{
            // this was a dead end room
            // pick a random door from available doors
            std::vector<std::pair<RoomData*, DoorData*>> options;
            for (auto& d : roomList) {
                for (auto& door : d->real_doors) {
                    if (!door.attached) {
                        options.push_back({d, &door});
                    }
                }
            }
            if (options.empty()) {
                print("ERROR WOW: chooseNextRoom: no free doors found AFTER DOING EXTRA CHECK.");
                return {{nullptr, nextRoom}, ""};
            }

            print("Found", (int)options.size(), "free doors for selection of door after dead end.");
            int doorIndex = rand() % options.size();
            DoorData* door = options[doorIndex].second;
            RoomData* room = options[doorIndex].first;
            return chooseNextRoom(room, roomsLeft);
        }
    } else {
        print("Last room is a hallway, picking random room.");
        // if the total amount of free doors is <= 2 then we are creating a room with > 2 doors
        // if not then we create a random room giving priority to required rooms from roomTypes making
        // sure we have at least 1 of each type before doubles
        std::vector<std::pair<RoomData*, DoorData*>> totalFreeDoors;
        for (auto& r : roomList) {
            for (auto& d : r->real_doors) {
                if (!d.attached) {
                    totalFreeDoors.push_back({r, &d});
                }
            }
        }

        if (totalFreeDoors.size() <= 2) {
            print("chooseNextRoom: total free doors <= 2, picking random room.");
            // get a free door, if it is a hall then add a room with 2 doors, if not add a hall
            // pick random door from free doors
            int doorIndex = rand() % totalFreeDoors.size();
            DoorData* door = totalFreeDoors[doorIndex].second;
            RoomData* room = totalFreeDoors[doorIndex].first;

            if (room->type != "hall") {
                print("chooseNextRoom: room is not a hall, picking random room.");
                return chooseNextRoom(room, roomsLeft);
            }else {
                print("chooseNextRoom: room is a hall, picking random room with more than 2 doors.");
                std::vector<RoomData> options;
                for (auto& r : roomTemplates) {
                    if (r.type == "room") {
                        if (r.doors.size() > 2) {
                            options.push_back(r);
                        }
                    }
                }
                if (options.size() == 0) {
                    error("chooseNextRoom: no room options found.");
                }

                int roomIndex = rand() % options.size();
                nextRoom = options[roomIndex];
            }

            lastRoom = room;
            doorAttachId = door->id;
        } else {
            print("chooseNextRoom: total free doors > 2, picking random room.");
            for (auto& d : lastRoom->real_doors) {
                if (!d.attached) {
                    doorAttachId = d.id; // door to attach to
                    break;
                }
            }


            // go through the rooms we have already created
            std::map<std::string, int> roomCount;
            for (auto& r : roomList) {
                roomCount[r->id]++;
            }
            int requiredLeft = 0;
            for (auto& r : roomTypes) {
                if (r.required && roomCount[r.name] == 0) {
                    requiredLeft++;
                }
            }
            print("chooseNextRoom: required left:", requiredLeft);
            if (requiredLeft > 0) {
                print("chooseNextRoom: rooms left <= required left + 1, picking random room.");
                // pick a random room from the required rooms
                std::vector<RoomData> options;
                for (auto& r : roomTemplates) {
                    if (isRoomRequired(r.id) && roomCount[r.id] == 0) {
                        options.push_back(r);
                    }
                }
                if (options.size() == 0) {
                    error("chooseNextRoom: no required room options found.");
                }

                int roomIndex = rand() % options.size();
                nextRoom = options[roomIndex];
            } else {
                print("chooseNextRoom: rooms left > required left + 1, picking random room.");
                // pick a random room from the non-required rooms
                std::vector<RoomData> options;
                for (auto& r : roomTemplates) {
                    if (!isRoomRequired(r.id) && r.type != "hall") {
                        options.push_back(r);
                    }
                }
                if (options.size() == 0) {
                    error("chooseNextRoom: no non-required room options found.");
                }

                int roomIndex = rand() % options.size();
                nextRoom = options[roomIndex];
            }

        }
    }

    if (nextRoom.id.empty()) {
        error("chooseNextRoom: no next room found.");
    }

    return {{lastRoom, nextRoom}, doorAttachId};
}

RoomData* World::placeRoom(RoomData& roomData) {
    if (roomData.id == "") {
        error("placeRoom: room ID is empty.");
        return nullptr;
    }
    print("Placing room:", roomData.id);

    auto* newRoom = new RoomData(roomData); // <-- HEAP ALLOCATE
    print(newRoom->doors.size());

    newRoom->real_bounds = newRoom->bounds;
    newRoom->real_bounds->size.x = getScaledPixelWidth(newRoom->real_bounds->size.x);
    newRoom->real_bounds->size.y = getScaledPixelHeight(newRoom->real_bounds->size.y);

    recenterRoom(newRoom, {0, 0}); // dereference when passing

    if (roomData.id == "fullMap1") {
        worldData.get()["spawnPoint"] = newRoom->center;
    }

    roomList.push_back(newRoom); // push pointer, not object
    print("Placed room:", newRoom->id, "at", newRoom->coords.x, newRoom->coords.y);

    return newRoom;
}

vector2 getDoorConnectPoint(const DoorData& door) {
    vector2 connect = door.coords;

    switch (door.heading.get()) {
        case 0: // Right-facing horizontal door
            connect.x += door.length;
            connect.y += door.width / 2.0f;
            break;
        case 180: // Left-facing horizontal door
            connect.y += door.width / 2.0f;
            break;
        case 90: // Down-facing vertical door
            connect.x += door.width / 2.0f;
            connect.y += door.length / 2;
            break;
        case 270: // Up-facing vertical door
            connect.x += door.width / 2.0f;
            break;
        default:
            break;
    }

    return connect;
}

void World::attachRooms(RoomData* lastRoom, RoomData* newRoom, std::string lastRoomDoorId) {
    DoorData* lastRoomDoor = nullptr;
    for (auto& d : lastRoom->real_doors) {
        if (d.id == lastRoomDoorId) {
            lastRoomDoor = &d;
            break;
        }
    }
    if (!lastRoomDoor) {
        error("attachRooms: attach last room door not found.");
        return;
    }

    DoorData* connectToDoor = nullptr;
    std::vector<DoorData*> freeDoors;
    for (auto& d : newRoom->real_doors) {
        if (d.attached) continue;
        freeDoors.push_back(&d);


        if ((lastRoomDoor->heading == 180 && d.heading == 0) ||
            (lastRoomDoor->heading == 0 && d.heading == 180) ||
            (lastRoomDoor->heading == 90 && d.heading == 270) ||
            (lastRoomDoor->heading == 270 && d.heading == 90)) {
            connectToDoor = &d;
            break;
        }
    }

    if (freeDoors.size() == 0) {
        error("attachRooms: no free doors found.");
        return;
    }

    if (!connectToDoor) {
        // todo we are going to have to rotate the room to fit the door
        // pick a random door from the free doors
        int doorIndex = rand() % freeDoors.size();
        connectToDoor = freeDoors[doorIndex];
        print("No door found, using random door:", connectToDoor->id);
        // Calculate the rotation needed to align doors correctly
        Heading desiredHeading = lastRoomDoor->heading + 180;
        Heading rotationAmount = desiredHeading - connectToDoor->heading;

        if (rotationAmount == 360 || rotationAmount == 1 || rotationAmount == 359) {
            rotationAmount = Heading(0);
        } else if (rotationAmount == 89 || rotationAmount == 91) {
            rotationAmount = Heading(90);
        } else if (rotationAmount == 179 || rotationAmount == 181) {
            rotationAmount = Heading(180);
        } else if (rotationAmount == 269 || rotationAmount == 271) {
            rotationAmount = Heading(270);
        }

        // Rotate newRoom by calculated amount
        newRoom->rotation = static_cast<int>(rotationAmount);
        rotateRoom(newRoom, rotationAmount.get());
        print("Rotated room:", newRoom->id, "by", rotationAmount, "degrees");
    }

    // Calculate connection points
    vector2 lastConnectPoint = getDoorConnectPoint(*lastRoomDoor);
    vector2 newConnectPoint = getDoorConnectPoint(*connectToDoor);

    // Offset newRoom so doors align
    vector2 offset = lastConnectPoint - newConnectPoint;
    vector2 setCoords = newRoom->coords + offset;

    recenterRoom(newRoom, setCoords);

    connectToDoor->targetDoor = lastRoomDoor;
    lastRoomDoor->targetDoor = connectToDoor;

    connectToDoor->attached = true;
    lastRoomDoor->attached = true;
}

void World::recenterRoom(RoomData* room, vector2 coords) {
    print("Recenter room:", room->id, "to", coords.x, coords.y);
    room->coords = coords;
    room->changes++;

    room->center = room->coords + getScaledCoords(room->center);

    RoomData templateRoom;
    for (auto& r : roomTemplates) {
        if (r.id == room->id) {
            templateRoom = r;
            break;
        }
    }

    room->real_walls.clear();
    for (auto& w : room->walls) {
        WallData newWall = w; // make a full copy first
        newWall.coords = getScaledCoords(newWall.coords);
        newWall.coords += room->coords; // offset wall coords
        newWall.length = getScaledPixelWidth(w.length);
        newWall.width = getScaledPixelHeight(w.width);
        wall* wPtr = new wall(newWall.coords, newWall.length, newWall.width, newWall.heading);
        wallList.push_back(wPtr); // global list
        room->real_walls.push_back(wPtr); // room list local
    }

    room->real_doors.clear();
    for (auto& d : room->doors) {
        DoorData door = d; // make a full copy first
        door.coords = getScaledCoords(door.coords);
        door.coords += room->coords; // offset door coords
        door.length = getScaledPixelWidth(d.length);
        door.width = getScaledPixelHeight(d.width);
        room->real_doors.push_back(door);
    }
}

void World::rotateRoom(RoomData* room, int angle) {
    vector2 center = room->center;
    float rad = angle * M_PI / 180.0f;

    for (auto& w : room->walls) {
        vector2 rel = w.coords - center;
        vector2 rotated(
                rel.x * cos(rad) - rel.y * sin(rad),
                rel.x * sin(rad) + rel.y * cos(rad)
        );
        w.coords = rotated + center;
        w.heading = static_cast<int>(w.heading + angle);
    }

    for (auto& d : room->doors) {
        vector2 rel = d.coords - center;
        vector2 rotated(
                rel.x * cos(rad) - rel.y * sin(rad),
                rel.x * sin(rad) + rel.y * cos(rad)
        );
        d.coords = rotated + center;

        // Before updating heading, check if a swap is needed
        int prevHeading = d.heading.get();
        print("Door - "+d.id+" - Prev heading before rot:", prevHeading);
        d.heading = static_cast<Heading>(prevHeading + angle);

        if (d.heading == 360 || d.heading == 1 || d.heading == 359) {
            d.heading = Heading(0);
        } else if (d.heading == 89 || d.heading == 91) {
            d.heading = Heading(90);
        } else if (d.heading == 179 || d.heading == 181) {
            d.heading = Heading(180);
        } else if (d.heading == 269 || d.heading == 271) {
            d.heading = Heading(270);
        }
        print("Door - "+d.id+" - New heading after rot:", d.heading.get());


        // If the heading rotated by 90 or 270, swap length and width
        int angleInt = static_cast<int>(angle) % 360;
        print(angleInt);
        if (angleInt == 90 || angleInt == 270) {
            print("Door - "+d.id+" - Swapping length and width");
            print("Door - "+d.id+" - Length before swap:", d.length);
            print("Door - "+d.id+" - Width before swap:", d.width);
            std::swap(d.length, d.width);
            print("Door - "+d.id+" - Length after swap:", d.length);
            print("Door - "+d.id+" - Width after swap:", d.width);
            if (prevHeading == 180) {
                if (d.heading == 270) {
                    // shift the coords to the left
                    d.coords.x -= d.length;
                }
            }else if (prevHeading == 0) {
                if (d.heading == 90) {
                    d.coords.x -= d.length;
                }
            }
//            else if (prevHeading == 270) {
//                print("Door - "+d.id+" - prevHeading == 270", d.heading.get());
//                if (d.heading == 0 || d.heading == 360) {
//                    // shift the coords to the left
//                    d.coords.x -= d.width;
//                }
//            }
        }else if (angleInt == 180) {
            // if the door is facing left, shift the coords to the left
            if (d.heading == 0) {
                d.coords.y -= d.width;
            }

            if (d.heading == 270) {
                // shift the coords to the left
                d.coords.x -= d.length;
            }else if (d.heading == 90) {
                // shift the coords to the left

            }
        }
    }

    // Update real doors/walls to reflect the rotation
    recenterRoom(room, room->coords);
}



void World::finalizeWorld() {
    // Final sealing logic could go here
}

bool World::isRectInWall(vectorList_t& rect) {
    if (isDebug()) return false;
    for (auto& w : roomList) {
        for (auto& cur_wall : w->real_walls) {
            if (cur_wall->isRectangleInWall(rect)) {
                return true;
            }
        }
    }
    return false;
}

bool World::doesRoomOverlap(RoomData* candidate) {
    if (!candidate->real_bounds) return false;

    // Calculate center of the candidate room
    vector2 center = candidate->center;

    for (auto* placed : roomList) {
        if (placed == candidate || !placed->real_bounds) continue;
        const auto& b = *placed->real_bounds;

        bool insideX = center.x >= b.position.x && center.x <= b.position.x + b.size.x;
        bool insideY = center.y >= b.position.y && center.y <= b.position.y + b.size.y;

        if (insideX && insideY) {
            return true; // Center is inside an existing room
        }
    }
    return false;
}
