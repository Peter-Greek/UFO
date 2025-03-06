//
// Created by xerxe on 3/6/2025.
//

#ifndef CSCI437_WORLD_H
#define CSCI437_WORLD_H

#include "xProcess.h"
#include "Util.h"
#include "jsonLoader.h"
#include "TxdLoader.h"
class world : public xProcess {
private:
    bool gameRunning = false;

    jsonLoader worldData;

    // Debug world maker function
    int curRoom = 0;
    int curWall = 0;

public:
    explicit world(const std::function<void(const std::string& eventName, const json& eventData)>& func) : xProcess(false, func) {
        worldData = jsonLoader("../resource/world.json");
    }

    int initialize() override;
    void update(float deltaMs) override;
    bool isDone() override { return !gameRunning; };
    void postSuccess() override {};
    void postFail() override {};
    void postAbort() override {};

    void loadWorld();
    void saveWorld();
    void updateWorld();

    jsonLoader getWorldData();
};


#endif //CSCI437_WORLD_H
