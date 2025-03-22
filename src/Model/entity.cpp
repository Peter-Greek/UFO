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

#include "entity.h"

int entity::initialize() {
    created = true;
    return 1;
}

void entity::update(float deltaMs) {
    updateInvincibility(deltaMs);
}

bool entity::isDone() {
    return created && !spawned;
}

void entity::spawn() {
    spawned = true;
}

bool entity::inWorld() const {
    return spawned;
}

vector2 entity::getSpawnCoords() {
    return spawnCoords;
}

// Entity Type Methods
bool entity::isEntityAPlayer() {return type == PLAYER;};
bool entity::isEntityAnEnemy() {return type == ENEMY;};
bool entity::isEntityAnEnemyBoss() {return type == ENEMY_BOSS;};
bool entity::isEntityALaser() {return type == LASER;}
bool entity::isEntityAPickup() {return type == ITEM_PICKUP;};
bool entity::isEntityAProjectile() {return type == PROJECTILE;};

entity::pType entity::getPickupType() {
    int rel = getMaxHearts();
    return (pType) rel;
}


// Health Methods
int entity::getHearts() const {
    return hearts;
};
void entity::setHearts(int newHearts) {
    hearts = newHearts;
    if (hearts > maxHearts) {
        hearts = maxHearts;
    }
};
void entity::addHearts(int heartsToAdd) {
    hearts += heartsToAdd;
    if (hearts > maxHearts) {
        hearts = maxHearts;
    }
};
void entity::removeHearts(int heartsToRemove) {
    if (isInvincible) {return;}
    hearts -= heartsToRemove;
    if (hearts < 0) {
        hearts = 0;
    }
};

// Max Health Methods
int entity::getMaxHearts() const {
    return maxHearts;
};
void entity::setMaxHearts(int newMaxHearts) {
    maxHearts = newMaxHearts;
}

// Position Methods
void entity::setPosition(vector2 newPosition) {
    coords = newPosition;
};

vector2 entity::getPosition() {
    return coords;
}

void entity::updateCoordsFromVelocity(float deltaMs) {
    lastCoords = {coords.x, coords.y};
    coords += vel * deltaMs;

    if (coords.y < WORLD_MIN_Y) {
        coords.y = WORLD_MIN_Y;
    } else if (coords.y > WORLD_MAX_Y - 10) {
        coords.y = WORLD_MAX_Y - 10;
    }
}

vector2 entity::getLastCoords() {
    return lastCoords;
}

// Velocity Methods
void entity::setVelocity(vector2 newVelocity) {
    vel = newVelocity;
};

vector2 entity::getVelocity() {
    return vel;
}

// Knockback Methods
bool entity::isKnockedBack() const {
    return inKnockback;
}

float entity::remainingKnockback() const {
    return knockbackRemaining;
}

void entity::setKnockedBack(bool knockedBack, float knockbackTime) {
    inKnockback = knockedBack;
    knockbackRemaining = knockbackTime;
}

int entity::getLength() const {
    return length;
}

int entity::getWidth() const {
    return width;
}

vector2 entity::getCenter() const {
    return {coords.x + length / 2, coords.y + width / 2};
}

vector2 entity::getDimensions() const {
    return {static_cast<float>(length), static_cast<float>(width)};
}

bool entity::isEntityInvincible() const {
    return isInvincible;
}

void entity::setEntityInvincible(bool invincible) {
    isInvincible = invincible;
}

void entity::setEntityInvincible(bool invincible, float time) {
    isInvincible = invincible;
    invincibleTime = time;
}

void entity::setEntityInvincibleTime(float time) {
    invincibleTime = time;
}

void entity::updateInvincibility(float deltaMs) {
    if (invincibleTime > 0) {
        invincibleTime -= deltaMs;
        if (invincibleTime <= 0) {
            invincibleTime = 0;
            isInvincible = false;
        }
    }
}

bool entity::isPointInEntity(vector2 point) const {
    vector2 p1 = coords + vector2(-length/2, width/2); // Top-left
    vector2 p2 = coords + vector2(length/2, width/2); // Top-right
    vector2 p3 = coords + vector2(length/2, -width/2); // Bottom-right
    vector2 p4 = coords + vector2(-length/2, -width/2); // Bottom-left
    return isPointInBounds(point, {p1, p2, p3, p4});
}

void entity::setLength(int newLength) {
    length = newLength;
}

void entity::setWidth(int newWidth) {
    width = newWidth;
}