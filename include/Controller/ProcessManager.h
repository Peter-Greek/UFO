//
// Created by xerxe on 2/4/2025.
//

#ifndef PONG_PROCESSMANAGER_H
#define PONG_PROCESSMANAGER_H

#include <list>
#include "xProcess.h"


#include <nlohmann/json.hpp>
using json = nlohmann::json;

class ProcessManager {

private:
    std::list<xProcess*> processList;
    std::list<UUID> uuidList;
public:



    int updateProcessList(float deltaMs, SDL_Window* window);
    void attachProcess(xProcess* p);
    void abortAllProcess();
    int removeProcess(xProcess* p);
    void triggerEventInAll(const std::string& eventName, const json& eventData);

    UUID generateUUID();

    ProcessManager() = default;
    ~ProcessManager() = default;

    bool containsUUID(UUID uuid);
};


#endif //PONG_PROCESSMANAGER_H
