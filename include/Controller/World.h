//
// Created by xerxe on 3/6/2025.
//

#ifndef CSCI437_WORLD_H
#define CSCI437_WORLD_H

#include "xProcess.h"
#include "Util.h"
#include "wall.h"
#include "vector2.h"
#include "jsonLoader.h"
#include <list>
#include <string>
#include <optional>
#include <vector>



struct WallData {
    vector2 coords;
    int length;
    int width;
    int heading;
};

struct DoorData {
    std::string id;
    bool attached;

    vector2 coords;
    Heading heading;
    int length;
    int width;
    bool closed;
    bool locked;

    DoorData* targetDoor;
};

struct BoundsData {
    vector2 position;
    vector2 size;
};

struct RoomData {
    std::string id;
    std::string name;
    std::string type; // "room" or "hall"
    vector2 center; // center of the room for heading ref
    vector2 coords; // top left corner of the room
    std::string imagePath;
    std::optional<BoundsData> bounds;
    std::optional<BoundsData> valid_zone;
    std::vector<WallData> walls;
    std::vector<DoorData> doors;

    std::optional<BoundsData> real_bounds;
    std::vector<wall*> real_walls;
    std::vector<DoorData> real_doors;
    int changes = 1;
    int rotation = 0; // 0, 90, 180, 270
    SDL_RendererFlip flip = SDL_FLIP_NONE;
};

using wallList_t = std::list<wall*>;
using roomList_t = std::vector<RoomData*>;
using nextRoomData_t = std::pair<std::pair<RoomData*, RoomData>, std::string>;

const static std::string imgPath = "../resource/GFX/world/rooms/";
const static std::string worldPath = "../resource/json/world.json";

class World : public xProcess {
public:
    World(passFunc_t func);

    int initialize();
    void update(float deltaMs);
    bool isDone() override { return !gameRunning; }
    void postSuccess() override {}
    void postFail() override {}
    void postAbort() override {}

    bool generateWorld(int roomCount);
    bool isRectInWall(vectorList_t& rect);

    // Compatibility
    wallList_t getWallList();
    roomList_t getRoomList();
    json getAllEntities();
    jLoader& getWorldData();
    vector2 getSpawnPoint();

private:

    bool gameRunning = false;
    wallList_t wallList;
    roomList_t roomList;


    jLoader worldData;
    std::vector<RoomData> roomTemplates;

    struct roomType {
        std::string name;
        bool required;
    };

    struct hallwayType {
        std::string name;
        bool horizontal;
    };

    std::list<std::string> roomPngs = {
            "adminRoom.png",
            "bossRoom.png",
            "engineRoom.png",
            "escapePodRoom.png",
            "horizontalHallway.png",
            "lockedRoom1.png",
            "lockedRoom2.png",
            "medicalRoom.png",
            "monsterAmbushRoom.png",
            "randomRoom.png",
            "randomRoom2.png",
            "startRoom.png",
            "verticalHallway.png",
    };

    std::list<roomType> roomTypes = {
            {"adminRoom", true},
            {"bossRoom", true},
            {"engineRoom", true},
            {"escapePodRoom", true},
            {"lockedRoom1", true},
            {"lockedRoom2", true},
            {"medicalRoom", false},
            {"monsterAmbushRoom", false},
            {"randomRoom", false},
            {"randomRoom2", false},
            {"startRoom", true},
    };
    int totalRequiredRooms = 7;

    std::list<hallwayType> hallwayTypes = {
            {"horizontalHallway", true},
            {"verticalHallway", false},
    };

    RoomData parseRoomFromJson(const json& roomJson);

    bool placeStartRoom();
    void finalizeWorld();

    int minRooms = 1;

    RoomData* placeRoom(RoomData &newRoom);

    void attachRooms(RoomData *lastRoom, RoomData *newRoom, std::string basicString);

    void recenterRoom(RoomData *room, vector2 coords);

    bool isRoomRequired(const std::string &roomId);

    nextRoomData_t chooseNextRoom(RoomData *lastRoom, int roomsLeft);

    void rotateRoom(RoomData *room, int angle);

    bool doesRoomOverlap(RoomData *candidate);
};




#endif //CSCI437_WORLD_H
