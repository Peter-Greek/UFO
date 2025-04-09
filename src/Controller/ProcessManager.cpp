// ProcessManager.cpp
#include "ProcessManager.h"
#include <algorithm>

int ProcessManager::updateProcessList(float deltaMs, SDL_Window *window) {
    std::list<std::shared_ptr<xProcess>> removalList;

    for (auto& p : processList) {
        if (p->state() == xProcess::UNINITIALIZED) {
            int result;
            if (p->isSDLSubProcess()) {
                result = p->initialize_SDL_process(window);
            } else {
                result = p->initialize();
            }
            if (result == 0) continue;
            p->initialized();
        }

        if (p->state() == xProcess::RUNNING) {
            p->update(deltaMs);
        }

        if (p->dead()) {
            if (p->state() == xProcess::SUCCESS) {
                p->postSuccess();
                if (p->hasChild()) {
                    print("Attaching child process: ", p->getChild());
                    attachProcess(std::shared_ptr<xProcess>(p->getChild()));
                }
            } else if (p->state() == xProcess::FAIL) {
                p->postFail();
            } else if (p->state() == xProcess::ABORT) {
                p->postAbort();
            }
            removalList.push_back(p);
        }
    }

    for (auto& p : removalList) {
        removeProcess(p);
    }
    removalList.clear();

    return 0;
}

void ProcessManager::attachProcess(std::shared_ptr<xProcess> p) {
    p->setId(generateUUID());
    processList.push_back(p);
}

void ProcessManager::attachProcess(xProcess* p) {
    attachProcess(std::shared_ptr<xProcess>(p)); // will own it now
}

void ProcessManager::abortAllProcess() {
    for (auto& p : processList) {
        p->abort();
        p->postAbort();
    }
    processList.clear();
}

int ProcessManager::removeProcess(const std::shared_ptr<xProcess>& p) {
    processList.remove(p);
    return 0;
}

int ProcessManager::removeProcess(xProcess* p) {
    auto it = std::find_if(processList.begin(), processList.end(),
                           [p](const std::shared_ptr<xProcess>& process) { return process.get() == p; });
    if (it != processList.end()) {
        processList.erase(it);
        return 0;
    }
    return -1; // Process not found
}

void ProcessManager::triggerEventInAll(const std::string &eventName, const json &eventData) {
    for (auto& p : processList) {
        if (p->dead()) continue;
        p->onTriggerEvent(eventName, eventData);
    }
}

bool ProcessManager::containsUUID(UUID_t uuid) {
    return std::find(uuidList.begin(), uuidList.end(), uuid) != uuidList.end();
}

UUID_t ProcessManager::generateUUID() {
    UUID_t uuid;
    do {
        uuid = "pm-xxxxxx-4xxx-yxxx";
        for (int i = 0; i < uuid.length(); ++i) {
            if (uuid[i] == 'x') {
                uuid[i] = "0123456789abcdef"[rand() % 16];
            } else if (uuid[i] == 'y') {
                uuid[i] = "89ab"[rand() % 4];
            }
        }
    } while (containsUUID(uuid));

    uuidList.push_back(uuid);
    return uuid;
}

