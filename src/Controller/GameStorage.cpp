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

#include "GameStorage.h"


int GameStorage::apply() {
    gameStorage["settings"] = gameStorage["settings"] != nullptr ? gameStorage["settings"] : json::object();
    if (gameStorage["settings"]["unlimitedFrames"] != nullptr) {
        unlimitedFrames = gameStorage["settings"]["unlimitedFrames"].get<bool>();
    }

    if (gameStorage["settings"]["debugMode"] != nullptr) {
        debugMode = gameStorage["settings"]["debugMode"].get<int>();
    }

    if (gameStorage["settings"]["curRoomIndex"] != nullptr) {
        curRoomIndex = gameStorage["settings"]["curRoomIndex"].get<int>();
    }

    if (gameStorage["settings"]["targetFPS"] != nullptr) {
        targetFPS = gameStorage["settings"]["targetFPS"].get<int>();
    }

    if (gameStorage["settings"]["AUDIO_ENABLED"] != nullptr) {
        AUDIO_ENABLED = gameStorage["settings"]["AUDIO_ENABLED"].get<bool>();
    }

    if (gameStorage["settings"]["VOLUME_MUSIC"] != nullptr) {
        VOLUME_MUSIC = gameStorage["settings"]["VOLUME_MUSIC"].get<float>();
    }

    if (gameStorage["settings"]["VOLUME_SFX"] != nullptr) {
        VOLUME_SFX = gameStorage["settings"]["VOLUME_SFX"].get<float>();
    }

    if (gameStorage["settings"]["FULL_SCREEN_ENABLED"] != nullptr) {
        FULL_SCREEN_ENABLED = gameStorage["settings"]["FULL_SCREEN_ENABLED"].get<bool>();
    }

    if (gameStorage["settings"]["SCREEN_RESOLUTION"] != nullptr) {
        SCREEN_RESOLUTION = gameStorage["settings"]["SCREEN_RESOLUTION"].get<std::pair<int, int>>();
    }



    updateSettings(); // Update non stored settings based on the new changes
    return 0;
}

int GameStorage::load() {
    gameStorage.update();

    // if no saves exist ensure we create a default save
    if (gameStorage["saves"] == nullptr || gameStorage["saves"].size() == 0) {
        gameStorage["saves"] = json::array();
        gameStorage["saves"][0] = json::object();

        gameStorage["saves"][0]["name"] = "Peter Greek";
        gameStorage["saves"][0]["ATCount"] = 0;
        gameStorage["saves"][0]["TotalAT"] = 0;
        gameStorage["saves"][0]["loads"] = 0;
        gameStorage["saves"][0]["time"] = 0; // play time
        // creation date
        gameStorage["saves"][0]["date"] = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        gameStorage["saves"][0]["upgrades"] = json::object();
        gameStorage["saves"][0]["upgrades"]["oxygen"] = 0;
        gameStorage["saves"][0]["upgrades"]["shield"] = 0;
        gameStorage["saves"][0]["upgrades"]["speed"] = 0;
        gameStorage["saves"][0]["upgrades"]["invisibility"] = 0;
        gameStorage["saves"][0]["upgrades"]["at_cannon"] = 0;

        gameStorage["saves"][0]["settings"] = json::object();
        gameStorage["saves"][0]["settings"]["unlimitedFrames"] = false;
        gameStorage["saves"][0]["settings"]["debugMode"] = 0;
        gameStorage["saves"][0]["settings"]["curRoomIndex"] = 0;
    }

    gameStorage["curPlayer"] = -1; // default to no player

    // Default Player
    gameStorage["player"] = json::object();
    gameStorage["player"]["ATCount"] = 0;
    gameStorage["player"]["TotalAT"] = 0;

    gameStorage["player"]["upgrades"] = json::object();
    gameStorage["player"]["upgrades"]["oxygen"] = 0;
    gameStorage["player"]["upgrades"]["shield"] = 0;
    gameStorage["player"]["upgrades"]["speed"] = 0;
    gameStorage["player"]["upgrades"]["invisibility"] = 0;
    gameStorage["player"]["upgrades"]["at_cannon"] = 0;

    apply();
    gameStorage["loads"] = gameStorage["loads"] != nullptr ? gameStorage["loads"].get<int>() + 1 : 1; // Increment the loads counter
    gameStorage.save();
    return 0;
}

void GameStorage::ResetPlayer() {
    print("Resetting Player");
    gameStorage["curPlayer"] = -1; // default to no player

    gameStorage["player"]["ATCount"] = 0;
    gameStorage["player"]["TotalAT"] = 0;

    gameStorage["player"]["upgrades"]["oxygen"] = 0;
    gameStorage["player"]["upgrades"]["shield"] = 0;
    gameStorage["player"]["upgrades"]["speed"] = 0;
    gameStorage["player"]["upgrades"]["invisibility"] = 0;
    gameStorage["player"]["upgrades"]["at_cannon"] = 0;

    gameStorage.save();
}

void GameStorage::CreatePlayer(const std::string& name) {
    print("Creating Player: ", name);
    gameStorage["saves"].push_back(json::object());
    gameStorage["saves"].back()["name"] = name;
    gameStorage["saves"].back()["ATCount"] = 0;
    gameStorage["saves"].back()["TotalAT"] = 0;
    gameStorage["saves"].back()["loads"] = 0;
    gameStorage["saves"].back()["time"] = 0; // play time
    gameStorage["saves"].back()["date"] = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    gameStorage["saves"].back()["upgrades"] = json::object();
    gameStorage["saves"].back()["upgrades"]["oxygen"] = 0;
    gameStorage["saves"].back()["upgrades"]["shield"] = 0;
    gameStorage["saves"].back()["upgrades"]["speed"] = 0;
    gameStorage["saves"].back()["upgrades"]["invisibility"] = 0;
    gameStorage["saves"].back()["upgrades"]["at_cannon"] = 0;

    gameStorage["saves"].back()["settings"] = json::object();
    gameStorage["saves"].back()["settings"]["unlimitedFrames"] = false;
    gameStorage["saves"].back()["settings"]["debugMode"] = 0;
    gameStorage["saves"].back()["settings"]["curRoomIndex"] = 0;

    gameStorage.save();
}

void GameStorage::SelectPlayer(int playerIndex) {
    print("Selected Player: ", playerIndex);
    gameStorage["saves"][playerIndex]["loads"] = gameStorage["saves"][playerIndex]["loads"].get<int>() + 1;
    gameStorage["curPlayer"] = playerIndex;
    gameStorage["player"] = gameStorage["saves"][playerIndex];
    gameStorage.save();
}

void GameStorage::SavePlayer() {
    print("Saving Player: ", gameStorage["curPlayer"].get<int>());
    gameStorage["saves"][gameStorage["curPlayer"].get<int>()] = gameStorage["player"];
    gameStorage.save();
}

jLoader& GameStorage::getStorage() {
    return gameStorage;
}

void GameStorage::save() {
    gameStorage.save();
}