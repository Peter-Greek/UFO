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

////
//// Created by xerxe on 2/4/2025.
////

#define JSON_USE_IMPLICIT_CONVERSIONS 0
//#define _GLIBCXX_USE_CXX11_ABI 0  // Fix for MinGW atomic issues

#include "Scheduler.h"

#include <memory>
#include <vector>
#include <algorithm>
#include <fstream>

#include <SDL.h>
#include "jsonLoader.h"
#include "Util.h"
#include "config.h"
#include "ProcessManager.h"
#include "view.h"
#include "ChatBox.h"
#include "UpgradeMenu.h"
#include "GameInitializer.h"
#include "GameStorage.h"

int main(int argc, char* argv[])
{
    // Game Init
    print("Program Start");

    // Create a new Scheduler
    Scheduler scheduler;

    // Create a Process Manager
    ProcessManager processManager;
    // Pass function to all processes to trigger events in the rest of the processes
    passFunc_t passFunc = [&processManager](const std::string& eventName, const json& eventData) {
        processManager.triggerEventInAll(eventName, eventData);
    };

    // Create a Game Storage
    GameStorage gameStorage;
    gameStorage.load(); // Load JSON storage file

    // Create a Game Initializer
    GameInitializer gameInitializer(passFunc, processManager, gameStorage, scheduler);
    gameInitializer.Init();
    processManager.attachProcess(&gameInitializer);


    // Create a View process; this is the compliment to the gameInitializer; this will load chatbox and main menus
    view* viewProcess = new view(passFunc, processManager);
    processManager.attachProcess(viewProcess);


    // Seed the random number generator
    if (debugMode == 1) {
        srand(69420);
    }else {
        // cast startTime to int
        srand(static_cast<unsigned int>(scheduler.getStartTime().time_since_epoch().count()));
    }

    // Config Change
    gameInitializer.AddEventHandler("UFO::Quit", [&scheduler]() {
        print("Quitting Game");
        scheduler.shutdown();
    });


    if (!viewProcess->initialize()) {
        error("View Process failed to initialize");
        delete viewProcess;
        return 0;
    }
    viewProcess->initialize_manual(); // Initialize the view process and set it to manual update mode

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Sleep for 1 ms to allow the view process to initialize fully
    SDL_Window* window = viewProcess->getWindow();
    float deltaMs = 0;
    viewProcess->TriggerEvent("UFO::StartGame"); // debug start game (later on we will have a main menu)
    while (scheduler.isRunning()) {
        deltaMs = scheduler.elapsedTime();
        deltaMs *= scheduler.getTimeFactor();
        deltaMs = deltaMs <= 0 ? 1 : deltaMs;
        if (! (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)) { // dont update any of the logic or view when the window is minimized
            processManager.updateProcessList(deltaMs, window);
        }
        viewProcess->update(deltaMs); // Update the view process last as all the logic gets updated before this

        if (!unlimitedFrames) std::this_thread::sleep_for(std::chrono::milliseconds(1));

        if (viewProcess->isDone()) {
            scheduler.shutdown();
        }
    }

    return 0;
}