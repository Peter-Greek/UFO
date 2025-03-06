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

class GameManager : public xProcess {
private:
    bool gameRunning = false;
    std::list<entity*> entityList;
    std::map<std::string, text*> textMap;
    std::map<std::string, AsepriteLoader*> asepriteMap;
    std::map<std::string, Animation*> animList;
    std::map<std::string, TxdLoader*> txdMap;
    std::ostringstream oss;
    camera* cam;
    world* worldMap;

    passFunc_t passFunc;
public:
    explicit GameManager(passFunc_t& func) : xProcess(false, func), passFunc(func) {}

    int initialize() override;
    void update(float deltaMs) override;
    bool isDone() override { return !gameRunning; };
    void postSuccess() override {};
    void postFail() override {};
    void postAbort() override {};

    void attachEntity(entity* e);
    void setCamera(camera* c);
    void setWorld(world *w);
    void attachText(std::string name, text* t);
    void attachAseprite(std::string name, AsepriteLoader *a);
    void attachTxd(std::string name, TxdLoader *txd);

    static void bounceEntities(entity* e1, entity* e2);

    void handleEnemyUpdate(entity *e);
    void handlePlayerUpdate(entity *e);

    void updatePlayerView(bool isVisible, entity *e, float deltaMs);


    void playerTakeHit(Player *p, int damage);

    void renderLaser(vector2 screenCoords, vector2 dim, Laser *l);

    void renderEnemy(vector2 screenCoords, vector2 dim, entity *e);

    void renderWorld();
};


#endif //CSCI437_GAMEMANAGER_H
