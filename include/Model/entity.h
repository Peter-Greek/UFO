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

#ifndef CSCI437_ENTITY_H
#define CSCI437_ENTITY_H

#include "xProcess.h"

class entity : public xProcess {
public:
    enum eType {
        PLAYER,
        ENEMY,
        ENEMY_BOSS,
        LASER,
        ITEM_PICKUP,
        PROJECTILE,
    };

    enum pType {
        AT,
        HEART,
        OXY_TANK,
        KEY_CARD,
    };

private:
    vector2 spawnCoords;
    bool created = false;
    bool spawned = false;
    eType type;
    int hearts;
    int maxHearts;
    vector2 coords;
    vector2 lastCoords;
    vector2 vel;

    bool inKnockback = false;
    float knockbackRemaining = 0;
    bool isInvincible = false; // is the player invincible
    float invincibleTime = 0; // time left of invincibility

    int length = 10;
    int width = 10;

    void setDefaultLengthWidth() {
        if (type == PLAYER) {
            length = 32;
            width = 32;
        }
    }
public:
    explicit entity(
            passFunc_t& func,
            int eTypeIndex, int hearts, vector2 position
    ) : xProcess(false, func),hearts(hearts), maxHearts(hearts), coords(position) {
        type = static_cast<eType>(eTypeIndex);
        spawnCoords = {position.x, position.y};
        lastCoords = {position.x, position.y};
        setDefaultLengthWidth();
    }

    explicit entity(
            passFunc_t& func,
            int eTypeIndex, int hearts, vector2 position,
            int length, int width
    ) : xProcess(false, func), length(length), width(width), hearts(hearts), maxHearts(hearts), coords(position) {
        type = static_cast<eType>(eTypeIndex);
        spawnCoords = {position.x, position.y};
        lastCoords = {position.x, position.y};
        setDefaultLengthWidth();
    }

    int initialize() override;
    void update(float deltaMs) override;
    bool isDone() override;
    void postSuccess() override {};
    void postFail() override {};
    void postAbort() override {};

    // get length and width
    [[nodiscard]] int getLength() const;
    [[nodiscard]] int getWidth() const;
    [[nodiscard]] vector2 getCenter() const;
    [[nodiscard]] vector2 getDimensions() const;

    bool isEntityAPlayer();
    bool isEntityAnEnemy();
    bool isEntityAnEnemyBoss();
    bool isEntityALaser();
    bool isEntityAPickup();
    bool isEntityAProjectile();

    pType getPickupType();

    [[nodiscard]] bool isKnockedBack() const;
    [[nodiscard]] float remainingKnockback() const;
    void setKnockedBack(bool knockedBack, float knockbackTime);


    [[nodiscard]] int getHearts() const;
    void setHearts(int newHearts);
    void addHearts(int heartsToAdd);
    void removeHearts(int heartsToRemove);
    [[nodiscard]] int getMaxHearts() const;
    void setMaxHearts(int newMaxHearts);

    void setPosition(vector2 newPosition);
    vector2 getPosition();
    void updateCoordsFromVelocity(float deltaMs);
    vector2 getLastCoords();

    void setVelocity(vector2 newVelocity);
    vector2 getVelocity();

    void spawn();
    [[nodiscard]] bool inWorld() const;
    vector2 getSpawnCoords();

    void updateInvincibility(float deltaMs);
    [[nodiscard]] bool isEntityInvincible() const;
    void setEntityInvincible(bool invincible);
    void setEntityInvincible(bool invincible, float time);
    void setEntityInvincibleTime(float time);
};


#endif //CSCI437_ENTITY_H
