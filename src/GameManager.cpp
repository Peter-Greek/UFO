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
// Created by xerxe on 2/27/2025.
//

#include "GameManager.h"

int GameManager::initialize() {
    gameRunning = true;
    print("Game Manager Initialize");


    AddEventHandler("SDL::OnUpdate", [this](float deltaMs) {
        // Update Before render
        for (auto& e : entityList) {
            vector2 currentCoords = e->getPosition();
            if  (cam != nullptr) {
                if (cam->isPointInView(currentCoords)) {
                    vector2 screenCoords = cam->worldToScreenCoords(currentCoords);
                    TriggerEvent("SDL::Render::DrawRect", screenCoords.x, screenCoords.y, 10, 10);
                    if (e->isEntityAPlayer()) {
                        if (debugMode) {
                            // convert screenCoords.x and screenCoords.y to string with only 2 decimal places
                            std::string xString = std::to_string(currentCoords.x);
                            std::string yString = std::to_string(currentCoords.y);
                            xString = xString.substr(0, xString.find(".") + 3);
                            yString = yString.substr(0, yString.find(".") + 3);
                            std::string coords = "X: " + xString + " Y: " + yString;
                            textMap["CamCoords"]->showText(coords);
                            textMap["CamCoords"]->setTextPosition(screenCoords.x, screenCoords.y - 40);
                        }
                        cam->updateCamera(currentCoords - vector2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2));
                    }

                }else {
                    // Entity is not in view
                    //print("Cant see entity: ", e, e->isEntityAPlayer(), e->isEntityAnEnemy(), e->isEntityAnEnemyBoss());
                    if (debugMode && e->isEntityAPlayer()) {
                        textMap["CamCoords"]->hideText();
                    }
                }
            }else {
                error("Camera not set in Game Manager");
            }
        }
    });

    AddEventHandler("SDL::OnPollEvent", [this](int eventType, int key) {
        // Poll Event
        if (eventType == SDL_KEYDOWN) {
            if (key == SDLK_c) {
                // print cur coords of the player
                print("Player Coords: ", entityList.front()->getPosition());
            }
        }
    });

    return 1;
}

void GameManager::update(float deltaMs) {
}

void GameManager::attachEntity(entity* e) {
    entityList.push_back(e);
}

void GameManager::setCamera(camera* c) {
    cam = c;
}

void GameManager::attachText(std::string name, text *t) {
    textMap[name] = t;
}



