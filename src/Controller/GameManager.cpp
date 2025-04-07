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

#include <utility>
#include "Projectile.h"

int GameManager::initialize() {
    print("Game Manager Initialize");

    AddEventHandler("SDL::OnUpdate", [this](float deltaMs) {
        if (!gameRunning) {return;}
        if (cam == nullptr) {print("ALERT: Camera not set in Game Manager"); return;}
        // Update Before render
        renderWorld(deltaMs);
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
            if (e->isEntityAnEnemy() || e->isEntityAnEnemyBoss()) {
                renderEnemy(screenCoords, dim, e);
            }else if (e->isEntityAPickup()) {
                if (e->getPickupType() == entity::AT) {
                    if (auto at = std::dynamic_pointer_cast<AT>(e)) {
                        renderAT(screenCoords, dim, at);
                    }
                }else if (e->getPickupType() == entity::HEART) {
                    renderHeart(screenCoords, dim, e);
                }else if (e->getPickupType() == entity::OXY_TANK) {
                    //TODO: Render Oxy Tank Like Hearts and AT
                    TriggerEvent("SDL::Render::SetDrawColor", 0, 255, 0, 255);
                    TriggerEvent("SDL::Render::DrawRect", screenCoords.x, screenCoords.y, dim.x, dim.y);
                    TriggerEvent("SDL::Render::ResetDrawColor");
                }else if (e->getPickupType() == entity::KEY_CARD) {
                    TriggerEvent("SDL::Render::SetDrawColor", 0, 255, 255, 255);
                    TriggerEvent("SDL::Render::DrawRect", screenCoords.x, screenCoords.y, dim.x, dim.y);
                    TriggerEvent("SDL::Render::ResetDrawColor");
                }else if (e->getPickupType() == entity::ESCAPE_POD) {
                    TriggerEvent("SDL::Render::SetDrawColor", 255, 215, 0, 255);
                    TriggerEvent("SDL::Render::DrawRect", screenCoords.x, screenCoords.y, dim.x, dim.y);
                    TriggerEvent("SDL::Render::ResetDrawColor");
                }
            }else if (e->isEntityALaser()) {
                if (auto l = std::dynamic_pointer_cast<Laser>(e)) {
                    if (l->isFiring()) {
                        renderLaser(screenCoords, dim, l);
                    }
                }
            }else if (e->isEntityAProjectile()) {
                //TODO: Render Projectile Like Hearts and AT
                TriggerEvent("SDL::Render::SetDrawColor", 255, 255, 255, 255);
                TriggerEvent("SDL::Render::DrawRect", screenCoords.x, screenCoords.y, dim.x, dim.y);
                TriggerEvent("SDL::Render::ResetDrawColor");
            }
        }
        handleDebugWorldCreator(deltaMs); // handle the debug world creator (used to create walls)
    });

    AddEventHandler("SDL::OnPollEvent", [this](int eventType, int key) {
        if (!gameRunning) {return;}
        if (isDebug()) {
            if (eventType == SDL_KEYDOWN) {
                if (key == SDLK_c) {
                    // print cur coords of the player
                    print("Player Coords: ", entityList.front()->getPosition());
                }
            }
        }
    });

    RegisterCommand("setRoomIndex", [this](std::string command, sList_t args, std::string message) {
        if (args.empty()) {
            TriggerEvent("UFO::Chat::AddMessage", "Incorrect Usage: setRoomIndex <index>");
            return;
        }

        int index = std::stoi(args[0]);
        curRoomIndex = index;
        TriggerEvent("UFO::OnConfigUpdate", "curRoomIndex");
        TriggerEvent("UFO::Chat::AddMessage", "Room Index Set to: " + std::to_string(index));
    });

    RegisterCommand("createNewRoom", [this](std::string command, sList_t args, std::string message) {
        if (!args.empty()) {
            TriggerEvent("UFO::Chat::AddMessage", "Incorrect Usage: createNewRoom");
            return;
        }
        int roomIndex = world_ptr->addRoom();
        curRoomIndex = roomIndex;
        TriggerEvent("UFO::OnConfigUpdate", "curRoomIndex");
        TriggerEvent("UFO::Chat::AddMessage", "New Room Created and set as current room: " + std::to_string(roomIndex));
    });


    RegisterCommand("escape", [this](std::string command, sList_t args, std::string message) {
        if (!args.empty()) {
            TriggerEvent("UFO::Chat::AddMessage", "Incorrect Usage: escape");
            return;
        }

        TriggerEvent("UFO::EndGame");
        TriggerEvent("UFO::Chat::AddMessage", "Game Ended!");
    });
    gameRunning = true;
    return 1;
}

void GameManager::terminateGame() {
    for (auto& e : entityList) {e->abort();}
    for (auto& t : textMap) {t.second->abort();}
    for (auto& a : asepriteMap) {a.second->abort();}
    for (auto& t : txdMap) {t.second->abort();}
    for (auto& a : audioMap) {a.second->abort();}
    if (cam) {cam->abort();}
    if (world_ptr) {world_ptr->abort();}
    entityList.clear();
    textMap.clear();
    asepriteMap.clear();
    animMap.clear();
    txdMap.clear();
    audioMap.clear();
    cam = nullptr;
    world_ptr = nullptr;
    debugWall = nullptr;
    gameRunning = false;
    print("Game Manager Terminated!");
}


// Update View Functions
void GameManager::updatePlayerView(bool isVisible, const sh_ptr_e& e, float deltaMs) {
    if (!isVisible) {
        textMap["CamCoords"]->hideText();
        return;
    }

    auto p = std::dynamic_pointer_cast<Player>(e);
    if (!p) return;

    vector2 currentCoords = e->getPosition();
    vector2 screenCoords = cam->worldToScreenCoords(currentCoords); // convert world coords to screen coords
    vector2 playerDim = e->getDimensions();

    if (isDebug()) {

        // RelHeading
        int x, y;
        SDL_GetMouseState(&x, &y);
        vector2 mouseCoords = cam->screenToWorldCoords(vector2(x, y));
        Heading h = getHeadingFromVectors(currentCoords, mouseCoords);
        if (textMap["RelHeading"] ) {
            textMap["RelHeading"]->showText("Heading: " + std::to_string(h.get()));
        }



        // convert screenCoords.x and screenCoords.y to string with only 2 decimal places
        std::string xString = std::to_string(currentCoords.x);
        std::string yString = std::to_string(currentCoords.y);
        xString = xString.substr(0, xString.find(".") + 3);
        yString = yString.substr(0, yString.find(".") + 3);
        std::string coords = "X: " + xString + " Y: " + yString;
        if (textMap["CamCoords"]) {
            textMap["CamCoords"]->showText(coords);
            textMap["CamCoords"]->setTextPosition(screenCoords.x, screenCoords.y - 40);
        }
    }else {
        if (textMap["CamCoords"]) {
            textMap["CamCoords"]->hideText();
        }

        if (textMap["RelHeading"]) {
            textMap["RelHeading"]->hideText();
        }
    }

    // Update Oxygen Text
    if (textMap["OxyTimer"]) {
        textMap["OxyTimer"]->setText("Oxygen: " + p->getOxygenString());
    }

    // Update Hearts
    if (p->getMaxHearts() < 10) {
        int currentHearts = p->getHearts();
        int x = 0;
        int y = 0;
        for (int i = 0; i < currentHearts; i++) {
            if (i != 0) {
                x += 32;
            }
            renderHeart(vector2(x, y), vector2(32, 32));
        }
    }


    cam->updateCamera(currentCoords - vector2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2));

    vector2 playerVel = e->getVelocity();
    SDL_Rect currentFrame;

    if (!animMap.count("FSS_IDLE") || !animMap.count("FSS_MOVE")) {
        return;
    }

    bool flip = false;
    int angle = 0;

    if (playerVel.x != 0 || playerVel.y != 0) {
        currentFrame = animMap["FSS_MOVE"]->getCurrentFrame(deltaMs);
        if (playerVel.x < 0 || p->isFacingLeft()) {
            flip = true;
        }
    }else {
        flip = p->isFacingLeft();
        currentFrame = animMap["FSS_IDLE"]->getCurrentFrame(deltaMs);
    }

    // Need to change the position based on the direction the player is moving so the sprite stays within the hit box
    SDL_Rect destRect = {
            static_cast<int>(screenCoords.x - (angle == 90 ? 16.0f : (angle == -90 ? 48.0f : 32.0f))) ,
            static_cast<int>(screenCoords.y - (std::abs(angle) == 90 ? 32.0f : 48.0f)) ,
            64, // 64 px
            64, // 64 px
    };
    if (p->isInvisible()) {
        asepriteMap["FSS"]->setTextureAlpha(80);
    }
    asepriteMap["FSS"]->renderFrame(currentFrame, destRect, flip, angle);
    asepriteMap["FSS"]->resetTextureAlpha();

    for (auto & e2 : entityList) {
        if (!e2->isEntityAPickup()) {
            continue;
        }
        renderPickupInteraction(p, e2, currentCoords);
    }

    if (isDebug()) {
        // draw the hit box of the player
        TriggerEvent("SDL::Render::SetDrawColor", 0, 255, 255, 50);
        TriggerEvent("SDL::Render::DrawRect",
                     screenCoords.x - playerDim.x/2,
                     screenCoords.y - playerDim.y/2,
                     playerDim.x,
                     playerDim.y
        );
        TriggerEvent("SDL::Render::ResetDrawColor");

        TriggerEvent("SDL::Render::SetDrawColor", 255, 0, 0, 255);
        TriggerEvent("SDL::Render::DrawPoint", screenCoords.x, screenCoords.y);
        TriggerEvent("SDL::Render::ResetDrawColor");
    }
}

void GameManager::renderEnemy(vector2 screenCoords, vector2 dim, const sh_ptr_e& e) {
    // yes I know I could have just randomized the textures on load of the class but that would have required
    // making a new class for enemies or add more bloat to the entity class so ya no this round about way is OK
    int ran = mapHashToRange(jenkinsOneAtATimeHash(e->getId()), 1, 3) + 0;
    std::string textureName = "ALIEN" + std::to_string(ran) + "::TEXTURE";

    auto it = txdMap.find(textureName);
    if (it == txdMap.end() || !it->second) {
        return;
    }
    SDL_Rect srcRect;

    switch (ran) {
        case 1:
            srcRect = {0, 0, 160, 160};
            break;
        case 2:
            srcRect = {0, 0, 160, 200};
            break;
        case 3:
            srcRect = {0, 0, 200, 160};
            break;
    }

    SDL_Rect destRect = {
            static_cast<int>(screenCoords.x - dim.x/2) ,
            static_cast<int>(screenCoords.y - dim.y/2) ,
            static_cast<int>(dim.x),
            static_cast<int>(dim.y) // down scale or up scale the texture
    };
    txdMap[textureName]->render(srcRect, destRect, 0, SDL_FLIP_NONE);

    if (isDebug()) {
        // draw the hit box of the player
        TriggerEvent("SDL::Render::SetDrawColor", 255, 0, 255, 50);
        TriggerEvent("SDL::Render::DrawRect",
                     screenCoords.x - dim.x/2,
                     screenCoords.y - dim.y/2,
                     dim.x,
                     dim.y
        );
        TriggerEvent("SDL::Render::ResetDrawColor");

        TriggerEvent("SDL::Render::SetDrawColor", 255, 0, 0, 255);
        TriggerEvent("SDL::Render::DrawPoint", screenCoords.x, screenCoords.y);
        TriggerEvent("SDL::Render::ResetDrawColor");
    }

    int hearts = e->getMaxHearts();
    if (hearts > 0) {
        int length = e->getLength();
        int width = e->getWidth();

        float heartWidth = static_cast<float>(length) / hearts;
        float heartHeight = heartWidth; // Make them square or adjust as needed

        float startX = screenCoords.x - length / 2.0f;
        float y = screenCoords.y - width / 2.0f - heartHeight - 2; // place above the head with a small gap

        for (int i = 0; i < hearts; i++) {
            if (i+1 > e->getHearts()) {
                break;
            }
            float x = startX + i * heartWidth;
            renderHeart(vector2(x, y), vector2(heartWidth, heartHeight));
        }
    }


}

void GameManager::renderLaser(vector2 screenCoords, vector2 dim, const sh_ptr_laser& l) {
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

void GameManager::renderAT(vector2 screenCoords, vector2 dim, const sh_ptr_at& at) {
    // Check if the texture exists in txdMap
    auto it = txdMap.find("AT::TEXTURE");
    if (it == txdMap.end() || !it->second) {
        return;
    }

    int textureSize = 125;

    // Render the texture
    SDL_Rect srcRect = {0, 0, textureSize, textureSize}; // load the entire texture
    SDL_Rect destRect = {
            static_cast<int>(screenCoords.x - (dim.x / 2)),
            static_cast<int>(screenCoords.y - (dim.y / 2)),
            static_cast<int>(dim.x),
            static_cast<int>(dim.y) // down scale the texture
    };

    if (isDebug()) { // draw a rec around the texture (background)
        TriggerEvent("SDL::Render::SetDrawColor", 0, 255, 255, 255);
        TriggerEvent("SDL::Render::DrawRect", screenCoords.x - (dim.x / 2), screenCoords.y - (dim.y / 2), dim.x, dim.y);
        TriggerEvent("SDL::Render::ResetDrawColor");

    }
    txdMap["AT::TEXTURE"]->render(srcRect, destRect, 0, SDL_FLIP_NONE);
    if (isDebug()) { // draw a center point of the AT
        TriggerEvent("SDL::Render::SetDrawColor", 255, 0, 0, 255);
        TriggerEvent("SDL::Render::DrawPoint", screenCoords.x, screenCoords.y);
        TriggerEvent("SDL::Render::ResetDrawColor");

    }
}

void GameManager::renderHeart(vector2 screenCoords, vector2 dim, const sh_ptr_e& e) {
    // Check if the texture exists in txdMap
    auto it = txdMap.find("HEART::TEXTURE");
    if (it == txdMap.end() || !it->second) {
        return;
    }
    int textureSize = 32;

    // Render the texture
    SDL_Rect srcRect = {0, 0, textureSize, textureSize}; // load the entire texture
    SDL_Rect destRect = {
            static_cast<int>(screenCoords.x - (dim.x/2)),
            static_cast<int>(screenCoords.y - (dim.y/2)),
            static_cast<int>(dim.x),
            static_cast<int>(dim.y) // down scale the texture
    };

    if (isDebug()) {
        TriggerEvent("SDL::Render::SetDrawColor", 255, 0, 0, 255);
        TriggerEvent("SDL::Render::DrawRect", screenCoords.x - (dim.x/2), screenCoords.y - (dim.y/2), dim.x, dim.y);
        TriggerEvent("SDL::Render::ResetDrawColor");
    }

    txdMap["HEART::TEXTURE"]->render(srcRect, destRect, 0, SDL_FLIP_NONE);
}

void GameManager::renderHeart(vector2 screenCoords, vector2 dim) {
    // Check if the texture exists in txdMap
    auto it = txdMap.find("HEART::TEXTURE");
    if (it == txdMap.end() || !it->second) {
        return;
    }
    int textureSize = 32;

    // Render the texture
    SDL_Rect srcRect = {0, 0, textureSize, textureSize}; // load the entire texture
    SDL_Rect destRect = {
            static_cast<int>(screenCoords.x),
            static_cast<int>(screenCoords.y),
            static_cast<int>(dim.x),
            static_cast<int>(dim.y) // down scale the texture
    };
    txdMap["HEART::TEXTURE"]->render(srcRect, destRect, 0, SDL_FLIP_NONE);
}

void GameManager::handleDebugWorldCreator(float deltaMs) {
    if (isDebug()) {
        // If Ctrl + left click, start create a wall or place point
        // If Ctrl + right click, Undo last point
        // P place point at player coords
        // C clear all points
        // store cur wall in debugWall
        // cur state debugWallSate of enum wallState
        // DO NOT USE EVENTS IN THIS FUNCTION USE NATIVE SDL EVENTS

        int x, y;
        Uint32 mouseState = SDL_GetMouseState(&x, &y); // Get mouse position
        const Uint8 *keyboard_state_array = SDL_GetKeyboardState(nullptr);
        if (mouseState && SDL_BUTTON(SDL_BUTTON_LEFT) && keyboard_state_array[SDL_SCANCODE_LCTRL]) {
            if (inClick) {
                return;
            }
            inClick = true;

            print("Debug Wall Create Mouse Clicked: ", x, " ", y, debugWallSate);
            if (debugWallSate == db_WCS::CREATION_NOT_STARTED) {

                // if we click near or in a wall set that as the current debug wall and set the indexes so we can update later on
                db_room_index = -1;
                db_wall_index = -1;
                debugWall = new wall();
                debugWall->length = 50;
                debugWall->width = 8;
                debugWall->position = cam->screenToWorldCoords(vector2(x, y));
                debugWall->heading = Heading(0);

                debugWallSate = db_WCS::COORDS_SET;
                textMap["DEBUG::WALL::0"]->showText(
                        "Wall Coords: " + std::to_string((int) debugWall->position.x) + " " +
                        std::to_string((int) debugWall->position.y));
                print("Wall Creation Started: set coords", debugWall->position);
            } else if (debugWallSate == db_WCS::COORDS_SET) {
                debugWall->length = (int) (cam->screenToWorldCoords(vector2(x, y)) - debugWall->position).length();
                debugWallSate = db_WCS::LENGTH_SET;
                textMap["DEBUG::WALL::1"]->showText("Wall Length: " + std::to_string(debugWall->length));
                print("Wall Creation: set length", debugWall->length);

                // set heading of wall
                Heading h = getHeadingFromVectors(debugWall->position, cam->screenToWorldCoords(vector2(x, y)));
                debugWall->heading = h;
                textMap["DEBUG::WALL::2"]->showText("Wall Heading: " + std::to_string(debugWall->heading.get()));
                print("Wall Creation: set heading", h.get());
            } else if (debugWallSate == db_WCS::LENGTH_SET) {
                // get the value / magnitude of the vector of h + 90 degrees
                vector2 cur_pos = cam->screenToWorldCoords(vector2(x, y));
                Heading h_cur = getHeadingFromVectors(debugWall->position, cur_pos);
                Heading w_heading = debugWall->heading;
                auto h_target = Heading(debugWall->heading.get() + 90);

                // check if the h_cur is from debugWall->heading to h_target
                if (h_cur.isWithin(w_heading, h_target)) {
                    vector2 v1 = debugWall->position;
                    vector2 v2 = debugWall->position + (angleToVector2(debugWall->heading) * (float) debugWall->length);
                    // get the magnitude of the vector from the wall to the point
                    float distance = getPerpendicularDistance(cur_pos, v1, v2);
                    if (distance > 0) {
                        debugWall->width = distance;
                        debugWallSate = db_WCS::WIDTH_SET;
                        textMap["DEBUG::WALL::3"]->showText("Wall Width: " + std::to_string(debugWall->width));
                    }
                }
            } else if (debugWallSate == db_WCS::WIDTH_SET) {
                // add the wall to the room
                json worldData = world_ptr->getWorldData().get();
                if (worldData["rooms"].empty() == 0) {
                    if (db_room_index == -1 || db_wall_index == -1) {
                        int roomIndex = curRoomIndex;
                        if (roomIndex == -1) {roomIndex = world_ptr->addRoom();}
                        curRoomIndex = roomIndex;
                        TriggerEvent("UFO::OnConfigUpdate", "curRoomIndex");
                        world_ptr->addWall(roomIndex, debugWall);
                    } else {
                        world_ptr->updateWall(db_room_index, db_wall_index, debugWall);
                    }
                } else {
                    int roomIndex = world_ptr->addRoom();
                    curRoomIndex = roomIndex;
                    TriggerEvent("UFO::OnConfigUpdate", "curRoomIndex");
                    world_ptr->addWall(roomIndex, debugWall);
                }
                world_ptr->saveWorld();
                debugWall = nullptr;
                debugWallSate = db_WCS::CREATION_NOT_STARTED;
                for (int i = 0; i <= GameManager::db_WCS::WIDTH_SET; i++) {
                    textMap["DEBUG::WALL::" + std::to_string(i)]->hideText();
                }
            }
        }else if (mouseState && SDL_BUTTON(SDL_BUTTON_LEFT) && keyboard_state_array[SDL_SCANCODE_LALT]) {
            if (debugWallSate != db_WCS::CREATION_NOT_STARTED) {
                // delete this wall from the shit
                if (db_room_index != -1 || db_wall_index != -1) {
                    world_ptr->deleteWall(db_room_index, db_wall_index);
                }

                debugWallSate = db_WCS::CREATION_NOT_STARTED;
                debugWall = nullptr;
                for (int i = 0; i <= GameManager::db_WCS::WIDTH_SET; i++) {
                    textMap["DEBUG::WALL::" + std::to_string(i)]->hideText();
                }
            }
        }else if (mouseState && SDL_BUTTON(SDL_BUTTON_LEFT) && keyboard_state_array[SDL_SCANCODE_LSHIFT]) {
            if (inShiftFind) {return;}

            inShiftFind = true;
            shiftFindStart = cam->screenToWorldCoords(vector2(x, y));
        }else {
            if (inShiftFind) {
                bool foundWall = false;
                roomList_t roomList = world_ptr->getRoomList();
                if (roomList.empty() == 0) {
                    for (auto &wallList: roomList) {
                        for (auto &cur_wall: wallList) {
                            vector2 wallPos = cur_wall->position;
                            vector2 mousePos = cam->screenToWorldCoords(vector2(x, y));
                            vectorList_t mm = getMinMaxFromVectors(mousePos, shiftFindStart);
                            if ( isPointInBounds(mousePos, calculateBoundingBox(mm[0], mm[1])) ) {
                                debugWall = cur_wall;
                                db_room_index = std::distance(roomList.begin(),
                                                              std::find(roomList.begin(), roomList.end(), wallList));
                                db_wall_index = std::distance(wallList.begin(),
                                                              std::find(wallList.begin(), wallList.end(), cur_wall));
                                foundWall = true;
                                print("Debug Found Wall", db_room_index, db_wall_index);
                                break;
                            }
                        }
                    }
                }

                if (foundWall) {
                    if (debugWallSate == db_WCS::CREATION_NOT_STARTED) {
                        debugWallSate = db_WCS::COORDS_SET;
                        textMap["DEBUG::WALL::0"]->showText(
                                "Wall Coords: " + std::to_string((int) debugWall->position.x) + " " +
                                std::to_string((int) debugWall->position.y));
                        print("Wall Creation Started: set coords", debugWall->position);
                    }
                }
                inShiftFind = false;
            }



            inClick = false;
            if (debugWallSate != db_WCS::CREATION_NOT_STARTED) {
                if (debugWallSate == db_WCS::COORDS_SET) {
                    vector2 cur_pos = cam->screenToWorldCoords(vector2(x, y));
                    debugWall->length = (int) (cur_pos - debugWall->position).length();
                    textMap["DEBUG::WALL::1"]->showText("Wall Length: " + std::to_string(debugWall->length));
                    // set heading of wall
                    Heading h = getHeadingFromVectors(debugWall->position, cur_pos);
                    debugWall->heading = h;
                    textMap["DEBUG::WALL::2"]->showText("Wall Heading: " + std::to_string(debugWall->heading.get()));
                }else if (debugWallSate == db_WCS::LENGTH_SET) {
                    // get the value / magnitude of the vector of h + 90 degrees
                    vector2 cur_pos = cam->screenToWorldCoords(vector2(x, y));
                    Heading h_cur = getHeadingFromVectors(debugWall->position, cur_pos);
                    Heading w_heading = debugWall->heading;
                    auto h_target = Heading(debugWall->heading.get() + 90);

                    // check if the h_cur is from debugWall->heading to h_target
                    if (h_cur.isWithin(w_heading, h_target)) {
                        vector2 v1 = debugWall->position;
                        vector2 v2 = debugWall->position + (angleToVector2(debugWall->heading) * (float) debugWall->length);
                        // get the magnitude of the vector from the wall to the point
                        float distance = getPerpendicularDistance(cur_pos, v1, v2);
                        if (distance > 0) {
                            debugWall->width = distance;
                            textMap["DEBUG::WALL::3"]->showText("Wall Width: " + std::to_string(debugWall->width));
                        }
                    }
                }
            }
        }
    }else {
        debugWallSate = db_WCS::CREATION_NOT_STARTED;
        debugWall = nullptr;
        for (int i = 0; i <= GameManager::db_WCS::WIDTH_SET; i++) {
            textMap["DEBUG::WALL::" + std::to_string(i)]->hideText();
        }
    }
}

void GameManager::drawWall(wall* cur_wall) {
    int textureLength = 32;
    int textureWidth = 4;
    auto h90 = Heading(90);
    auto h180 = Heading(270);
    // make dashed line for each wall in the direction of center following h till length
    // use triggerevent SDL::Render::DrawPoint
    auto h = cur_wall->heading;
    vector2 startPoint = cur_wall->position;

    int baseLength = cur_wall->length;
    int baseWidth = cur_wall->width;

    int wid = 0;
    vector2 dir = angleToVector2(h);
    auto h2 =  Heading (h.get() + 90);
    vector2 perpDir = angleToVector2(h2);
    while (wid < baseWidth) {
        int len = 0;
        int cur_wid = std::min(baseWidth - wid, textureWidth);
        vector2 curPoint = startPoint + (perpDir * (float)(wid));
        while (len < baseLength) {
            int cur_len = std::min(baseLength - len, textureLength);
            vector2 thisPoint = curPoint + (dir * (float) len);
            if (cam->isPointInView(thisPoint)) {
                vector2 screenCoords = cam->worldToScreenCoords(thisPoint);




                SDL_Rect srcRect;
                SDL_Rect destRect;
                srcRect = {0, 0, textureLength, textureWidth};

                bool isWithin = h.isWithin(h90, h180);

                destRect = {
                        static_cast<int>(std::round(screenCoords.x)),
                        static_cast<int>(std::round(screenCoords.y)),
                        isWithin ? -cur_len : cur_len, cur_wid
                };

                if (len >= baseLength && wid >= baseWidth) {
                    // dump all the data into print
                    print("Wall: ", cur_wall->position, cur_wall->heading, cur_wall->length, cur_wall->width);
                    print("Wall 2: ", thisPoint, wid, len, cur_wid, cur_len);
                    print("Wall 3: ", screenCoords, destRect.x, destRect.y, destRect.w, destRect.h);
                }else {
                    TriggerEvent("SDL::Render::DrawRect", screenCoords.x + 5, screenCoords.y, 1, 1);
                }

//                txdMap["WALL::TEXTURE"]->render(srcRect, destRect, h, SDL_FLIP_NONE);
            }
            len += cur_len;
        }
        wid += cur_wid;
    }


//    for (int w = 0; w < txdsW; ++w) {
//    vector2 curPoint = startPoint + (angleToVector2(h2) * (float) (w * textureWidth));
//        for (int l = 0; l < txdsL; ++l) {
//            vector2 dir = angleToVector2(h);
//            vector2 thisPoint = curPoint + (dir * (float) (l * textureLength));
//            if (cam->isPointInView(thisPoint)) {
//                vector2 screenCoords = cam->worldToScreenCoords(thisPoint);
//                SDL_Rect srcRect;
//                SDL_Rect destRect;
//                srcRect = {0, 0, textureLength, textureWidth};
//                destRect = {
//                        static_cast<int>(screenCoords.x),
//                        static_cast<int>(screenCoords.y),
//                        textureLength, textureWidth
//                };
//                txdMap["WALL::TEXTURE"]->render(srcRect, destRect, h, SDL_FLIP_NONE);
//            }
//        }
//    }
}

void GameManager::renderWorld(float deltaMs) {
    // loop through worldData.rooms and then draw each wall in it
    if (world_ptr == nullptr) {
        error("World Map not set in Game Manager");
        return;
    }

    auto it = txdMap.find("WALL::TEXTURE");
    if (it == txdMap.end() || !it->second) {
        return;
    }

    if (isDebug()) {
        roomList_t roomList = world_ptr->getRoomList();


        if (roomList.empty() == 0) {
            for (auto& wallList : roomList) {
                for (auto& cur_wall : wallList) {
                    drawWall(cur_wall);
                }
            }
        }


        if (debugWall != nullptr) {
            if (debugWallSate != db_WCS::CREATION_NOT_STARTED) {
                drawWall(debugWall);
            }
        }
    }
}

void GameManager::renderTextOnEntity(const sh_ptr_e& e, const std::string& textMapName, const std::string& textDefault) {
    if (textMapName.empty()) {
        return;
    }

    auto it = textMap.find(textMapName);
    if (it == textMap.end() || !it->second) {
        return;
    }

    auto mapped = it->second;


    if (textDefault.empty()) {
        mapped->hideText();
        return;
    }

    vector2 screenCoords = cam->worldToScreenCoords(e->getPosition());
    vector2 dim = e->getDimensions();
    SDL_Rect rec = mapped->getTextRect();
    mapped->setTextPosition(screenCoords.x + dim.x/2 - rec.w/2, screenCoords.y - dim.y/2 - rec.h);
    mapped->showText(textDefault);
}

void GameManager::clearPickupInteraction(const sh_ptr_e& e) {
    if (!e) {
        return;
    }

    entity::pType pickupType = e->getPickupType();
    if (pickupType != entity::KEY_CARD && pickupType != entity::ESCAPE_POD) {
        return;
    }

    std::string textMapName;

    if (pickupType == entity::KEY_CARD) {
        textMapName = "KEYCARD::" + e->getId();
    }else if (pickupType == entity::ESCAPE_POD) {
        textMapName = "ESCAPE::" + e->getId();
    }

    auto it = textMap.find(textMapName);
    if (it != textMap.end()) {
        it->second->hideText();
        textMap.erase(it);
    }
}

void GameManager::renderPickupInteraction(const sh_ptr_ply& ply, const sh_ptr_e& e, vector2& currentCoords) {
    if (!e || !ply) {
        return;
    }

    entity::pType pickupType = e->getPickupType();
    if (pickupType != entity::KEY_CARD && pickupType != entity::ESCAPE_POD) {
        return;
    }


    vector2 enemyCoords = e->getPosition();
    bool isInside = ply->isPointInEntity(enemyCoords);

    std::string textMapName;

    if (pickupType == entity::KEY_CARD) {
        textMapName = "KEYCARD::" + e->getId();
    }else if (pickupType == entity::ESCAPE_POD) {
        textMapName = "ESCAPE::" + e->getId();
    }

    bool inView = cam->isPointInView(enemyCoords);
    bool isClose = ((enemyCoords - currentCoords).length() < 150);

    sh_ptr<text> mapped = textMap[textMapName];

    if (!inView) {
        if (mapped) {mapped->hideText();}
        return;
    }

    if (!mapped) {
        textMap[textMapName] = std::make_shared<text>(passFunc, "[E]", 25);
        pM->attachProcess(textMap[textMapName]);
        mapped = textMap[textMapName];
    }

    if (!isClose) {
        mapped->hideText();
        return;
    }

    vector2 dim = e->getDimensions();
    vector2 screenCoords = cam->worldToScreenCoords(enemyCoords);
    std::string displayText = "[E]";
    if (isInside) {
        switch (pickupType) {
            case entity::KEY_CARD:
                displayText = "Press [E] to pick up keycard!";
                break;
            case entity::ESCAPE_POD:
                displayText = "Press [E] to enter ESCAPE POD!";
                break;
            default:
                break;
        }
    }

    mapped->showText(displayText);
    SDL_Rect rec = mapped->getTextRect();
    mapped->setTextPosition(screenCoords.x + dim.x/2 - rec.w/2, screenCoords.y - dim.y/2 - rec.h);
}



// Update Controller Functions
void GameManager::update(float deltaMs) {
    if (!gameRunning) {return;}

    std::list<sh_ptr_e> removalList;
    for (auto& e : entityList) {
        if (e->isDone() || e->getHearts() <= 0) {
            e->fail();
            removalList.push_back(e);

            if (e->isEntityAPlayer()) {
                if (!gameRunning) {return;}
                TriggerEvent("UFO::EndGame");
                TriggerEvent("UFO::Chat::AddMessage", "Game Over!");
                return;
            }

            continue;
        }

        // Remove projectiles that are too far away from the shooting position and not in view
        if (e->isEntityAProjectile() && e->inWorld()) {
            auto p = std::dynamic_pointer_cast<Projectile>(e);
            if (!p) continue;

            if (p->isOutOfRange()) {
                e->abort();
                removalList.push_back(e);
                continue;
            }
        }

        // Handle Collisions
        if (e->isEntityAnEnemy()) {
            handleEnemyUpdate(e, deltaMs);
        }else if (e->isEntityAnEnemyBoss()) {
            handleBossUpdate(e, deltaMs);
        }else if (e->isEntityAPlayer()) {
            handlePlayerUpdate(e, deltaMs);
        }

        // Handle Update of Coords
        e->updateCoordsFromVelocity(deltaMs);
        // Check if the new coords are out of bounds or hitting a wall
        vectorList_t entityCorners;
        entityCorners.push_back(e->getPosition() + (-e->getLength()/2, -e->getWidth()/2));
        entityCorners.push_back(e->getPosition() + (-e->getLength()/2, e->getWidth()/2));
        entityCorners.push_back(e->getPosition() + (e->getLength()/2, e->getWidth()/2));
        entityCorners.push_back(e->getPosition() + (e->getLength()/2, -e->getWidth()/2));
        if (world_ptr->isRectInWall(entityCorners)) {
            e->setPosition(e->getLastCoords());
            if (e->isEntityAProjectile()) {
                e->removeHearts(e->getHearts());
                if (e->getHearts() == 0) {
                    e->fail();
                }
            }
        }
    }


    for (const auto& e : removalList) {
        entityList.remove(e);
    }
    removalList.clear();
}

void GameManager::handlePlayerUpdate(const sh_ptr_e& e, float deltaMs) {
    // Cast to player and if does not work return
    auto p = std::dynamic_pointer_cast<Player>(e);
    if (!p) return;

    vector2 currentCoords = e->getPosition();
    for (auto& e2 : entityList) {
        if (e2->isEntityAPickup()) {
            vector2 enemyCoords = e2->getPosition();
            int pickupType = e2->getPickupType();
            std::string keyCardName;
            bool isInside = p->isPointInEntity(enemyCoords);

            if (isInside) {
                if (pickupType == entity::AT) {
                    e2->setHearts(0);
                    e2->succeed();
                    p->addATCount(); // adds 1 AT to player current loop total
                    textMap["ATScore"]->setText("AT: " + std::to_string(p->getATCount()));
                }else if (pickupType == entity::HEART) {
                    if (p->getHearts() == p->getMaxHearts()) {
                        continue; // This is a design choice to not allow the player to pick up hearts if they are full
                    }
                    e2->setHearts(0);
                    e2->succeed();
                    p->addHearts(1); // adds 1 heart
                    textMap["PlayerHearts"]->setText("Hearts: " + std::to_string(p->getHearts()));
                }else if (pickupType == entity::OXY_TANK) {
                    e2->setHearts(0);
                    e2->succeed();
                    p->addOxygen(30000.0f); // adds 30 seconds of oxygen
                    textMap["OxyTimer"]->setText("Oxygen: " + p->getOxygenString());
                }else if (pickupType == entity::KEY_CARD) {
                    const Uint8 *keyboard_state_array = SDL_GetKeyboardState(nullptr);
                    if (keyboard_state_array[SDL_SCANCODE_E]) {
                        e2->setHearts(0);
                        clearPickupInteraction(e2);
                        e2->succeed();
                    }
                }else if (pickupType == entity::ESCAPE_POD) {
                    const Uint8 *keyboard_state_array = SDL_GetKeyboardState(nullptr);
                    if (keyboard_state_array[SDL_SCANCODE_E]) {
                        e2->setHearts(0);
                        clearPickupInteraction(e2);
                        e2->succeed();
                        TriggerEvent("UFO::EndGame");
                    }
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
            auto l = std::dynamic_pointer_cast<Laser>(e2);
            if (!l) return;
            if (l->isFiring()) {
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

        }else if (e2->isEntityAProjectile()) {
            vector2 projCoords = e2->getPosition();
            if (p->isPointInEntity(projCoords)) {
                if (e2->getHearts() > 0) {
                    auto proj = std::dynamic_pointer_cast<Projectile>(e2);
                    if (!proj) continue;
                    if (proj->getOwner() == e2) {
                        // if the projectile is from the player ignore it
                        continue;
                    }

                    playerTakeHit(p, e2->getHearts());
                    bounceEntities(e, e2);
                    e2->removeHearts(e2->getHearts());
                    if (e2->getHearts() == 0) {e2->succeed();}
                    textMap["PlayerHearts"]->setText("Hearts: " + std::to_string(p->getHearts()));
                }
            }
        }
    }

    if (p->getOxygenLevel() <= 0.0f && !isDebug()) { // if oxygen level is 0 remove hearts if not in debug mode
        p->removeHearts(1);
        textMap["PlayerHearts"]->setText("Hearts: " + std::to_string(p->getHearts()));
        p->setOxygenLevel(0.0f);
    }

    if (p->getHearts() <= 0) {
        TriggerEvent("UFO::EndGame");
        TriggerEvent("UFO::Chat::AddMessage", "Game Over!");
    }

    if (p->isATCannonFire()) {
        // create new entity with projectile type and shoot it in front of the player in the direction of the mouse
        // set the vel of it to be 3x npc speed or player base speed

        if (p->getATCount() > 0 && !p->isProjectileCreated()) {
            print("Creating Projectile for AT Cannon fire");
            int damage = p->getATCannonDamage();
            p->removeATCount(damage); //TODO: might want to change this later on and set it to always be 1
            print("Removing AT Count FOR SHOOT: ", damage);
            textMap["ATScore"]->setText("AT: " + std::to_string(p->getATCount()));
            vector2 playerCoords = p->getPosition();
            int x, y; SDL_GetMouseState(&x, &y);
            vector2 mouseCoords = cam->screenToWorldCoords(vector2(x, y));
            Heading h = getHeadingFromVectors(currentCoords, mouseCoords);
            vector2 pVel = angleToVector2(h) * 0.35f;

            vector2 spawnCoords = playerCoords + (angleToVector2(h) * (p->getDimensions().x + 1.0f));
            auto proj = std::make_shared<Projectile>(passFunc, spawnCoords, damage, std::shared_ptr<Player>(p));
            pM->attachProcess(proj);
            attachEntity(proj);
            proj->setVelocity(pVel);
            p->setProjectileCreated(true);
            proj->spawn();

            proj->AddEventHandler("ENTITY::SUCCEED", [p, this]() {
                audioMap["hitmarker"]->play(0.1f);
            });

            if (p->isInvisible()) {
                p->setInvisible(false); // leave invisibility when you shoot
            }
        }
    }
}

void GameManager::handleEnemyUpdate(const sh_ptr_e& e, float deltaMs) {
    vector2 currentCoords = e->getPosition();
    vector2 curVel = e->getVelocity();
    vector2 newVel = vector2(0.0f, 0.0f);
    bool isClose = false;
    bool inKnockback = e->isKnockedBack();

    for (auto& e2 : entityList) {
        if (e2->isEntityAPlayer()) {
            auto p = std::dynamic_pointer_cast<Player>(e2);
            if (!p) continue;
            if (p->isInvisible()) { // cant see invisible players so no follow
                continue;
            }
            vector2 playerCoords = e2->getPosition();
            if ((currentCoords - playerCoords).length() < (SCREEN_WIDTH / 4)) {
                newVel = (playerCoords - currentCoords).normalize() * 0.05f;
                isClose = true;
            }
            continue;
        }else if (e2->isEntityAProjectile()) {
            vector2 projCoords = e2->getPosition();
            if (e->isPointInEntity(projCoords)) {
                if (e2->getHearts() > 0) {
                    auto proj = std::dynamic_pointer_cast<Projectile>(e2);
                    if (!proj) continue;
                    if (proj->getOwner() != getPlayer()) {
                        // if the projectile is NOT from the player ignore it
                        continue;
                    }



                    e->removeHearts(e2->getHearts());
                    if (e->getHearts() == 0) {
                        e->succeed();
                    }
                    bounceEntities(e2, e);
                    curVel = e->getVelocity();
                    inKnockback = true;
                    e2->removeHearts(e2->getHearts());
                    e2->succeed();
                }
            }
        }
    }


    if (!inKnockback) {
        // get the closest player if < 60 units away start to move towards player
        if (!isClose) {
            // if not close to spawn point move back to spawn point
            vector2 spawnCoords = e->getSpawnCoords();
            if ((currentCoords - spawnCoords).length() > 5) {
                newVel = (spawnCoords - currentCoords).normalize() * 0.05f;
            }
        }
    }else {
        if (e->remainingKnockback() - deltaMs > 0 && e->isKnockedBack()) {
            e->setKnockedBack(true, e->remainingKnockback() - deltaMs);
            newVel = curVel;
        } else {
            e->setKnockedBack(false, 0.0f);
        }
    }

    if (!isClose && !inKnockback) {
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

void GameManager::handleBossUpdate(const sh_ptr_e& e, float deltaMs) {
    vector2 currentCoords = e->getPosition();
    vector2 curVel = e->getVelocity();
    vector2 newVel = vector2(0.0f, 0.0f);

    auto b = std::dynamic_pointer_cast<Boss>(e);
    if (!b) return;

    for (auto& e2 : entityList) {
        if (e2->isEntityAPlayer()) {
            // If the player is nearby (later will add check if in boss room)
            // create minions and projectiles
            auto p = std::dynamic_pointer_cast<Player>(e2);
            if (!p) return;
            if (p->isInvisible()) { // cant see invisible players so no follow
                continue;
            }
            vector2 playerCoords = e2->getPosition();
            if ((currentCoords - playerCoords).length() < ((float) SCREEN_WIDTH)) {
                if (b->canSpawnMinion()) {
                    Heading h = getHeadingFromVectors(currentCoords, playerCoords);
                    vector2 pVel = angleToVector2(h) * 0.35f;
                    vector2 spawnCoords = currentCoords + (angleToVector2(h) * (p->getDimensions().x + 10.0f));
                    sh_ptr_e minion = b->spawnMinion(spawnCoords);
                    pM->attachProcess(minion);
                    attachEntity(minion);
                    minion->spawn();
                    minion->AddEventHandler("ENTITY::SUCCEED", [b, minion, this]() {
                        b->removeMinion(minion);
                    });

                    minion->AddEventHandler("ENTITY::ABORT", [b, minion, this]() {
                        b->removeMinion(minion);
                    });
                }

                if (b->canSpawnProjectile()) {
                    Heading h = getHeadingFromVectors(currentCoords, playerCoords);
                    vector2 pVel = angleToVector2(h) * 0.35f;
                    vector2 spawnCoords = currentCoords + (angleToVector2(h) * (p->getDimensions().x + 10.0f));
                    sh_ptr_e proj = b->spawnProjectile(spawnCoords);
                    pM->attachProcess(proj);
                    attachEntity(proj);
                    proj->spawn();
                    proj->setVelocity(pVel);

                    proj->AddEventHandler("ENTITY::SUCCEED", [b, proj, this]() {
                        b->removeProjectile(proj);
                    });

                    proj->AddEventHandler("ENTITY::ABORT", [b, proj, this]() {
                        b->removeProjectile(proj);
                    });
                }
            }
            continue;
        }else if (e2->isEntityAProjectile()) {
            vector2 projCoords = e2->getPosition();
            if (e->isPointInEntity(projCoords)) {
                if (e2->getHearts() > 0) {
                    auto proj = std::dynamic_pointer_cast<Projectile>(e2);
                    if (!proj) continue;
                    if (proj->getOwner() != getPlayer()) {
                        // if the projectile is NOT from the player ignore it
                        continue;
                    }

                    e->removeHearts(e2->getHearts());
                    if (e->getHearts() == 0) {
                        e->succeed();
                    }
                    e2->removeHearts(e2->getHearts());
                    e2->succeed();
                }
            }
        }
    }

    e->setVelocity({0.0,0.0});
}


// Setters
void GameManager::setProcessManager(sh_ptr<ProcessManager> pm) {
    pM = std::move(pm);
}

void GameManager::setCamera(sh_ptr<camera> c) {
    cam = std::move(c);
}

void GameManager::setWorld(sh_ptr<world> w) {
    world_ptr = std::move(w);
}


// Attach Functions
void GameManager::attachEntity(sh_ptr<entity> e) {
    entityList.push_back(e);
    if (e->isEntityAPlayer()) {
        if (world_ptr != nullptr) {
            vector2 spawnPoint = world_ptr->getSpawnPoint();
            e->setPosition(spawnPoint);
        }
    }
}

void GameManager::attachAseprite(const std::string& name, sh_ptr<AsepriteLoader> a) {
    asepriteMap[name] = std::move(a);
}

void GameManager::attachText(const std::string& name, sh_ptr<text> t) {
    textMap[name] = std::move(t);
}

void GameManager::attachTxd(const std::string& name, sh_ptr<TxdLoader> txd) {
    txdMap[name] = std::move(txd);
}

void GameManager::attachAudio(const std::string& name, sh_ptr<AudioLoader> audio) {
    audioMap[name] = std::move(audio);
}

void GameManager::attachAnim(const std::string &name, sh_ptr<Animation> anim) {
    animMap[name] = std::move(anim);
}


// Getters
sh_ptr_ply GameManager::getPlayer() {
    for (auto& e : entityList) {
        if (e->isEntityAPlayer()) {
            return std::dynamic_pointer_cast<Player>(e);
        }
    }
    return nullptr;
}
sh_ptr_e GameManager::getBoss() {
    for (auto& e : entityList) {
        if (e->isEntityAnEnemyBoss()) {
            return e;
        }
    }
    return nullptr;
}


// Logic Functions
void GameManager::playerTakeHit(const sh_ptr_ply& p, int damage) {
    //TODO: move to player class
    if (p->doesPlayerHaveShield()) {
        //TODO: Add shield hit sound and animation
        p->hitShield();
        print("Shield Hit");
    }else {
        //TODO: Add player hit sound and animation
        p->removeHearts(damage);
        textMap["PlayerHearts"]->setText("Hearts: " + std::to_string(p->getHearts()));
        print("Player Hit");
    }
}

void GameManager::bounceEntities(sh_ptr_e e1, sh_ptr_e e2) {
    vector2 e1Pos = e1->getPosition();
    vector2 e2Pos = e2->getPosition();
    Heading h = getHeadingFromVectors(e1Pos, e2Pos);
    print("Bounce Heading: ", h.get());
    vector2 newVel = angleToVector2(h) * 0.5f;
    h.set(h.get() + 180);
    print("Rebounce Heading: ", h.get());
    vector2 otherVel = angleToVector2(h) * 0.5f;
    e1->setVelocity(otherVel);
    e2->setVelocity(newVel);
    e1->setKnockedBack(true, 250.0f);
    e2->setKnockedBack(true, 250.0f);
}
