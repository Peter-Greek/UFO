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

#ifndef CSCI437_GAMEMANAGER_H
#define CSCI437_GAMEMANAGER_H

#include <SDL.h>
#include "xProcess.h"
#include "Player.h"
#include "AT.h"
#include "Laser.h"
#include "camera.h"
#include "world.h"
#include "text.h"
#include "AsepriteLoader.h"
#include "TxdLoader.h"
#include "vector2.h"
#include "ProcessManager.h"
#include "entity.h"
#include "wall.h"
#include "AudioLoader.h"
#include "Boss.h"


class GameManager : public xProcess {
public:
    enum db_WCS {CREATION_NOT_STARTED, COORDS_SET, LENGTH_SET, WIDTH_SET, WALL_CREATED};
private:
    ProcessManager* PM;
    bool gameRunning = false;
    std::list<entity*> entityList;
    std::map<std::string, text*> textMap;
    std::map<std::string, AsepriteLoader*> asepriteMap;
    std::map<std::string, Animation*> animList;
    std::map<std::string, TxdLoader*> txdMap;
    std::map<std::string, AudioLoader*> audioMap;
    camera* cam;
    world* worldMap;

    passFunc_t passFunc;


    wall* debugWall = nullptr;
    // debug wall creation state
    db_WCS debugWallSate = CREATION_NOT_STARTED;
    bool inClick = false;
    int db_room_index = -1;
    int db_wall_index = -1;
    bool inShiftFind = false;
    vector2 shiftFindStart;

public:
    explicit GameManager(passFunc_t& func) : xProcess(false, func), passFunc(func) {}

    int initialize() override;
    void update(float deltaMs) override;
    bool isDone() override { return !gameRunning; };
    void postSuccess() override {};
    void postFail() override {};
    void postAbort() override {};

    void attachProcessManager(ProcessManager* pm);
    void attachEntity(entity* e);
    void setCamera(camera* c);
    void setWorld(world *w);
    void attachText(std::string name, text* t);
    void attachAseprite(std::string name, AsepriteLoader *a);
    void attachTxd(std::string name, TxdLoader *txd);
    void attachAudio(const std::string& name, AudioLoader *audio);

    static void bounceEntities(entity* e1, entity* e2);

    void handleEnemyUpdate(entity *e, float deltaMs);
    void handlePlayerUpdate(entity *e, float deltaMs);

    void updatePlayerView(bool isVisible, entity *e, float deltaMs);


    void playerTakeHit(Player *p, int damage);

    void renderLaser(vector2 screenCoords, vector2 dim, Laser *l);

    void renderEnemy(vector2 screenCoords, vector2 dim, entity *e);

    void renderWorld(float deltaMs);

    void handleDebugWorldCreator(float deltaMs);

    void drawWall(wall *cur_wall);

    void renderAT(vector2 screenCoords, vector2 dim, AT *at);

    void renderHeart(vector2 screenCoords, vector2 dim, entity *e);

    void renderHeart(vector2 screenCoords, vector2 dim);
};


#endif //CSCI437_GAMEMANAGER_H
