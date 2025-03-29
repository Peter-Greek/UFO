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

    updateSettings(); // Update non stored settings based on the new changes
    return 0;
}

int GameStorage::load() {
    gameStorage.update();

    if (gameStorage["player"] == nullptr) {
        gameStorage["player"] = json::object();
        gameStorage["player"]["ATCount"] = 0;
        gameStorage["player"]["TotalAT"] = 0;
    }

    if (gameStorage["player"]["ATCount"] == nullptr) {
        gameStorage["player"]["ATCount"] = 0;
        gameStorage.save();
    }

    if (gameStorage["player"]["TotalAT"] == nullptr) {
        gameStorage["player"]["TotalAT"] = 0;
        gameStorage.save();
    }

    if (gameStorage["player"]["upgrades"] == nullptr) {
        gameStorage["player"]["upgrades"] = json::object();
        gameStorage["player"]["upgrades"]["oxygen"] = 0;
        gameStorage["player"]["upgrades"]["shield"] = 0;
        gameStorage["player"]["upgrades"]["speed"] = 0;
        gameStorage["player"]["upgrades"]["invisibility"] = 0;
        gameStorage["player"]["upgrades"]["at_cannon"] = 0;
        gameStorage.save();
    }

    apply();
    gameStorage["loads"] = gameStorage["loads"] != nullptr ? gameStorage["loads"].get<int>() + 1 : 1; // Increment the loads counter
    gameStorage.save();
    return 0;
}

jLoader& GameStorage::getStorage() {
    return gameStorage;
}

void GameStorage::save() {
    gameStorage.save();
}