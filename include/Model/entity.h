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
        AT = 1,
        HEART = 2,
        OXY_TANK = 3,
        KEY_CARD = 4,
        ESCAPE_POD = 5,
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

    int appliedLength = 10; // length applied to the entity
    int appliedWidth = 10; // width applied to the entity

    void setDefaultLengthWidth() {
        vector2 def = getDefLengthWidth();
        length = def.x;
        width = def.y;
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
        appliedLength = length;
        appliedWidth = width;
        type = static_cast<eType>(eTypeIndex);
        spawnCoords = {position.x, position.y};
        lastCoords = {position.x, position.y};
        setDefaultLengthWidth();
    }

    int initialize() override;
    void update(float deltaMs) override;
    bool isDone() override;
    void postSuccess() override { onTriggerEvent("ENTITY::SUCCEED", json {}); };
    void postFail() override { onTriggerEvent("ENTITY::FAILED", json {}); };
    void postAbort() override {onTriggerEvent("ENTITY::ABORTED", json {}); };

    // get length and width
    [[nodiscard]] int getLength() const;
    [[nodiscard]] int getWidth() const;
    void setLength(int newLength);
    void setWidth(int newWidth);
    [[nodiscard]] vector2 getCenter() const;
    [[nodiscard]] vector2 getDimensions() const;
    vector2 getDefLengthWidth() {
        // this is staying in h file as its basically a config
        switch (type) {
            case PLAYER:
                return {64, 48};
            case ENEMY:
                return {32, 32};
            case ITEM_PICKUP:
                switch (getPickupType()) {
                    case HEART:
                        return {16, 16};
                    case OXY_TANK:
                        return {16, 16};
                    case KEY_CARD:
                        return {16, 16};
                    case AT:
                        return {32, 32};
                    default:
                        return {static_cast<float>(appliedLength), static_cast<float>(appliedWidth)};
                }
            default:
                return {static_cast<float>(appliedLength), static_cast<float>(appliedWidth)};
        }
    }

    [[nodiscard]] bool isPointInEntity(vector2 point) const;

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
