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


// NOTE: Only create event handlers inside of methods that are not called multiple times as this class does not get
// destroyed when the game loop ends. This is to prevent multiple event handlers from being created and causing issues.
void GameInitializer::Init() {
    print("Game Init");

    // Config Changes
    AddEventHandler("UFO::OnConfigUpdate", [this](const std::string configName) {
        updateSettings(); // Update settings based on global variables

        if (configName == "unlimitedFrames") {
            (*gameStorage)["settings"]["unlimitedFrames"] = unlimitedFrames;
            gameStorage->save();
        } else if (configName == "debugMode") {
            (*gameStorage)["settings"]["debugMode"] = debugMode;
            gameStorage->save();
        }else if (configName == "curRoomIndex") {
            (*gameStorage)["settings"]["curRoomIndex"] = curRoomIndex;
            gameStorage->save();
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

    // Main Menu Press Start
    AddEventHandler("UFO::StartGame", [this]() {
        // Create Game Environment
        print("Starting Game...");
        ShutdownMainMenu();
        CreateSaveSelector();
    });

    AddEventHandler("UFO::SaveSelector::Select", [this](int slotIndex) {
        print("Selected Slot: ", slotIndex);
        if ((*gameStorage)["saves"].size() <= slotIndex) {
            print("Invalid Slot Index");
            return;
        }

        ShutdownSaveSelector();
        (*gameStorage).SelectPlayer(slotIndex);
        CreateUpgradeMenu();
    });

    // Upgrade Menu Press Play
    AddEventHandler("UFO::UpgradeMenu::StartGameLoop", [this]() {
        ShutdownUpgradeMenu();
        Start();
        GameDebug();
    });

    // Press ESC in upgrade menu
    AddEventHandler("UFO::UpgradeMenu::State", [this](bool state) {
        if (state == false && uMenu != nullptr) {
            ShutdownUpgradeMenu();
            CreateMainMenu();
        }
    });

    AddEventHandler("UFO::UpgradePurchased", [this](std::string upgrade, int atCount) {
        int curAT = (*gameStorage)["player"]["ATCount"].get<int>();
        if (curAT < atCount) {
            print("Not enough AT to purchase upgrade");
            TriggerEvent("UFO::Chat::AddMessage", "Not enough AT to purchase upgrade");
            return;
        }

        //setting max amound that a user can purchase depending on which upgrade they are buying
        int max = 5;
        if (upgrade == "at_cannon") max = 5;
        if (upgrade == "invisibility" or upgrade == "shield") max = 3;

        int up = (*gameStorage)["player"]["upgrades"][upgrade].get<int>();
        if (up < max) {
            (*gameStorage)["player"]["ATCount"] = curAT - atCount;
            (*gameStorage)["player"]["upgrades"][upgrade] = up + 1;
            (*gameStorage).SavePlayer();

            TriggerEvent("UFO::UpgradeMenu::DisplayATCount", (*gameStorage)["player"]["ATCount"].get<int>());
            print("Upgrade purchased: ", upgrade, " AT Count: ", atCount);
            TriggerEvent("UFO::Chat::AddMessage", "Upgrade purchased: " + upgrade + " AT Count: " + std::to_string(atCount));
        } else {
            print("Upgrade already maxed out");
            TriggerEvent("UFO::Chat::AddMessage", "Upgrade already maxed out");
        }
    });

    // GameManager Triggers Game End, we check if the player is dead or if the boss is dead
    AddEventHandler("UFO::EndGame", [this]() {
        int atcount = 0;
        bool isDead = true;
        bool beatBoss = false;

        auto player = gameManager->getPlayer();
        if (player != nullptr) {
            atcount = player->getATCount();

            if (player->getHearts() <= 0 || player->isDone()) {
                atcount = 0;
                isDead = true;
            }else {
                isDead = false;
            }
        }

        auto boss = gameManager->getBoss();
        if (boss == nullptr || boss->isDone() || boss->getHearts() <= 0) {
            beatBoss = true;
        }

        print("Ending Game Loop, AT Count: ", atcount);
        (*gameStorage)["player"]["ATCount"] = (*gameStorage)["player"]["ATCount"].get<int>() + atcount;
        (*gameStorage)["player"]["TotalAT"] = (*gameStorage)["player"]["TotalAT"].get<int>() + atcount;
        (*gameStorage)["player"]["time"] = (*gameStorage)["player"]["time"].get<int>() + int (sch->getGameTime() - gameStartTime);
        (*gameStorage).SavePlayer();

        End();
    });

    // Display the MainMenu and Debug text
    CreateMainMenu();
    Debug();
}

void GameInitializer::Start(){
    print("Game Start Called");
    gameStartTime = sch->getGameTime();
    auto gM = attachProcess<GameManager>();
    gM->setProcessManager(processManager);
    gameManager = gM;

    gM->AddEventHandler("UFO::CHECK::UUID", [this](UUID id) {
        print("Checking UUID: ", id);
        print(processManager->containsUUID(id));
    });

    LoadTextures(); // Load all textures
    LoadAudio();

    // Create World (HAS TO START BEFORE ANY ENTITY IS ATTACHED!)
    auto w = attachGameProcess<world>();
//    LoadEntitiesFromWorld(w);

    // Create Camera
    auto cam = attachGameProcess<camera>();

    // Get Player Upgrades
    upgradeList_t upgrades = {0, 0, 0, 0, 0}; // for debug purposes eventually will be loaded from json storage
    upgrades[Player::UPGRADES::OXYGEN]       = (*gameStorage)["player"]["upgrades"]["oxygen"].get<int>();
    upgrades[Player::UPGRADES::SHIELD]       = (*gameStorage)["player"]["upgrades"]["shield"].get<int>();
    upgrades[Player::UPGRADES::SPEED]        = (*gameStorage)["player"]["upgrades"]["speed"].get<int>();
    upgrades[Player::UPGRADES::INVISIBILITY] = (*gameStorage)["player"]["upgrades"]["invisibility"].get<int>();
    upgrades[Player::UPGRADES::AT_CANNON]    = (*gameStorage)["player"]["upgrades"]["at_cannon"].get<int>();

    // Create Player
    auto ppl = attachGameProcess<Player>(upgrades);


    // Create Random AT (Debug)
    for (int i = 0; i < 200; i++) {
        // random location for the AT
        vector2 coords = {static_cast<float>(random(0, SCREEN_WIDTH)), static_cast<float>(random(0, SCREEN_HEIGHT))};
        auto at = attachGameProcess<AT>(coords);
    }


    // Create Laser
    auto laser = attachGameProcess<Laser>(vector2 {-700.0f, 0.0f}, Heading (360 - 45 * 6), 500, 20, 10, 200000, 1, 1);
    laser->setSpin(true);

    auto laser2 = attachGameProcess<Laser>(vector2 {50.0f, 0.0f}, Heading (360 - 45 * 7), 500, 20, 1000, 3000, 1, 1);

    // Create NPC
    auto npc = attachGameProcess<entity>(entity::ENEMY, 3, vector2{350.0f, 0.0f});

    // Create Boss
    auto boss = attachGameProcess<Boss>(vector2{0.0f, -550.0f});

    // Create Heart Pickup

    auto heart = attachGameProcess<entity>(entity::ITEM_PICKUP, entity::HEART, vector2{-50.0f, 0.0f});

    // Create Oxy Pickup
    auto oxy = attachGameProcess<entity>(entity::ITEM_PICKUP, entity::OXY_TANK, vector2{0.0f, 50.0f});

    // Create Key Card Pickup
    auto key = attachGameProcess<entity>(entity::ITEM_PICKUP, entity::KEY_CARD, vector2{0.0f, 250.0f});

    // Create Escape Pod "Pickup"
    auto escape = attachGameProcess<entity>(entity::ITEM_PICKUP, entity::ESCAPE_POD, vector2{0.0f, 550.0f});

    print("Game Environment Created");
}

void GameInitializer::End() {
    TriggerEvent("UFO::OnGameEnd"); // clean up the current game environment

    gameManager->abort();

    gameManager = nullptr;

    print("Game Loop Ended");

    //TODO: Add in some game result screen

    uMenu = attachProcess<UpgradeMenu>();
    uMenu->setATCount((*gameStorage)["player"]["ATCount"].get<int>());
    uMenu->showUpgradeMenu();
}

void GameInitializer::Debug() {
    if (!debugTexts.empty()) {
        return;
    }
    print("Debug Mode: ", debugMode);
    // General Debug Text
    std::string fpsTextContent = "FPS: " + std::to_string(64);
    auto fpsText = attachProcess<text>(fpsTextContent);
    fpsText->setTextRelativePosition(0.0f, -0.8f);
    debugTexts.push_back(fpsText);

    std::string gameTimeContent = "Time: " + std::to_string(sch->getGameTime());
    auto gameTimeText = attachProcess<text>(gameTimeContent);
    gameTimeText->setTextRelativePosition(0.0f, 0.8f);
    debugTexts.push_back(gameTimeText);

    AddEventHandler("SDL::OnUpdate", [gameTimeText, fpsText, this](float deltaMs) {
        std::string fpsTextContent = "FPS: " + std::to_string(1000.0f/deltaMs);
        fpsText->setText(fpsTextContent);
        std::string gameTimeContent = "Time: " + std::to_string(sch->getGameTime());
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

void GameInitializer::GameDebug() {
    //Debug Text For Wall Creation
    for (int i = 0; i <= GameManager::db_WCS::WIDTH_SET; i++) {
        std::string txtName = "DEBUG::WALL::" + std::to_string(i);
        auto txt = attachGameMappedProcess<text>(txtName, "WALL: " + std::to_string(i), 35);
        txt->setTextRelativePosition(-1.0f, (-0.1f * (float) (i + 1)));
        txt->hideText();
    }

    // Display Current Player Information (Debug)
    auto atScore = attachGameMappedProcess<text>("ATScore", "AT: 0", 35);
    atScore->setTextRelativePosition(-1.0f, -0.7f);

    std::string pHeartsContent = "Hearts: 3";
    if (gameManager->getPlayer() != nullptr) {
        pHeartsContent = "Hearts: " + std::to_string(gameManager->getPlayer()->getHearts());
    }
    auto pHearts = attachGameMappedProcess<text>("PlayerHearts", pHeartsContent, 35);
    pHearts->setTextRelativePosition(-1.0f, -0.8f);

    auto oxyTime = attachGameMappedProcess<text>("OxyTimer", "Oxygen: 3:00", 35);
    oxyTime->setTextRelativePosition(-1.0f, -0.9f);

    // Debug Text for Camera
    auto cText = attachGameMappedProcess<text>("CamCoords", "X: 0.0, Y: 0.0", 35);
    cText->setTextRelativePosition(0.0f, -0.8f);

    auto rHeading = attachGameMappedProcess<text>("RelHeading", "Heading: 0", 35);
    rHeading->setTextRelativePosition(0.0f, -0.7f);
}

void GameInitializer::CreateMainMenu() {
    ShutdownSaveSelector();
    ShutdownUpgradeMenu();
    (*gameStorage).ResetPlayer();
    auto menuTxd = attachMappedProcess<TxdLoader>("MENU::TEXTURE", "../resource/MainMenuV2.png");
    mMenu = attachProcess<MainMenu>(menuTxd);
}

void GameInitializer::ShutdownMainMenu() {
    if (mMenu != nullptr) {
        mMenu->abort();
        mMenu = nullptr;
    }
}

void GameInitializer::CreateSaveSelector() {
    ShutdownMainMenu();
    sMenu = attachProcess<SaveSelector>((*gameStorage)["saves"]);
}

void GameInitializer::ShutdownSaveSelector() {
    if (sMenu != nullptr) {
        sMenu->abort();
        sMenu = nullptr;
    }
}

void GameInitializer::CreateUpgradeMenu() {
    ShutdownMainMenu();
    ShutdownSaveSelector();
    uMenu = attachProcess<UpgradeMenu>();
    uMenu->setATCount((*gameStorage)["player"]["ATCount"].get<int>());
    uMenu->showUpgradeMenu();
}

void GameInitializer::ShutdownUpgradeMenu() {
    if (uMenu != nullptr) {
        uMenu->abort();
        uMenu = nullptr;
    }
}

void GameInitializer::LoadTextures() {
    print("Loading Textures");
    // [[Asperite Textures]]


    // Create Player Anim
    auto fAnim = attachGameMappedProcess<AsepriteLoader>("FSS", "../resource/FSS.png", "../resource/FSS.json");
        // [[Animations]]
        auto fAnimIdle = attachGameMappedNonProcess<Animation>("FSS_IDLE", fAnim->getJSONData(), "Ferret Sprite Sheet (Idle)");
        auto fAnimMove = attachGameMappedNonProcess<Animation>("FSS_MOVE", fAnim->getJSONData(), "Ferret Sprite Sheet (Movement)");



    // [[Normal Textures]]


    // Create Wall Texture
    auto wTxd = attachGameMappedProcess<TxdLoader>("WALL::TEXTURE", "../resource/wall.png");

    // Create AT Texture
    auto ATTxd = attachGameMappedProcess<TxdLoader>("AT::TEXTURE", "../resource/ATLoot1.png");

    // Create Laser Texture
    auto laserTxd = attachGameMappedProcess<TxdLoader>("LASER::TEXTURE", "../resource/LaserBeams.png");

    // Create NPC Txds
    auto a1Txd =  attachGameMappedProcess<TxdLoader>("ALIEN1::TEXTURE", "../resource/Alien1.png");
    auto a2Txd = attachGameMappedProcess<TxdLoader>("ALIEN2::TEXTURE", "../resource/Alien2.png");
    auto a3Txd = attachGameMappedProcess<TxdLoader>("ALIEN3::TEXTURE", "../resource/Alien3.png");

    // Create Heart Texture
    auto HeartTxd = attachGameMappedProcess<TxdLoader>("HEART::TEXTURE", "../resource/HeartSS.png");
}

void GameInitializer::LoadAudio() {
    print("Loading Audio");
    // [[Audio]]
    auto aHitMarker = attachGameMappedProcess<AudioLoader>("hitmarker", "../resource/sfx/hitmarker.wav");
}

void GameInitializer::LoadEntitiesFromWorld(sh_ptr<world> w) {
    print("Loading Entities from World");
    w->loadWorld(); // loads the world
    json worldEntityList = w->getAllEntities();

    for (auto &worldE : worldEntityList) {
        int hearts = 0;
        if (worldE["hearts"] == nullptr) {
            hearts = worldE["pType"].get<int>();
        } else {
            hearts = worldE["hearts"].get<int>();
        }

        if (worldE["eType"] == nullptr) continue;

        auto eType = static_cast<entity::eType>(worldE["eType"].get<int>());
        vector2 coords = worldE["coords"].get<vector2>();

        switch (eType) {
            case entity::PLAYER:
                break;
            case entity::ENEMY:
            case entity::PROJECTILE: {
                auto e = attachGameProcess<entity>(eType, hearts, coords);
                break;
            }

            case entity::ITEM_PICKUP: {
                if (static_cast<entity::pType>(hearts) == entity::pType::AT) {
                    auto e = attachGameProcess<AT>(coords);
                } else {
                    auto e = attachGameProcess<entity>(entity::ITEM_PICKUP, hearts, coords);
                }
                break;
            }

            case entity::LASER: {
                auto heading = Heading(worldE["heading"].get<float>());
                int length = worldE["length"].get<int>();
                int width = worldE["width"].get<int>();
                int interval = worldE["interval"].get<int>();
                int duration = worldE["duration"].get<int>();
                int damage = worldE["damage"].get<int>();
                int speed = worldE["speed"].get<int>();

                auto laser = attachGameProcess<Laser>(coords, heading, length, width, interval, duration, damage, speed);
                if (worldE["spin"] != nullptr) {
                    laser->setSpin(worldE["spin"].get<bool>());
                }
                break;
            }

            case entity::ENEMY_BOSS:
                break;
        }
    }
}
