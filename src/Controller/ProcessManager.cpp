/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Peter Greek
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Proper permission is grated by the copyright holder.
 *
 * Credit is attributed to the copyright holder in some form in the product.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

//
// Created by xerxe on 2/4/2025.
//

#include "ProcessManager.h"
#include <algorithm> // Required for std::find

int ProcessManager::updateProcessList(float deltaMs, SDL_Window *window) {
    std::list<xProcess*> removalList;
    for (auto p : processList) {
        if (p->state() == xProcess::UNINITIALIZED) {
            int result;
            if (p->isSDLSubProcess()) {
                result = p->initialize_SDL_process(window);
            }else {
                result = p->initialize();
            }

            if (result == 0) continue;
            p->initialized();
        };
        if (p->state() == xProcess::RUNNING) p->update(deltaMs);
        if (p->dead())
        {
            if (p->state() == xProcess::SUCCESS)
            {
                p->postSuccess();
                if(p->hasChild()) {
                    print("Attaching child process: ", p->getChild());
                    attachProcess(p->getChild());
                }
            }
            if (p->state() == xProcess::FAIL) p->postFail();
            if (p->state() == xProcess::ABORT) p->postAbort();
            removalList.push_back(p);
        }
    }

    for (auto p : removalList) {
        removeProcess(p);
    }
    removalList.clear();

    return 0;
}

void ProcessManager::attachProcess(xProcess *p) {
    p->setId(generateUUID());
    processList.push_back(p);
}

void ProcessManager::abortAllProcess() {
    for (auto p : processList) {
        p->abort();
    }
    processList.clear();
}

int ProcessManager::removeProcess(xProcess *p) {
    print("Removing process: ", p);
    processList.remove(p);
    return 0;
}

void ProcessManager::triggerEventInAll(const std::string &eventName, const json &eventData) {
    for (auto p : processList) {
        p->onTriggerEvent(eventName, eventData);
    }
}

bool ProcessManager::containsUUID(UUID uuid) {
    return std::find(uuidList.begin(), uuidList.end(), uuid) != uuidList.end();
}

UUID ProcessManager::generateUUID() {
    UUID uuid;
    do {
        uuid = "";
        uuid = "pm-xxxxxx-4xxx-yxxx";
        for (int i = 0; i < uuid.length(); i++) {
            if (uuid[i] == 'x') {
                uuid[i] = "0123456789abcdef"[rand() % 16];
            }else if (uuid[i] == 'y') {
                uuid[i] = "89ab"[rand() % 4];
            }
        }
    } while (containsUUID(uuid));
    uuidList.push_back(uuid);
    return uuid;
}
