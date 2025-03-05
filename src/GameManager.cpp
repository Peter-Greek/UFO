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
        std::list<entity*> removalList;
        for (auto& e : entityList) {
            vector2 currentCoords = e->getPosition();
            if  (cam != nullptr) {
                if (cam->isPointInView(currentCoords)) {
                    if (e->isEntityAPlayer()) {
                        updatePlayerView(true, e, deltaMs);
                    }else {
                        if (!e->isDone() && e->getHearts() > 0) {
                            vector2 screenCoords = cam->worldToScreenCoords(currentCoords); // convert world coords to screen coords
                            if (e->isEntityAnEnemy()) {
                                TriggerEvent("SDL::Render::SetDrawColor", 0, 0, 255, 255);
                                TriggerEvent("SDL::Render::DrawRect", screenCoords.x, screenCoords.y, 10, 10);
                                TriggerEvent("SDL::Render::ResetDrawColor");
                            }else if (e->isEntityAPickup()) {
                                if (e->getPickupType() == entity::AT) {
                                    TriggerEvent("SDL::Render::SetDrawColor", 0, 255, 255, 255);
                                    TriggerEvent("SDL::Render::DrawRect", screenCoords.x, screenCoords.y, 10, 10);
                                    TriggerEvent("SDL::Render::ResetDrawColor");
                                }else if (e->getPickupType() == entity::HEART) {
                                    TriggerEvent("SDL::Render::SetDrawColor", 255, 0, 0, 255);
                                    TriggerEvent("SDL::Render::DrawRect", screenCoords.x, screenCoords.y, 10, 10);
                                    TriggerEvent("SDL::Render::ResetDrawColor");
                                }
                            }
                        }else {
                            removalList.push_back(e);
                        }
                    }
                }else {
                    // Entity is not in view
                    if (e->isEntityAPlayer()) {
                        updatePlayerView(false, e, deltaMs);
                    }
                }
            }else {
                error("Camera not set in Game Manager");
            }
        }
        for (auto e : removalList) {
            entityList.remove(e);
        }
        removalList.clear();
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

void GameManager::updatePlayerView(bool isVisible, entity* e, float deltaMs) {
    if (!isVisible) {
        textMap["CamCoords"]->hideText();
        return;
    }


    auto* p = dynamic_cast<Player*>(e);
    vector2 currentCoords = e->getPosition();
    vector2 screenCoords = cam->worldToScreenCoords(currentCoords); // convert world coords to screen coords

    if (debugMode) {
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
        flip = p->isFacingLeft();
        currentFrame = animList["FSS_IDLE"]->getCurrentFrame(deltaMs);
    }

    SDL_Rect destRect = { static_cast<int>(screenCoords.x) - currentFrame.w, static_cast<int>(screenCoords.y) - currentFrame.h, currentFrame.w * 2, currentFrame.h * 2 };
    asepriteMap["FSS"]->renderFrame(currentFrame, destRect, flip, angle);
}

void GameManager::update(float deltaMs) {
    for (auto& e : entityList) {
        if (e->isEntityAnEnemy()) {
            handleEnemyUpdate(e);
        }else if (e->isEntityAPlayer()) {
            handlePlayerUpdate(e);
        }
    }
}

void GameManager::handlePlayerUpdate(entity* e) {
    auto* p = dynamic_cast<Player*>(e);
    vector2 currentCoords = e->getPosition();
    for (auto& e2 : entityList) {
        if (e2->isEntityAPickup()) {
            vector2 enemyCoords = e2->getPosition();
            if ((currentCoords - enemyCoords).length() < 25) {
                if (e2->getPickupType() == entity::AT) {
                    e2->setHearts(0);
                    e2->succeed();
                    p->addATCount();
                    textMap["ATScore"]->setText("AT: " + std::to_string(p->getATCount()));
                }else if (e2->getPickupType() == entity::HEART) {
                    e2->setHearts(0);
                    e2->succeed();
                    p->setHearts(p->getHearts() + 1);
                    textMap["PlayerHearts"]->setText("Hearts: " + std::to_string(p->getHearts()));
                }
            }
        }else if (e2->isEntityAnEnemy()) {
            vector2 enemyCoords = e2->getPosition();
            if ((currentCoords - enemyCoords).length() < 20) {
                if (e2->getHearts() > 0) {
                    if (p->doesPlayerHaveShield()) {
                        //TODO: Add shield hit sound and animation
                        p->hitShield();
                    }else {
                        //TODO: Add player hit sound and animation
                        p->setHearts(p->getHearts() - 1);
                    }

                    e2->setHearts(e2->getHearts() - 1);
                    if (e2->getHearts() == 0) {
                        e2->succeed();
                    }

                    bounceEntities(e, e2);

                    textMap["PlayerHearts"]->setText("Hearts: " + std::to_string(p->getHearts()));
                }
            }
        }
    }
}

void GameManager::handleEnemyUpdate(entity* e) {
    vector2 currentCoords = e->getPosition();
    vector2 curVel = e->getVelocity();
    vector2 newVel = vector2(0.0f, 0.0f);
    bool isClose = false;
    if (!e->isKnockedBack()) {
        // get the closest player if < 60 units away start to move towards player
        for (auto& e2 : entityList) {
            if (e2->isEntityAPlayer()) {
                vector2 playerCoords = e2->getPosition();
                if ((currentCoords - playerCoords).length() < (SCREEN_WIDTH / 4)) {
                    newVel = (playerCoords - currentCoords).normalize() * 0.05f;
                    isClose = true;
                }
                break;
            }
        }

        if (!isClose) {
            // if not close to spawn point move back to spawn point
            vector2 spawnCoords = e->getSpawnCoords();
            if ((currentCoords - spawnCoords).length() > 5) {
                newVel = (spawnCoords - currentCoords).normalize() * 0.05f;
            }
        }

    }else {
        if (curVel.length() == 0.0f) {
            e->setKnockedBack(false);
        }
    }

    if (!isClose) {
        if (curVel.y != 0.0f) {
            newVel.y = curVel.y * 0.25f; // reduce velocity
            if (newVel.y < 0.1f && newVel.y > -0.1f) {
                newVel.y = 0.0f; // stop the paddle
            }
        }

        if (curVel.x != 0.0f) {
            newVel.x = curVel.x * 0.25f; // reduce velocity
            if (newVel.x < 0.1f && newVel.x > -0.1f) {
                newVel.x = 0.0f; // stop the paddle
            }
        }
    }

    e->setVelocity(newVel);
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

void GameManager::bounceEntities(entity *e1, entity *e2) {
    //TODO: THIS NEEDS MAJOR REWORKING BUT IT WORKS FOR NOW
    vector2 e1Pos = e1->getPosition();
    vector2 e2Pos = e2->getPosition();

    vector2 e1Vel = e1->getVelocity();
    vector2 e2Vel = e2->getVelocity();

    vector2 e1NewVel = (e2Pos - e1Pos) * (e1Vel - 120) * (e1Vel.dot(e2Pos - e1Pos) / (e2Pos - e1Pos).lengthSquared());
    vector2 e2NewVel = (e1Pos - e2Pos) * (e2Vel - 120) * (e2Vel.dot(e1Pos - e2Pos) / (e1Pos - e2Pos).lengthSquared());

    e1->setVelocity(e1NewVel);
    e2->setVelocity(e2NewVel);
}
