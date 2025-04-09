//
// Created by xerxe on 2/4/2025.
//

#ifndef CSCI437_PROCESSMANAGER_H
#define CSCI437_PROCESSMANAGER_H
#include <list>
#include <memory>
#include "xProcess.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ProcessManager {
private:
    std::list<std::shared_ptr<xProcess>> processList;
    std::list<UUID_t> uuidList;
    passFunc_t passFunc;

public:
    ProcessManager() = default;
    ~ProcessManager() = default;

    int updateProcessList(float deltaMs, SDL_Window* window);
    void attachProcess(std::shared_ptr<xProcess> p);
    void abortAllProcess();
    int removeProcess(const std::shared_ptr<xProcess>& p);
    void triggerEventInAll(const std::string& eventName, const json& eventData);
    UUID_t generateUUID();
    bool containsUUID(UUID_t uuid);

    void attachProcess(xProcess *p);

    int removeProcess(xProcess *p);
};


#endif //CSCI437_PROCESSMANAGER_H
