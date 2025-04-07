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

using sh_ptr_e = sh_ptr<entity>;
using sh_ptr_at = sh_ptr<AT>;
using sh_ptr_laser = sh_ptr<Laser>;
using sh_ptr_ply =  sh_ptr<Player>;

class GameManager : public xProcess {
public:
    enum db_WCS {CREATION_NOT_STARTED, COORDS_SET, LENGTH_SET, WIDTH_SET, WALL_CREATED};
private:
    sh_ptr<ProcessManager> pM;
    bool gameRunning = false;
    std::list<sh_ptr<entity>> entityList;
    std::map<std::string, sh_ptr<text>> textMap;
    std::map<std::string, sh_ptr<AsepriteLoader>> asepriteMap;
    std::map<std::string, sh_ptr<Animation>> animMap;
    std::map<std::string, sh_ptr<TxdLoader>> txdMap;
    std::map<std::string, sh_ptr<AudioLoader>> audioMap;
    sh_ptr<camera> cam;
    sh_ptr<world> world_ptr;
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
    void postSuccess() override {terminateGame();};
    void postFail() override {terminateGame();};
    void postAbort() override {terminateGame();};

    void setProcessManager(sh_ptr<ProcessManager> pm);
    void setCamera(sh_ptr<camera> c);
    void setWorld(std::shared_ptr<world> w);

    void attachEntity(sh_ptr<entity> e);
    void attachText(const std::string& name, sh_ptr<text> t);
    void attachAseprite(const std::string& name, sh_ptr<AsepriteLoader> a);
    void attachTxd(const std::string& name, sh_ptr<TxdLoader> txd);
    void attachAudio(const std::string& name, sh_ptr<AudioLoader> audio);
    void attachAnim(const std::string& name, sh_ptr<Animation> anim);

    sh_ptr_ply getPlayer();
    sh_ptr_e getBoss();

    static void bounceEntities(sh_ptr_e e1, sh_ptr_e e2);
    void playerTakeHit(const sh_ptr_ply& p, int damage);

    void handleEnemyUpdate(const sh_ptr_e& e, float deltaMs);
    void handlePlayerUpdate(const sh_ptr_e& e, float deltaMs);

    void updatePlayerView(bool isVisible, const sh_ptr_e& e, float deltaMs);
    void renderLaser(vector2 screenCoords, vector2 dim, const sh_ptr_laser& l);
    void renderEnemy(vector2 screenCoords, vector2 dim, const sh_ptr_e& e);
    void renderWorld(float deltaMs);
    void handleDebugWorldCreator(float deltaMs);
    void drawWall(wall *cur_wall);
    void renderAT(vector2 screenCoords, vector2 dim, const sh_ptr_at& at);
    void renderHeart(vector2 screenCoords, vector2 dim, const sh_ptr_e& e);
    void renderHeart(vector2 screenCoords, vector2 dim);

    void handleBossUpdate(const sh_ptr_e& e, float deltaMs);

    void terminateGame();

    void renderTextOnEntity(const sh_ptr_e &e, const std::string &textMapName, const std::string &textDefault);

    void renderPickupInteraction(const sh_ptr_ply &ply, const sh_ptr_e &e, vector2 &currentCoords);

    void clearPickupInteraction(const sh_ptr_e &e);
};


#endif //CSCI437_GAMEMANAGER_H
