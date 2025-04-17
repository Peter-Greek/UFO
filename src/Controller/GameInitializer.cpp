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

    // Create And Play Main Menu Music (Stays until the main game loop is started)
    CreateBackgroundMusic();

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

    // Main Menu Press Settings
    AddEventHandler("UFO::SetSettingsState", [this](bool state) {
        print("TODO: Open Settings");
        if (state) {
            CreateSettingsMenu();
        }else {
            ShutdownSettingsMenu();
            CreateMainMenu();
        }
    });

    // Main Menu Press Leaderboard
    AddEventHandler("UFO::OpenLeaderboard", [this]() {
        print("TODO: Open Leaderboard");
    });

    AddEventHandler("UFO::SaveSelector::Select", [this](int slotIndex) {
        print("Selected Slot: ", slotIndex);
        if ((*gameStorage)["saves"].size() <= slotIndex) {
            print("Invalid Slot Index");
            return;
        }

        ShutdownSaveSelector();
        (*gameStorage).SelectPlayer(slotIndex);
        ShutdownBackgroundMusic();
        CreateUpgradeMenu();
    });

    AddEventHandler("UFO::SaveSelector::Close", [this]() {
        print("Closing Save Selector");
        ShutdownSaveSelector();
        CreateMainMenu();
    });

    // Upgrade Menu Press Play
    AddEventHandler("UFO::UpgradeMenu::StartGameLoop", [this]() {
        ShutdownUpgradeMenu();
        ShutdownBackgroundMusic();
        CreateGameLoopBackgroundMusic();
        Start();
        GameDebug();
    });

    // Press ESC in upgrade menu
    AddEventHandler("UFO::UpgradeMenu::State", [this](bool state) {
        if (state == false && uMenu != nullptr) {
            ShutdownUpgradeMenu();
            CreateBackgroundMusic();
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
        if (upgrade == "at_cannon") max = 2;
        if (upgrade == "invisibility" or upgrade == "shield") max = 3;

        int up = (*gameStorage)["player"]["upgrades"][upgrade].get<int>();
        if (up < max) {
            (*gameStorage)["player"]["ATCount"] = curAT - atCount;
            (*gameStorage)["player"]["upgrades"][upgrade] = up + 1;
            (*gameStorage).SavePlayer();

            uMenu->setATCount((*gameStorage)["player"]["ATCount"].get<int>());
            uMenu->setOxygenCount((*gameStorage)["player"]["upgrades"]["oxygen"].get<int>());
            uMenu->setSpeedCount((*gameStorage)["player"]["upgrades"]["speed"].get<int>());  
            TriggerEvent("UFO::UpgradeMenu::DisplayATCount", (*gameStorage)["player"]["ATCount"].get<int>(), (*gameStorage)["player"]["upgrades"]["oxygen"].get<int>(), (*gameStorage)["player"]["upgrades"]["speed"].get<int>());
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

        int thisEnding = 0;
        if (beatBoss) thisEnding += 2; // the good ending
        if (isDead) thisEnding -= 1; // make it a bad ending if we are dead

        print("Ending Game Loop, AT Count: ", atcount);
        int curEnding = 0;
        if ((*gameStorage)["player"]["Ending"] != nullptr) {
            curEnding = (*gameStorage)["player"]["Ending"].get<int>();
        }

        (*gameStorage)["player"]["Ending"] = std::max(curEnding, thisEnding); // which ever ending is better save
        (*gameStorage)["player"]["ATCount"] = (*gameStorage)["player"]["ATCount"].get<int>() + atcount;
        (*gameStorage)["player"]["TotalAT"] = (*gameStorage)["player"]["TotalAT"].get<int>() + atcount;
        (*gameStorage)["player"]["time"] = (*gameStorage)["player"]["time"].get<int>() + int (sch->getGameTime() - gameStartTime);

        (*gameStorage).SavePlayer();
        print("Saved Player Data!");

        GAME_RESULT res = GAME_RESULT::NONE;
        if (beatBoss) {
            if (isDead) {
                res = GAME_RESULT::WIN_NO_ESCAPE;
            } else {
                res = GAME_RESULT::WIN_ESCAPE;
            }
        }else {
            if (isDead) {
                res = GAME_RESULT::LOSE;
            } else {
                res = GAME_RESULT::ESCAPE;
            }
        }

        End(res);
    });

    // Only triggered when main menu is open / settings menu
    AddEventHandler("UFO::View::UpdateWindowSize", [this](int w, int h) {
        print("Game Initializer: Window Size Resetting main menu: ", mMenu, sMenu, uMenu, SCREEN_WIDTH, "x", SCREEN_HEIGHT);
        sch->setTimeout(100, [=]() {
            if (mMenu != nullptr) {
                ShutdownMainMenu();
                CreateMainMenu();
            }
        });
    });

    // Triggered when a new user is trying to be made
    AddEventHandler("UFO::SaveSelector::Create", [this]() {
        print("Creating New User Attempt");
        initializeUserInputBox();
    });

    // Triggered whenever a message is sent in any userinput including chatbox
    AddEventHandler("UFO::UserInput::NewInput", [this](UUID_t from, std::string message) {
        if (!userInputBox) {
            return;
        }

        if (userInputBox->getId() != from) {
            return;
        }

        if (message.empty()) {
            userInputBox->addMessage("Please enter a valid name!");
            return;
        }

        if (message.length() > 20) {
            userInputBox->addMessage("Name too long! Max length is 20 characters.");
            return;
        }

        if (message.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_") != std::string::npos) {
            userInputBox->addMessage("Invalid characters! Only letters, numbers, and underscores are allowed.");
            return;
        }

        // Check if the name already exists
        for (const auto& save : (*gameStorage)["saves"]) {
            if (save["name"].get<std::string>() == message) {
                userInputBox->addMessage("Name already exists! Please choose a different name.");
                return;
            }
        }

        // Create new save
        (*gameStorage).CreatePlayer(message);
        ShutdownUserInputBox();
        ShutdownSaveSelector();
        (*gameStorage).SelectPlayer((int) (*gameStorage)["saves"].size() - 1);
        CreateUpgradeMenu();
    });

    // Display the MainMenu and Debug text
    CreateMainMenu();
    Debug();
}

void GameInitializer::Start(){
    print("Game Start Called");
    gameStartTime = sch->getGameTime();
    gameState = GAME_LOOP;
    auto gM = attachProcess<GameManager>();
    gM->setProcessManager(processManager);
    gM->setScheduler(sch);
    gameManager = gM;

    gM->AddEventHandler("UFO::CHECK::UUID", [this](UUID_t id) {
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


    auto sc = getScaledCoords; // scale the coords of placed entities by resolution factor
    using v2 = vector2;
    // Create Laser
    auto laser = attachGameProcess<Laser>(sc(v2{-700.0f, 0.0f}), Heading (360 - 45 * 6), 500, 20, 10, 200000, 1, 1);
    laser->setSpin(true);

    auto laser2 = attachGameProcess<Laser>(sc(v2 {50.0f, 0.0f}), Heading (360 - 45 * 7), 500, 20, 1000, 3000, 1, 1);

    // Create NPC
    auto npc = attachGameProcess<entity>(entity::ENEMY, 3, sc(v2{350.0f, 0.0f}));

    // Create Boss
    auto boss = attachGameProcess<Boss>(sc(v2{0.0f, -550.0f}));

    // Create Heart Pickup

    auto heart = attachGameProcess<entity>(entity::ITEM_PICKUP, entity::HEART, sc(v2{-50.0f, 0.0f}));

    // Create Oxy Pickup
    auto oxy = attachGameProcess<entity>(entity::ITEM_PICKUP, entity::OXY_TANK, sc(v2{0.0f, 50.0f}));

    // Create Key Card Pickup
    auto key = attachGameProcess<entity>(entity::ITEM_PICKUP, entity::KEY_CARD, sc(v2{0.0f, 250.0f}));

    // Create Escape Pod "Pickup"
    auto escape = attachGameProcess<entity>(entity::ITEM_PICKUP, entity::ESCAPE_POD, sc(v2{0.0f, 550.0f}));

    print("Game Environment Created");
}

void GameInitializer::update(float deltaMs) {
    if (gameState == GAME_LOOP) {
        auto player = gameManager->getPlayer();
        float maxOxy = player->getMaxOxygenTime();
        float oxy = player->getOxygenLevel();
        float oxyPercent = oxy / maxOxy;
        int intensityLevel = -1;

        if (oxyPercent < 0.10f) {
            intensityLevel = 3;
        } else if (oxyPercent < 0.20f) {
            intensityLevel = 2;
        } else if (oxyPercent < 0.40f) {
            intensityLevel = 1;
        } else if (oxyPercent < 0.75f) {
            intensityLevel = 0;
        }

        if (intensityLevel != lastIntensityLevel) {
            for (int i = 0; i < 4; ++i) {
                if (i == intensityLevel) {
                    gameLoopMusic->enableTrack(i);
                } else {
                    gameLoopMusic->disableTrack(i);
                }
            }
        }
    }else {
        lastIntensityLevel = -1;
    }
}

void GameInitializer::End(GAME_RESULT result) {
    gameResult = result;
    TriggerEvent("UFO::OnGameEnd"); // clean up the current game environment

    gameManager->abort();

    gameManager = nullptr;

    print("Game Loop Ended");

    //TODO: Add in some game result screen

    ShutdownGameLoopBackgroundMusic();
    CreateUpgradeMenu();
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

void GameInitializer::LoadTextures() {
    print("Loading Textures");
    // [[Asperite Textures]]


    // Create Player Anim
    auto fAnim = attachGameMappedProcess<AsepriteLoader>("FSS", "../resource/GFX/sprites/FSS.png", "../resource/json/FSS.json");
        // [[Animations]]
        auto fAnimIdle = attachGameMappedNonProcess<Animation>("FSS_IDLE", fAnim->getJSONData(), "Ferret Sprite Sheet (Idle)");
        auto fAnimMove = attachGameMappedNonProcess<Animation>("FSS_MOVE", fAnim->getJSONData(), "Ferret Sprite Sheet (Movement)");



    // [[Normal Textures]]


    // Create Wall Texture
    auto wTxd = attachGameMappedProcess<TxdLoader>("WALL::TEXTURE", "../resource/GFX/world/wall.png");

    // Create AT Texture
    auto ATTxd = attachGameMappedProcess<TxdLoader>("AT::TEXTURE", "../resource/GFX/icons/ATLoot1.png");

    // Create Laser Texture
    auto laserTxd = attachGameMappedProcess<TxdLoader>("LASER::TEXTURE", "../resource/GFX/icons/LaserBeams.png");

    // Create NPC Txds
    auto a1Txd =  attachGameMappedProcess<TxdLoader>("ALIEN1::TEXTURE", "../resource/GFX/sprites/Alien1.png");
    auto a2Txd = attachGameMappedProcess<TxdLoader>("ALIEN2::TEXTURE", "../resource/GFX/sprites/Alien2.png");
    auto a3Txd = attachGameMappedProcess<TxdLoader>("ALIEN3::TEXTURE", "../resource/GFX/sprites/Alien3.png");

    // Create Heart Texture
    auto HeartTxd = attachGameMappedProcess<TxdLoader>("HEART::TEXTURE", "../resource/GFX/icons/HeartSS.png");

    // Create Oxy Texture
    auto OxyTxd = attachGameMappedProcess<TxdLoader>("OXY_TANK::TEXTURE", "../resource/GFX/icons/OxygenUpgradeIcon.png");

    // Create Key Card Textures
    auto keyTxd =  attachGameMappedProcess<TxdLoader>("KEY_CARD1::TEXTURE", "../resource/GFX/icons/Keycard1.png");
    auto key2Txd = attachGameMappedProcess<TxdLoader>("KEY_CARD2::TEXTURE", "../resource/GFX/icons/Keycard2.png");

}

void GameInitializer::LoadAudio() {
    print("Loading Audio");
    // [[Audio]]
    auto aHitMarker = attachGameMappedProcess<AudioLoader>("hitmarker", "../resource/audio/hitmarker.wav", false);
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

void GameInitializer::CreateBackgroundMusic() {
    ShutdownBackgroundMusic();
    ShutdownGameLoopBackgroundMusic();
    mainMenuMusic = attachProcess<AudioLoader>("../resource/audio/MenuThemePreSynthLoopReady.wav", true);
    mainMenuMusic->setVolume(VOLUME_MUSIC / 100.0f);
    mainMenuMusic->play();
}

void GameInitializer::ShutdownBackgroundMusic() {
    if (mainMenuMusic != nullptr) {
        mainMenuMusic->stop();
        mainMenuMusic->abort();
        mainMenuMusic = nullptr;
    }
}

void GameInitializer::CreateGameLoopBackgroundMusic() {
    print("[GameInit] Creating Game Loop Music");
    ShutdownBackgroundMusic();
    ShutdownGameLoopBackgroundMusic();
    gameLoopMusic = attachProcess<AudioLoader>("../resource/audio/TestBase.wav", true);
    gameLoopMusic->attachTrack("../resource/audio/Drum1.wav"); // index: 0
    gameLoopMusic->attachTrack("../resource/audio/Drum2.wav"); // index: 1
    gameLoopMusic->attachTrack("../resource/audio/Drum3.wav"); // index: 2
    gameLoopMusic->attachTrack("../resource/audio/Drum4.wav"); // index: 3
//    gameLoopMusic->enableTrack(0);
//    gameLoopMusic->enableTrack(1);
//    gameLoopMusic->enableTrack(2);
    gameLoopMusic->setTrackVolume(0, VOLUME_MUSIC / 100.0f);
    gameLoopMusic->setTrackVolume(1, VOLUME_MUSIC / 100.0f);
    gameLoopMusic->setTrackVolume(2, VOLUME_MUSIC / 100.0f);
    gameLoopMusic->setTrackVolume(3, VOLUME_MUSIC / 100.0f);


    gameLoopMusic->setVolume(VOLUME_MUSIC / 100.0f);
    gameLoopMusic->play();
}

void GameInitializer::ShutdownGameLoopBackgroundMusic() {
    print("[GameInit] Shutting Down Game Loop Music");
    if (gameLoopMusic != nullptr) {
        gameLoopMusic->stop();
        gameLoopMusic->abort();
        gameLoopMusic = nullptr;
    }
}

void GameInitializer::CreateMainMenu() {
    ShutdownSaveSelector();
    ShutdownUpgradeMenu();
    gameState = MAIN_MENU;
    (*gameStorage).ResetPlayer();
    auto menuTxd = attachMappedProcess<TxdLoader>("MENU::TEXTURE", "../resource/GFX/screens/MainMenuV2.png");
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
    gameState = SAVE_SELECTOR;
    auto menuTxd = attachMappedProcess<TxdLoader>("SAVE_MENU::TEXTURE", "../resource/GFX/screens/SpaceBackground.png");
    sMenu = attachProcess<SaveSelector>((*gameStorage)["saves"], menuTxd);
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
    gameState = UPGRADE_MENU;
    auto dTxd = attachMappedProcess<TxdLoader>("DEATH_SCREEN::TEXTURE", "../resource/GFX/screens/deathScreen.png");
    auto eTxd = attachMappedProcess<TxdLoader>("ESCAPE_SCREEN::TEXTURE", "../resource/GFX/screens/escapeScreen.png");
    auto wTxd = attachMappedProcess<TxdLoader>("WIN_SCREEN::TEXTURE", "../resource/GFX/screens/escapeScreenWin2.png");
    auto menuTxd = attachMappedProcess<TxdLoader>("MENU::TEXTURE", "../resource/GFX/screens/SpaceBackground.png");
    auto rocketTxd = attachMappedProcess<TxdLoader>("ROCKET::TEXTURE", "../resource/GFX/icons/StartRocketIcon.png");
    uMenu = attachProcess<UpgradeMenu>(gameResult, dTxd, eTxd, wTxd, menuTxd, rocketTxd);
    uMenu->setATCount((*gameStorage)["player"]["ATCount"].get<int>());
    uMenu->setOxygenCount((*gameStorage)["player"]["upgrades"]["oxygen"].get<int>());
    uMenu->setSpeedCount((*gameStorage)["player"]["upgrades"]["speed"].get<int>()); 
    uMenu->showUpgradeMenu();
    gameResult = GAME_RESULT::NONE;
}

void GameInitializer::ShutdownUpgradeMenu() {
    if (uMenu != nullptr) {
        uMenu->abort();
        uMenu = nullptr;
    }
}

void GameInitializer::initializeUserInputBox() {
    ShutdownUserInputBox();

    userInputBox = attachProcess<UserInput>();
    userInputBox->addMessage("Enter a name for your new save slot:");
    userInputBox->showChatBox();
}

void GameInitializer::ShutdownUserInputBox() {
    if (userInputBox != nullptr) {
        userInputBox->abort();
        userInputBox = nullptr;
    }
}

void GameInitializer::CreateSettingsMenu() {
    ShutdownMainMenu();
    ShutdownSaveSelector();
    gameState = SETTING_MENU;
    setMenu = attachProcess<SettingsMenu>(gameStorage);
}

void GameInitializer::ShutdownSettingsMenu() {
    if (setMenu != nullptr) {
        setMenu->abort();
        setMenu = nullptr;
    }
}


