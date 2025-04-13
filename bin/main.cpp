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
#include "MainMenu.h"
#include "GameInitializer.h"
#include "GameStorage.h"

int main(int argc, char* argv[])
{
    // Game Init
    print("Program Start");

    // Create a new Scheduler
    auto scheduler = std::make_shared<Scheduler>();

    // Create a Process Manager
    auto processManager = std::make_shared<ProcessManager>();
    // Pass function to all processes to trigger events in the rest of the processes
    passFunc_t passFunc = [processManager](const std::string& eventName, const json& eventData) {
        processManager->triggerEventInAll(eventName, eventData);
    };

    // Create a Game Storage
    auto gameStorage = std::make_shared<GameStorage>();
    gameStorage->load();

    // Create a Game Initializer
    auto gameInitializer = std::make_shared<GameInitializer>(passFunc, processManager, gameStorage, scheduler);
    gameInitializer->Init();
    processManager->attachProcess(gameInitializer);


    // Create a View process; this is the compliment to the gameInitializer; this will load chatbox and main menus
    auto viewProcess = std::make_shared<view>(passFunc, processManager);
    processManager->attachProcess(viewProcess);


    // Seed the random number generator
    if (debugMode == 1) {
        srand(69420);
    }else {
        // cast startTime to int
        srand(static_cast<unsigned int>(scheduler->getStartTime().time_since_epoch().count()));
    }

    // Config Change
    gameInitializer->AddEventHandler("UFO::Quit", [scheduler]() {
        print("Quitting Game");
        scheduler->shutdown();
    });

    // Auto room from PNG
    scheduler->setTimeout(2000, [gameInitializer]() {
        print("Creating Room from PNG");
        std::string message = "createRoomFromPng testwalls_720.png";
        sList_t args = split(message, " ");
        args.erase(args.begin());
        gameInitializer->TriggerEvent("__internal_command_createRoomFromPng", "chat", args, message);
    });


    if (!viewProcess->initialize()) {
        error("View Process failed to initialize");
        processManager->abortAllProcess();
        return 0;
    }
    viewProcess->initialize_manual(); // Initialize the view process and set it to manual update mode

    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Sleep for 1 ms to allow the view process to initialize fully
    SDL_Window* window = viewProcess->getWindow();

    while (scheduler->isRunning()) {
        float deltaMs = scheduler->run();

        if (!(SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)) {
            processManager->updateProcessList(deltaMs, window);
        }

        viewProcess->update(deltaMs);

        if (viewProcess->isDone()) {
            scheduler->shutdown();
        }

        scheduler->wait();
    }

    processManager->abortAllProcess();

    return 0;
}