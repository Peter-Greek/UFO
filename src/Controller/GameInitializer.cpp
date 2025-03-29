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

#include "GameInitializer.h"

void GameInitializer::Init() {
    print("Game Init");
    AddEventHandler("UFO::OnConfigUpdate", [this](const std::string configName) {
        updateSettings(); // Update settings based on global variables

        if (configName == "unlimitedFrames") {
            gameStorage["settings"]["unlimitedFrames"] = unlimitedFrames;
            gameStorage.save();
        } else if (configName == "debugMode") {
            gameStorage["settings"]["debugMode"] = debugMode;
            gameStorage.save();
        }else if (configName == "curRoomIndex") {
            gameStorage["settings"]["curRoomIndex"] = curRoomIndex;
            gameStorage.save();
        }
    });

    AddEventHandler("UFO::ChangeConfigValue", [this](const std::string configName) {
        updateSettings(); // Update settings based on global variables

        if (configName == "unlimitedFrames") {
            unlimitedFrames = !unlimitedFrames;
        } else if (configName == "debugMode") {
            debugMode = debugMode == 0 ? 1 : 0;
        }

        TriggerEvent("UFO::OnConfigUpdate", configName);
    });

    AddEventHandler("UFO::StartGame", [this]() {
        // Create Game Environment
        print("Starting Game Loop");
        Start();
        Debug();
    });

    AddEventHandler("UFO::EndGame", [this](int hearts, int atcount) {
        print("Ending Game Loop, AT Count: ", atcount);
        TriggerEvent("UFO::OnGameEnd"); // clean up the current game environment
        gameStorage["player"]["ATCount"] = gameStorage["player"]["ATCount"].get<int>() + atcount;
        gameStorage["player"]["TotalAT"] = gameStorage["player"]["TotalAT"].get<int>() + atcount;
    });
}

void GameInitializer::Debug()
{
    std::string fpsTextContent = "FPS: " + std::to_string(64);
    text* fpsText = new text(passFunc, fpsTextContent);
    fpsText->setTextRelativePosition(0.0f, -0.8f);
    processManager.attachProcess(fpsText);

    std::string gameTimeContent = "Time: " + std::to_string(sch.getGameTime());
    text* gameTimeText = new text(passFunc, gameTimeContent);
    gameTimeText->setTextRelativePosition(0.0f, 0.8f);
    processManager.attachProcess(gameTimeText);

    AddEventHandler("SDL::OnUpdate", [gameTimeText, fpsText, this](float deltaMs) {
        std::string fpsTextContent = "FPS: " + std::to_string(1000.0f/deltaMs);
        fpsText->setText(fpsTextContent);
        std::string gameTimeContent = "Time: " + std::to_string(sch.getGameTime());
        gameTimeText->setText(gameTimeContent);
    });

    AddEventHandler("UFO::OnConfigUpdate", [fpsText, gameTimeText](const std::string configName) {
        if (configName == "debugMode") {
            if (debugMode == 1) {
                fpsText->setTextRelativePosition(0.0f, -0.8f);
                gameTimeText->setTextRelativePosition(0.0f, 0.6f);
            } else {
                fpsText->setTextPosition(-100.0f, -100.0f);
                gameTimeText->setTextPosition(-100.0f, -100.0f);
            }
        }
    });


    if (debugMode != 1) {
        fpsText->setTextPosition(-100.0f, -100.0f);
        gameTimeText->setTextPosition(-100.0f, -100.0f);
    }
}

void GameInitializer::Start(){
    auto* upgradeMenu = new UpgradeMenu(passFunc);
    processManager.attachProcess(upgradeMenu);
    upgradeMenu->setATCount(gameStorage["player"]["ATCount"].get<int>());


    auto* gM = new GameManager(passFunc);
    processManager.attachProcess(gM);
    // attach the process manager to the game manager so new entities can be created on the fly
    gM->attachProcessManager(&processManager);

    gM->AddEventHandler("UFO::CHECK::UUID", [this](UUID id) {
        print("Checking UUID: ", id);
        print(processManager.containsUUID(id));
    });

    // Create World (HAS TO START BEFORE ANY ENTITY IS ATTACHED!)
    auto* w = new world(passFunc);
    processManager.attachProcess(w);
    gM->setWorld(w);


    auto* wTxd = new TxdLoader(passFunc, "../resource/wall.png");
    processManager.attachProcess(wTxd);
    gM->attachTxd("WALL::TEXTURE", wTxd);

    for (int i = 0; i <= GameManager::db_WCS::WIDTH_SET; i++) {
        std::string coordsTextContent = "WALL: " + std::to_string(i);
        auto* cText = new text(passFunc, coordsTextContent, 35);
        cText->setTextRelativePosition(-1.0f, (-0.1f * (i + 1)));
        processManager.attachProcess(cText);
        gM->attachText("DEBUG::WALL::" + std::to_string(i), cText);
        cText->hideText();
    }

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

    upgradeList_t upgrades = {0, 0, 0, 0, 0}; // for debug purposes eventually will be loaded from json storage
    upgrades[Player::UPGRADES::OXYGEN]       = gameStorage["player"]["upgrades"]["oxygen"].get<int>();
    upgrades[Player::UPGRADES::SHIELD]       = gameStorage["player"]["upgrades"]["shield"].get<int>();
    upgrades[Player::UPGRADES::SPEED]        = gameStorage["player"]["upgrades"]["speed"].get<int>();
    upgrades[Player::UPGRADES::INVISIBILITY] = gameStorage["player"]["upgrades"]["invisibility"].get<int>();
    upgrades[Player::UPGRADES::AT_CANNON]    = gameStorage["player"]["upgrades"]["at_cannon"].get<int>();



    auto* ppl = new Player(passFunc, upgrades);
    processManager.attachProcess(ppl);
    gM->attachEntity(ppl);
    ppl->spawn();

    std::string atScoreText = "AT: 0";
    auto* atScore = new text(passFunc, atScoreText, 35);
    atScore->setTextRelativePosition(-1.0f, -0.7f);
    processManager.attachProcess(atScore);
    gM->attachText("ATScore", atScore);

    std::string heartsText = "Hearts: " + std::to_string(ppl->getHearts());
    auto* pHearts = new text(passFunc, heartsText, 35);
    pHearts->setTextRelativePosition(-1.0f, -0.8f);
    processManager.attachProcess(pHearts);
    gM->attachText("PlayerHearts", pHearts);

    std::string oxyTimeC = "Oxygen: 3:00";
    auto* oxyTime = new text(passFunc, oxyTimeC, 35);
    oxyTime->setTextRelativePosition(-1.0f, -0.9f);
    processManager.attachProcess(oxyTime);
    gM->attachText("OxyTimer", oxyTime);

    //Create Audio
    auto* audioHitmarker = new AudioLoader(passFunc, "../resource/sfx/hitmarker.wav");
    processManager.attachProcess(audioHitmarker);
    gM->attachAudio("hitmarker", audioHitmarker);

    // Create AT
    //TODO: anything above 700 starts to lag only when rendered on screen so view needs optimizations
    auto* ATTxd = new TxdLoader(passFunc, "../resource/ATLoot1.png");
    processManager.attachProcess(ATTxd);
    gM->attachTxd("AT::TEXTURE", ATTxd);
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
    gM->attachTxd("ALIEN1::TEXTURE", aTxd);

    auto* a2Txd = new TxdLoader(passFunc, "../resource/Alien2.png");
    processManager.attachProcess(a2Txd);
    gM->attachTxd("ALIEN2::TEXTURE", a2Txd);

    auto* a3Txd = new TxdLoader(passFunc, "../resource/Alien3.png");
    processManager.attachProcess(a3Txd);
    gM->attachTxd("ALIEN3::TEXTURE", a3Txd);

    auto* npc = new entity(passFunc, entity::ENEMY, 3, {350.0f, 0.0f});
    processManager.attachProcess(npc);
    gM->attachEntity(npc);
    npc->spawn();

    // Create Boss
    auto* boss = new Boss(passFunc, {0.0f, 0.0f});
    processManager.attachProcess(boss);
    gM->attachEntity(boss);
    boss->spawn();

    // Create Heart Pickup
    auto* HeartTxd = new TxdLoader(passFunc, "../resource/HeartSS.png");
    processManager.attachProcess(HeartTxd);
    gM->attachTxd("HEART::TEXTURE", HeartTxd);

    auto* heart = new entity(passFunc, entity::ITEM_PICKUP, entity::HEART, {-50.0f, 0.0f});
    processManager.attachProcess(heart);
    gM->attachEntity(heart);
    heart->spawn();

    // Create Oxy Pickup
    auto* oxy = new entity(passFunc, entity::ITEM_PICKUP, entity::OXY_TANK, {0.0f, 50.0f});
    processManager.attachProcess(oxy);
    gM->attachEntity(oxy);
    oxy->spawn();

    print("Game Environment Created");
}

void GameInitializer::End() {

}