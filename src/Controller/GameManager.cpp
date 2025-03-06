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
    print("Game Manager Initialize");

    AddEventHandler("SDL::OnUpdate", [this](float deltaMs) {
        if (!gameRunning) {return;}
        if (cam == nullptr) {error("Camera not set in Game Manager");}
        // Update Before render
        renderWorld();
        for (auto& e : entityList) {
            bool isPlayer = e->isEntityAPlayer();
            vector2 currentCoords = e->getPosition();
            bool inView = cam->isPointInView(currentCoords);

            // Update Player View
            if (isPlayer) {
                updatePlayerView(inView, e, deltaMs);
                continue;
            }

            // If entity is done or out of hearts
            if (e->isDone() || e->getHearts() <= 0) {
                continue;
            }

            // If not in view, skip rendering
            if (!inView) {
                continue;
            }

            vector2 screenCoords = cam->worldToScreenCoords(currentCoords); // convert world coords to screen coords
            vector2 dim = e->getDimensions(); // get the dimensions of the entity (length, width)
            if (e->isEntityAnEnemy()) {
                renderEnemy(screenCoords, dim, e);
            }else if (e->isEntityAPickup()) {
                if (e->getPickupType() == entity::AT) {
                    TriggerEvent("SDL::Render::SetDrawColor", 0, 255, 255, 255);
                    TriggerEvent("SDL::Render::DrawRect", screenCoords.x, screenCoords.y, dim.x, dim.y);
                    TriggerEvent("SDL::Render::ResetDrawColor");
                }else if (e->getPickupType() == entity::HEART) {
                    TriggerEvent("SDL::Render::SetDrawColor", 255, 0, 0, 255);
                    TriggerEvent("SDL::Render::DrawRect", screenCoords.x, screenCoords.y, dim.x, dim.y);
                    TriggerEvent("SDL::Render::ResetDrawColor");
                }else if (e->getPickupType() == entity::OXY_TANK) {
                    TriggerEvent("SDL::Render::SetDrawColor", 0, 255, 0, 255);
                    TriggerEvent("SDL::Render::DrawRect", screenCoords.x, screenCoords.y, dim.x, dim.y);
                    TriggerEvent("SDL::Render::ResetDrawColor");
                }
            }else if (e->isEntityALaser()) {
                auto* l = dynamic_cast<Laser*>(e);
                if (l && l->isFiring()) {
                    renderLaser(screenCoords, dim, l);
                }
            }
        }
    });

    AddEventHandler("SDL::OnPollEvent", [this](int eventType, int key) {
        if (debugMode == 1) {
            if (eventType == SDL_KEYDOWN) {
                if (key == SDLK_c) {
                    // print cur coords of the player
                    print("Player Coords: ", entityList.front()->getPosition());
                }
            }
        }
    });

    gameRunning = true;
    return 1;
}

// Update View Functions
void GameManager::updatePlayerView(bool isVisible, entity* e, float deltaMs) {
    if (!isVisible) {
        textMap["CamCoords"]->hideText();
        return;
    }

    auto* p = dynamic_cast<Player*>(e);
    vector2 currentCoords = e->getPosition();
    vector2 screenCoords = cam->worldToScreenCoords(currentCoords); // convert world coords to screen coords

    if (debugMode) {

        // RelHeading
        int x, y;
        SDL_GetMouseState(&x, &y);
        vector2 mouseCoords = cam->screenToWorldCoords(vector2(x, y));
        Heading h = getHeadingFromVectors(currentCoords, mouseCoords);
        textMap["RelHeading"]->showText("Heading: " + std::to_string(h.get()));


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
        textMap["RelHeading"]->hideText();
    }

    textMap["OxyTimer"]->setText("Oxygen: " + p->getOxygenString());

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
    if (p->isInvisible()) {
        asepriteMap["FSS"]->setTextureAlpha(80);
    }
    asepriteMap["FSS"]->renderFrame(currentFrame, destRect, flip, angle);
    asepriteMap["FSS"]->resetTextureAlpha();
}

void GameManager::renderEnemy(vector2 screenCoords, vector2 dim, entity* e) {
    auto it = txdMap.find("ALIEN::TEXTURE");
    if (it == txdMap.end() || !it->second) {
        return;
    }
    int textureSize = 160;
    SDL_Rect srcRect = {0, 0, textureSize, textureSize}; // load the entire texture

    SDL_Rect destRect = {
            static_cast<int>(screenCoords.x),
            static_cast<int>(screenCoords.y),
            32, 32 // down scale the texture
    };
    txdMap["ALIEN::TEXTURE"]->render(srcRect, destRect, 0, SDL_FLIP_NONE);

    TriggerEvent("SDL::Render::SetDrawColor", 0, 0, 255, 255);
    TriggerEvent("SDL::Render::DrawRect", screenCoords.x, screenCoords.y, dim.x, dim.y);
    TriggerEvent("SDL::Render::ResetDrawColor");
}

void GameManager::renderLaser(vector2 screenCoords, vector2 dim, Laser* l) {
    Heading h = l->getHeading(); // Number from 0 to 360
    vector2 laserStart = {screenCoords.x, screenCoords.y};
    int length = dim.x;
    int width = dim.y;

    // Check if the texture exists in txdMap
    auto it = txdMap.find("LASER::TEXTURE");
    if (it == txdMap.end() || !it->second) {
        return;
    }

    // Green Laser 40 41 --> 380 76
    // Green Laser no ball 72 42 --> 380 76

    int textureSize = 128;
    int lasers = std::max(1, length / textureSize); // Number of laser segments to render


    // Convert heading into a direction vector
    vector2 direction = angleToVector2(h);
    for (int i = 0; i < lasers; i++) {
        // Compute new laser segment position in the correct direction
        vector2 laserEnd = laserStart + (direction * textureSize);
        SDL_Rect srcRect;
        SDL_Rect destRect;

        if (i == 0) {
            srcRect = {40, 41, 380 - 40, 76 - 41};
        }else {
            srcRect = {73, 41, 380 - 72, 76 - 41};
        }

        destRect = {
                static_cast<int>(laserStart.x),
                static_cast<int>(laserStart.y),
                textureSize, width
        };

        // Render laser segment with rotation
        txdMap["LASER::TEXTURE"]->render(srcRect, destRect, h, SDL_FLIP_NONE);

        // Move to the next segment
        laserStart = laserEnd;
    }
}

void GameManager::renderWorld() {
    // loop through worldData.rooms and then draw each wall in it
    if (worldMap == nullptr) {
        error("World Map not set in Game Manager");
        return;
    }

    if (debugMode == 1) {
        print("Draw World");
        jsonLoader worldData = worldMap->getWorldData();
        if (worldData["rooms"].empty() == 0) {
            for (auto& room : worldData["rooms"]) {
                for (auto& wall : room["walls"]) {
                    // make dashed line for each wall in the direction of center following h till length
                    // use triggerevent SDL::Render::DrawPoint
                    auto h = (Heading)wall["h"].get<int>();
                    int len = wall["l"].get<int>();
                    int wid = wall["w"].get<int>();
                    vector2 startPoint = wall["coords"].get<vector2>();
                    for (int w = 0; w < wid; ++w) {
                        auto h2 =  Heading (h.get() + 90);
                        vector2 curPoint = startPoint + (angleToVector2(h2) * (float) w);
                        for (int l = 0; l < len; ++l) {
                            vector2 dir = angleToVector2(h);
                            vector2 thisPoint = curPoint + (dir * (float) l);
                            if (l % 6) {
                                if (cam->isPointInView(thisPoint)) {
                                    vector2 screenCoords = cam->worldToScreenCoords(thisPoint);
                                    TriggerEvent("SDL::Render::DrawPoint", (int) screenCoords.x, (int) screenCoords.y);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

// Update Controller Functions
void GameManager::update(float deltaMs) {
    std::list<entity*> removalList;
    for (auto& e : entityList) {
        if (e->isDone() || e->getHearts() <= 0) {
            removalList.push_back(e);
            continue;
        }
        if (e->isEntityAnEnemy()) {
            handleEnemyUpdate(e);
        }else if (e->isEntityAPlayer()) {
            handlePlayerUpdate(e);
        }
    }
    for (auto e : removalList) {
        entityList.remove(e);
    }
    removalList.clear();
}

void GameManager::playerTakeHit(Player* p, int damage) {
    //TODO: move to player class
    if (p->doesPlayerHaveShield()) {
        //TODO: Add shield hit sound and animation
        p->hitShield();
    }else {
        //TODO: Add player hit sound and animation
        p->removeHearts(damage);
        textMap["PlayerHearts"]->setText("Hearts: " + std::to_string(p->getHearts()));
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
                    p->addATCount(); // adds 1 AT to player current loop total
                    textMap["ATScore"]->setText("AT: " + std::to_string(p->getATCount()));
                }else if (e2->getPickupType() == entity::HEART) {
                    e2->setHearts(0);
                    e2->succeed();
                    p->addHearts(1); // adds 1 heart
                    textMap["PlayerHearts"]->setText("Hearts: " + std::to_string(p->getHearts()));
                }else if (e2->getPickupType() == entity::OXY_TANK) {
                    e2->setHearts(0);
                    e2->succeed();
                    p->addOxygen(30000.0f); // adds 30 seconds of oxygen
                    textMap["OxyTimer"]->setText("Oxygen: " + p->getOxygenString());
                }
            }
        }else if (e2->isEntityAnEnemy()) {
            vector2 enemyCoords = e2->getPosition();

            if (e2->isKnockedBack()) {
                continue;
            }

            if (p->isKnockedBack()) {
                continue;
            }

            if (p->isEntityInvincible()) {
                continue;
            }

            if (p->isInvisible()) { // enemy cant see you to attack you
                continue;
            }

            if ((currentCoords - enemyCoords).length() < 20) {
                if (e2->getHearts() > 0) {
                    playerTakeHit(p, 1);

                    e2->removeHearts(1);
                    if (e2->getHearts() == 0) {
                        e2->succeed();
                    }

                    bounceEntities(e, e2);

                    textMap["PlayerHearts"]->setText("Hearts: " + std::to_string(p->getHearts()));
                }
            }
        }else if (e2->isEntityALaser()) {
            auto* l = dynamic_cast<Laser*>(e2);
            if (l && l->isFiring()) {
                // Get the laser start and end points
                vector2 p1 = l->getPosition();
                vector2 direction = angleToVector2(l->getHeading());  // Normalized direction vector
                vector2 p2 = p1 + direction * l->getLength();  // Laser extends in this direction
                // Calculate perpendicular vector to get the width of the laser
                vector2 perp(-direction.y, direction.x);  // Rotates 90 degrees to get width
                perp = perp * (l->getWidth() / 2.0f);  // Scale perpendicular by half width
                std::vector<vector2> laserBox = {
                        p1 - perp,  // Bottom-left
                        p1 + perp,  // Bottom-right
                        p2 + perp,  // Top-right
                        p2 - perp   // Top-left
                };
                if (isPointInBounds(currentCoords, laserBox)) {
                    playerTakeHit(p, l->getDamage());
                    if (!p->isEntityInvincible()) {
                        if (l->isSpinning()) {
                            p->setEntityInvincible(true, 16.0f * l->getSpeed() * l->getWidth());
                        } else {
                            p->setEntityInvincible(true, std::min(l->timeLeft(), l->getInterval()));
                        }
                    }
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
    bool inKnockback = e->isKnockedBack();
    if (!inKnockback) {
        // get the closest player if < 60 units away start to move towards player
        for (auto& e2 : entityList) {
            if (e2->isEntityAPlayer()) {
                auto* p = dynamic_cast<Player*>(e2);
                if (p->isInvisible()) { // cant see invisible players so no follow
                    break;
                }
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

    if (!isClose || inKnockback) {
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

// Attach Functions
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

void GameManager::attachTxd(std::string name, TxdLoader* txd) {
    txdMap[name] = txd;
}

// Logic Functions
void GameManager::bounceEntities(entity *e1, entity *e2) {
    vector2 e1Pos = e1->getPosition();
    vector2 e2Pos = e2->getPosition();
    Heading h = getHeadingFromVectors(e1Pos, e2Pos);
    print("Bounce Heading: ", h.get());
    vector2 newVel = angleToVector2(h) * 9.9f;
    h.set(h.get() + 180);
    print("Rebounce Heading: ", h.get());
    vector2 otherVel = angleToVector2(h) * 9.9f;
    e1->setVelocity(otherVel);
    e2->setVelocity(newVel);
    e1->setKnockedBack(true);
    e2->setKnockedBack(true);
}

void GameManager::setWorld(world *w) {
    worldMap = w;
}
