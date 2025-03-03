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

                    if (e->isEntityAPlayer()) {
                        // cast to player
                        auto* p = dynamic_cast<Player*>(e);



                        if (debugMode && 0) {
                            // convert screenCoords.x and screenCoords.y to string with only 2 decimal places
                            std::string xString = std::to_string(currentCoords.x);
                            std::string yString = std::to_string(currentCoords.y);
                            xString = xString.substr(0, xString.find(".") + 3);
                            yString = yString.substr(0, yString.find(".") + 3);
                            std::string coords = "X: " + xString + " Y: " + yString;
                            textMap["CamCoords"]->showText(coords);
                            textMap["CamCoords"]->setTextPosition(screenCoords.x, screenCoords.y - 40);
                        }else {
                            textMap["CamCoords"]->hideText();
                        }

                        for (auto& e2 : entityList) {
                            if (e2->isEntityAPickup()) {
                                vector2 enemyCoords = e2->getPosition();
                                if ((currentCoords - enemyCoords).length() < 25) {
                                    if (e2->getHearts() == 1) {
                                        // is at
                                        e2->setHearts(0);
                                        e2->succeed();
                                        p->addATCount();
                                    }
                                }
                            }
                        }

                        cam->updateCamera(currentCoords - vector2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2));

                        vector2 playerVel = e->getVelocity();
                        SDL_Rect currentFrame;

                        if (!animList.count("FSS_IDLE")) {
                            animList["FSS_IDLE"] = new Animation(asepriteMap["FSS"]->getJSONData(), "Ferret Sprite Sheet (Idle)");
                        }

                        if (!animList.count("FSS_MOVE")) {
                            animList["FSS_MOVE"] = new Animation(asepriteMap["FSS"]->getJSONData(), "Ferret Sprite Sheet (Movement)");
                        }

                        bool flip = false;
                        int angle = 0;

                        if (playerVel.x != 0 || playerVel.y != 0) {
                            currentFrame = animList["FSS_MOVE"]->getCurrentFrame(deltaMs);
                            if (playerVel.x < 0) {
                                flip = true;
                            }


                            if (playerVel.y != 0) {
                                angle = playerVel.y > 0 ? 90 : -90;
                                if (playerVel.x < 0) {
                                    angle = playerVel.y > 0 ? -45 : 45;
                                }else if (playerVel.x > 0) {
                                    angle = playerVel.y > 0 ? 45 : -45;
                                }
                            }
                        }else {
                            currentFrame = animList["FSS_IDLE"]->getCurrentFrame(deltaMs);
                        }

                        SDL_Rect destRect = { static_cast<int>(screenCoords.x) - currentFrame.w, static_cast<int>(screenCoords.y) - currentFrame.h, currentFrame.w * 2, currentFrame.h * 2 };
                        asepriteMap["FSS"]->renderFrame(currentFrame, destRect, flip, angle);
                    }else {
                        if (!e->isDone() && e->getHearts() > 0) {
                            TriggerEvent("SDL::Render::DrawRect", screenCoords.x, screenCoords.y, 10, 10);
                        }else {
                            //TODO: Remove entity from list
                        }
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

void GameManager::attachAseprite(std::string name, AsepriteLoader* a) {
    asepriteMap[name] = a;
}

void GameManager::attachText(std::string name, text *t) {
    textMap[name] = t;
}

void GameManager::setCamera(camera* c) {
    cam = c;
}