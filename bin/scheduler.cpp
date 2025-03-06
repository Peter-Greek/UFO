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


#include <chrono>
#include <thread>
#include <mutex>
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
#include "text.h"

#include "GameManager.h"
#include "entity.h"
#include "Player.h"
#include "camera.h"
#include "Laser.h"
#include "AT.h"
#include "AsepriteLoader.h"

// Scheduler Variables
jLoader gameStorage("../resource/storage.json");
bool threadDone = false;
long gameTimeFactor = 1;
float gameTime = 0;
static auto startTime = std::chrono::high_resolution_clock::now();

// Scheduler Functions
float getTimeElapsed()
{
    auto curTime = std::chrono::high_resolution_clock::now();
    float diff = std::chrono::duration<float, std::chrono::milliseconds::period>(curTime - startTime).count();
    //long (std::chrono::duration_cast<std::chrono::milliseconds>(curTime - startTime).count());
    float timeElapsed = diff - gameTime;
    gameTime = diff;
    return timeElapsed;
}

float GetGameTimer() {
    return gameTime;
}

// Timeout System
std::mutex m;
void setTimeout(int delay, std::function<void()> function) {
    std::thread t([delay, function]() {
        // mutex to lock the thread
        std::lock_guard<std::mutex> lock(m);
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        function();
    });
    t.detach();
}

void CreateDebugText(passFunc_t passFunc, ProcessManager& processManager)
{
    std::string frameTextContent = "Unlimited Frames: " + std::to_string(unlimitedFrames);
    text* frameText = new text(passFunc, frameTextContent);
    frameText->setTextRelativePosition(0.0f, 0.8f);
    processManager.attachProcess(frameText);


    std::string fpsTextContent = "FPS: " + std::to_string(unlimitedFrames);
    text* fpsText = new text(passFunc, fpsTextContent);
    fpsText->setTextRelativePosition(0.0f, -0.8f);
    fpsText->AddEventHandler("SDL::OnUpdate", [fpsText](float deltaMs) {
        std::string fpsTextContent = "FPS: " + std::to_string(1000/deltaMs);
        fpsText->setText(fpsTextContent);
    });
    processManager.attachProcess(fpsText);

    std::string gameTimeContent = "Time: " + std::to_string(gameTime);
    text* gameTimeText = new text(passFunc, gameTimeContent);
    gameTimeText->setTextRelativePosition(0.0f, 0.6f);
    gameTimeText->AddEventHandler("SDL::OnUpdate", [gameTimeText](float deltaMs) {
        std::string gameTimeContent = "Time: " + std::to_string(gameTime);
        gameTimeText->setText(gameTimeContent);
    });
    processManager.attachProcess(gameTimeText);


    frameText->AddEventHandler("UFO::OnConfigUpdate", [frameText, fpsText, gameTimeText](const std::string configName) {
        if (configName == "unlimitedFrames") {
            std::string frameTextContent = "Unlimited Frames: " + std::to_string(unlimitedFrames);
            frameText->setText(frameTextContent);
        }else if (configName == "debugMode") {
            if (debugMode == 1) {
                frameText->setTextRelativePosition(0.0f, 0.8f);
                fpsText->setTextRelativePosition(0.0f, -0.8f);
                gameTimeText->setTextRelativePosition(0.0f, 0.6f);
            } else {
                frameText->setTextPosition(-100.0f, -100.0f);
                fpsText->setTextPosition(-100.0f, -100.0f);
                gameTimeText->setTextPosition(-100.0f, -100.0f);
            }
        }
    });


    if (debugMode != 1) {
        frameText->setTextPosition(-100.0f, -100.0f);
        fpsText->setTextPosition(-100.0f, -100.0f);
        gameTimeText->setTextPosition(-100.0f, -100.0f);
    }
}

void CreateGameEnvironment(passFunc_t passFunc, ProcessManager& processManager){
    auto* gM = new GameManager(passFunc);
    processManager.attachProcess(gM);

    // Create Camera
    auto* cam = new camera(passFunc);
    processManager.attachProcess(cam);
    gM->setCamera(cam);

    std::string coordsTextContent = "X: 0.0, Y: 0.0";
    auto* cText = new text(passFunc, coordsTextContent, 35);
    cText->setTextRelativePosition(0.0f, -0.8f);
    processManager.attachProcess(cText);
    gM->attachText("CamCoords", cText);

    std::string rHeadingC = "Heading: 0";
    auto* rHeading = new text(passFunc, rHeadingC, 35);
    rHeading->setTextRelativePosition(0.0f, -0.7f);
    processManager.attachProcess(rHeading);
    gM->attachText("RelHeading", rHeading);

    // Create Player
    auto* fAnim = new AsepriteLoader(passFunc, "../resource/FSS.png", "../resource/FSS.json");
    processManager.attachProcess(fAnim);
    gM->attachAseprite("FSS", fAnim);

    auto* ppl = new Player(passFunc);
    processManager.attachProcess(ppl);
    gM->attachEntity(ppl);
    ppl->spawn();

    std::string atScoreText = "AT: 0";
    auto* atScore = new text(passFunc, atScoreText, 35);
    atScore->setTextRelativePosition(-1.0f, -0.7f);
    processManager.attachProcess(atScore);
    gM->attachText("ATScore", atScore);

    std::string heartsText = "Hearts: 5";
    auto* pHearts = new text(passFunc, heartsText, 35);
    pHearts->setTextRelativePosition(-1.0f, -0.8f);
    processManager.attachProcess(pHearts);
    gM->attachText("PlayerHearts", pHearts);

    std::string oxyTimeC = "Oxygen: 3:00";
    auto* oxyTime = new text(passFunc, oxyTimeC, 35);
    oxyTime->setTextRelativePosition(-1.0f, -0.9f);
    processManager.attachProcess(oxyTime);
    gM->attachText("OxyTimer", oxyTime);



    //TODO: anything above 700 starts to lag only when rendered on screen so view needs optimizations
    for (int i = 0; i < 200; i++) {
        // random location for the AT
        auto* at = new AT(passFunc, {static_cast<float>(random(0, SCREEN_WIDTH)), static_cast<float>(random(0, SCREEN_HEIGHT))});
        processManager.attachProcess(at);
        gM->attachEntity(at);
        at->spawn();
    }

    // Create Laser
    auto* lTxd = new TxdLoader(passFunc, "../resource/LaserBeams.png");
    processManager.attachProcess(lTxd);
    gM->attachTxd("LASER::TEXTURE", lTxd);

    auto* laser = new Laser(passFunc, {-700.0f, 0.0f}, Heading (360 - 45 * 6), 500, 20, 10, 200000, 1, 1);
    processManager.attachProcess(laser);
    gM->attachEntity(laser);
    laser->setSpin(true);
    laser->spawn();

    auto* laser2 = new Laser(passFunc, {50.0f, 0.0f}, Heading (360 - 45 * 7), 500, 20, 1000, 3000, 1, 1);
    processManager.attachProcess(laser2);
    gM->attachEntity(laser2);
    laser2->spawn();


    // Create NPC
    auto* aTxd = new TxdLoader(passFunc, "../resource/Alien1.png");
    processManager.attachProcess(aTxd);
    gM->attachTxd("ALIEN::TEXTURE", aTxd);

    auto* npc = new entity(passFunc, entity::ENEMY, 3, {350.0f, 0.0f});
    processManager.attachProcess(npc);
    gM->attachEntity(npc);
    npc->spawn();

    // Create Heart Pickup
    auto* heart = new entity(passFunc, entity::ITEM_PICKUP, 2, {-50.0f, 0.0f});
    processManager.attachProcess(heart);
    gM->attachEntity(heart);
    heart->spawn();

    // Create Oxy Pickup
    auto* oxy = new entity(passFunc, entity::ITEM_PICKUP, 3, {0.0f, 50.0f});
    processManager.attachProcess(oxy);
    gM->attachEntity(oxy);
    oxy->spawn();
}

int applySettings() {
    gameStorage["settings"] = gameStorage["settings"] != nullptr ? gameStorage["settings"] : json::object();
    if (gameStorage["settings"]["unlimitedFrames"] != nullptr) {
        unlimitedFrames = gameStorage["settings"]["unlimitedFrames"].get<bool>();
    }

    if (gameStorage["settings"]["debugMode"] != nullptr) {
        debugMode = gameStorage["settings"]["debugMode"].get<int>();
    }

    updateSettings(); // Update non stored settings based on the new changes
    return 0;
}

int loadGameStorage() {
    gameStorage.update();
    applySettings(); // Apply the settings from the storage file
    gameStorage["loads"] = gameStorage["loads"] != nullptr ? gameStorage["loads"].get<int>() + 1 : 1; // Increment the loads counter
    gameStorage.save();
    return 0;
}


int main(int argc, char* argv[])
{
    // Create a Process Manager
    ProcessManager processManager;

    // Game Init
    print("Game Init");

    // Pass function to all processes to trigger events in the rest of the processes
    passFunc_t passFunc = [&processManager](const std::string& eventName, const json& eventData) {
        processManager.triggerEventInAll(eventName, eventData);
    };

    // Create a view process; this will have to come first as it will be the main window and renderer
    view* viewProcess = new view(passFunc);
    processManager.attachProcess(viewProcess);

    // Load JSON storage file
    loadGameStorage();

    // Seed the random number generator
    if (debugMode == 1) {
        srand(69420);
    }else {
        // cast startTime to int
        srand(static_cast<unsigned int>(startTime.time_since_epoch().count()));
    }

    // Config Changes
    viewProcess->AddEventHandler("UFO::OnConfigUpdate", [](const std::string configName) {
        updateSettings(); // Update settings based on global variables

        if (configName == "unlimitedFrames") {
            gameStorage["settings"]["unlimitedFrames"] = unlimitedFrames;
            gameStorage.save();
        } else if (configName == "debugMode") {
            gameStorage["settings"]["debugMode"] = debugMode;
            gameStorage.save();
        }
    });

    viewProcess->AddEventHandler("UFO::ChangeConfigValue", [viewProcess](const std::string configName) {
        updateSettings(); // Update settings based on global variables

        if (configName == "unlimitedFrames") {
            unlimitedFrames = !unlimitedFrames;
        } else if (configName == "debugMode") {
            debugMode = debugMode == 0 ? 1 : 0;
        }

        viewProcess->TriggerEvent("UFO::OnConfigUpdate", configName);
    });

    // Create Main Menu
    viewProcess->AddEventHandler("UFO::StartGame", [&processManager, passFunc]() {
        // Create Game Environment
        CreateGameEnvironment(passFunc, processManager);
        CreateDebugText(passFunc, processManager);
    });

    viewProcess->AddEventHandler("UFO::EndGame", [&viewProcess, passFunc, &processManager](int winner) {
        print("Ending Game Loop: ", winner);
        viewProcess->TriggerEvent("UFO::OnGameEnd"); // clean up the current game environment
        // Create a new main menu with the winner
    });

    viewProcess->AddEventHandler("UFO::Quit", [&viewProcess, &processManager]() {
        print("Quitting Game");
        threadDone = true;
    });

    std::thread schedulerThread([&processManager, &viewProcess]()
    {

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
        while (!threadDone) {
            deltaMs = getTimeElapsed();
            deltaMs *= gameTimeFactor;
            deltaMs = deltaMs <= 0 ? 1 : deltaMs;
            if (! (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)) { // dont update any of the logic or view when the window is minimized
                processManager.updateProcessList(deltaMs, window);
            }
            viewProcess->update(deltaMs); // Update the view process last as all the logic gets updated before this
            if (!unlimitedFrames) std::this_thread::sleep_for(std::chrono::milliseconds(1));

            if (viewProcess->isDone()) {
                threadDone = true;
            }
        }
        return 0;
    });

    // Wait for the thread to finish
    schedulerThread.join();

    return 0;
}