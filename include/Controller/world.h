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

using wallList_t = std::list<wall*>;
using roomList_t = std::list<wallList_t>;

class world : public xProcess {
private:
    bool gameRunning = false;

    jLoader worldData;

    // Debug world maker function
    int curRoom = 0;
    int curWall = 0;

    wallList_t wallList;
    roomList_t roomList;
public:
    explicit world(const std::function<void(const std::string& eventName, const json& eventData)>& func) : xProcess(false, func) {
        worldData = jsonLoader("../resource/world.json");
        loadWorld();
        generateLayout(5);
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
};


#endif //CSCI437_WORLD_H
