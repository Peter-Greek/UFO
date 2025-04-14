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
#include "MainMenu.h"
#include "Scheduler.h"

#include "MainMenu.h"
#include "SaveSelector.h"
#include "UpgradeMenu.h"
#include "UserInput.h"
#include "SettingsMenu.h"

class GameInitializer : public xProcess {
public:
    // replicate changes in upgrade menu
    enum GAME_RESULT {
        NONE,
        LOSE,
        ESCAPE,
        WIN_NO_ESCAPE,
        WIN_ESCAPE,
        WIN_ESCAPE_TOP_SCORE
    };
private:
    sh_ptr<MainMenu> mMenu;
    sh_ptr<UpgradeMenu> uMenu;
    sh_ptr<SaveSelector> sMenu;
    sh_ptr<SettingsMenu> setMenu;


    passFunc_t passFunc;
    sh_ptr<ProcessManager> processManager;
    sh_ptr<GameStorage> gameStorage;
    sh_ptr<Scheduler> sch;
    sh_ptr<GameManager> gameManager;

    std::list<sh_ptr<text>> debugTexts;
    std::list<sh_ptr<TxdLoader>> txdLoaders;

    float gameStartTime = 0.0f; // used for save data

    sh_ptr<UserInput> userInputBox; // used for user input

    GAME_RESULT gameResult = NONE;
public:
    GameInitializer(passFunc_t p1,
        sh_ptr<ProcessManager> pM,
        sh_ptr<GameStorage> loader,
        sh_ptr<Scheduler> sch_p)
    : xProcess(false, p1),
      passFunc(p1),
      processManager(std::move(pM)),
      gameStorage(std::move(loader)),
      sch(std::move(sch_p)) {}

    ~GameInitializer() override = default;



    template<typename T, typename... Args>
    sh_ptr<T> attachProcess(Args&&... args) {
        auto instance = std::make_shared<T>(passFunc, std::forward<Args>(args)...);
        processManager->attachProcess(instance);
        return instance;
    }

    template<typename T, typename... Args>
    sh_ptr<T> attachGameProcess(Args&&... args) {
        auto instance = std::make_shared<T>(passFunc, std::forward<Args>(args)...);
        processManager->attachProcess(instance);

        if constexpr (std::is_base_of_v<entity, T>) {
            gameManager->attachEntity(instance);
            instance->spawn();
        } else if constexpr (std::is_same_v<T, world>) {
            gameManager->setWorld(instance);
        } else if constexpr (std::is_same_v<T, camera>) {
            gameManager->setCamera(instance);
        }

        return instance;
    }

    template<typename T, typename... Args>
    sh_ptr<T> attachGameMappedProcess(std::string name, Args&&... args) {
        auto instance = std::make_shared<T>(passFunc, std::forward<Args>(args)...);
        processManager->attachProcess(instance);

        // text , aseprite, txd, audio
        if constexpr (std::is_base_of_v<text, T>) {
            gameManager->attachText(name, instance);
        } else if constexpr (std::is_base_of_v<AsepriteLoader, T>) {
            gameManager->attachAseprite(name, instance);
        } else if constexpr (std::is_base_of_v<TxdLoader, T>) {
            gameManager->attachTxd(name, instance);
        } else if constexpr (std::is_base_of_v<AudioLoader, T>) {
            gameManager->attachAudio(name, instance);
        }else if constexpr (std::is_base_of_v<Animation, T>) {
            gameManager->attachAnim(name, instance);
        }

        return instance;
    }

    template<typename T, typename... Args>
    sh_ptr<T> attachGameMappedNonProcess(std::string name, Args&&... args) {
        auto instance = std::make_shared<T>(std::forward<Args>(args)...);

        // text , aseprite, txd, audio
        if constexpr (std::is_base_of_v<Animation, T>) {
            gameManager->attachAnim(name, instance);
        }

        return instance;
    }

    template<typename T, typename... Args>
    sh_ptr<T> attachMappedProcess(std::string name, Args&&... args) {
        auto instance = std::make_shared<T>(passFunc, std::forward<Args>(args)...);
        processManager->attachProcess(instance);

        if constexpr (std::is_base_of_v<TxdLoader, T>) {
            txdLoaders.push_back(instance);
        }

        return instance;
    }




    void update(float deltaMs) override {};
    bool isDone() override { return false; };

    void Init();
    void Start();
    void End(GAME_RESULT result);
    void Debug();

    void LoadTextures();

    void LoadAudio();

    void LoadEntitiesFromWorld(sh_ptr<world> w);

    void GameDebug();

    void CreateMainMenu();

    void CreateUpgradeMenu();

    void ShutdownMainMenu();

    void ShutdownUpgradeMenu();

    void CreateSaveSelector();

    void ShutdownSaveSelector();

    void initializeUserInputBox();

    void ShutdownUserInputBox();

    void CreateSettingsMenu();

    void ShutdownSettingsMenu();
};


#endif //CSCI437_GAMEINITIALIZER_H
