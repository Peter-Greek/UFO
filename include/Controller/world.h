//
// Created by xerxe on 3/6/2025.
//

#ifndef CSCI437_WORLD_H
#define CSCI437_WORLD_H

#include "xProcess.h"
#include "Util.h"
#include "jsonLoader.h"
#include "TxdLoader.h"
#include "wall.h"
#include "vector"
#include <unordered_set>

using wallList_t = std::list<wall*>;
using roomList_t = std::list<wallList_t>;

class world : public xProcess {
private:
    bool gameRunning = false;
    jLoader worldData;

    struct roomType {
        std::string name;
        bool required;
    };

    std::list<roomType> roomTypes = {
        {"startRoom", true},
        {"lockedRoom1", false},
        {"lockedRoom2", false},
        {"medicalRoom", false},
        {"engineRoom", false},
        {"escapePodRoom", false},
        {"monsterAmbushRoom", false},
        {"adminRoom", false},
        {"randomRoom", false},
        {"randomRoom2", false},
    };

    struct hallwayType {
        std::string name;
        bool horizontal;
    };

    std::list<hallwayType> hallwayTypes = {
        {"horizontalHallway", true},
        {"verticalHallway", false},
    };

    std::string imgPath = "../resource/GFX/world/";
    std::string worldPath = "../resource/json/world.json";

    // Debug world maker function
    int curRoom = 0;
    int curWall = 0;
    wallList_t wallList;
    roomList_t roomList;
    std::unordered_set<std::string> usedRoomTypes;
    std::unordered_map<std::string, int> roomTypeCount;
    std::vector<std::string> requiredRoomTypes;
    json getRoomById(const json& templates, const std::string& id);
    void offsetRoom(json& room, const vector2& offset);
    void rotateRoom(json& room, float angle);
    void markDoors(json& room, std::vector<json>& doorPool, json* skip = nullptr);
    float getRequiredRotation(const json& openDoor, const json& newDoor);
    bool shouldAddHallway(int specialPlaced, int maxCount, int placed, const std::string& lastType);
    bool shouldAddSpecialRoom(int specialPlaced);
    json pickHallway(const json& templates);
    json pickUnusedSpecialRoom(const json& templates);
    std::string getConnectingRoomType(const json& door);
    json* pickAvailableDoor(std::vector<json>& doors);
    json* findUnusedDoor(json& room);
    json pickUsedSpecialRoom(const json& templates);
public:
    explicit world(const std::function<void(const std::string& eventName, const json& eventData)>& func) : xProcess(false, func) {
        worldData = jsonLoader(worldPath);
    }

    int initialize() override;
    void update(float deltaMs) override;
    bool isDone() override { return !gameRunning; };
    void postSuccess() override {};
    void postFail() override {};
    void postAbort() override {};
    void loadWorld();
    void saveWorld();
    jLoader& getWorldData();
    wallList_t getWallList();
    roomList_t getRoomList();
    bool isPointInWall(vector2 vector21);
    bool isRectInWall(vectorList_t& rect);
    int addRoom();
    void updateWall(int roomId, int wallId, wall *w);
    int addWall(int roomId, wall *w);
    void deleteWall(int roomId, int wallId);
    void deleteRoom(int roomId);
    vector2 getSpawnPoint();
    void generateLayout(int count);
    json getAllEntities();
};


#endif //CSCI437_WORLD_H
