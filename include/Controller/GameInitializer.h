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
// Created by xerxe on 3/28/2025.
//

#ifndef CSCI437_GAMEINITIALIZER_H
#define CSCI437_GAMEINITIALIZER_H
#include "xProcess.h"
#include "Util.h"
#include "ProcessManager.h"
#include "GameStorage.h"

#include "GameManager.h" // has all the headers for the entities and components
#include "UpgradeMenu.h"
#include "Scheduler.h"

class GameInitializer : public xProcess {
private:
    passFunc_t passFunc;
    ProcessManager& processManager;
    GameStorage& gameStorage;
    Scheduler& sch;
public:
    GameInitializer(passFunc_t p1, ProcessManager& pM, GameStorage& loader, Scheduler& sch_p) :
        passFunc(p1),
        processManager(pM),
        gameStorage(loader),
        xProcess(false, p1),
        sch(sch_p)
    {};
    void update(float deltaMs) override {};
    bool isDone() override {return false;};
    void Init();
    void Start();
    void End();
    void Debug();
};


#endif //CSCI437_GAMEINITIALIZER_H
