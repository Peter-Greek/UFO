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
    updateCoordsFromVelocity(deltaMs);
}

bool entity::isDone() {
    return created && !spawned;
}

void entity::postSuccess() {

}

void entity::postFail() {

}

void entity::postAbort() {

}

void entity::spawn() {
    spawned = true;
}

bool entity::inWorld() {
    return spawned;
}

// Entity Type Methods
bool entity::isEntityAPlayer() {
    return type == PLAYER;
};
bool entity::isEntityAnEnemy() {
    return type == ENEMY;
};
bool entity::isEntityAnEnemyBoss() {
    return type == ENEMY_BOSS;
};
bool entity::isEntityAPickup() {
    return type == ITEM_PICKUP;
};

// Health Methods
int entity::getHearts() const {
    return hearts;
};
void entity::setHearts(int newHearts) {
    hearts = newHearts;
};
void entity::addHearts(int heartsToAdd) {
    hearts += heartsToAdd;
};
void entity::removeHearts(int heartsToRemove) {
    hearts -= heartsToRemove;
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
    coords += vel * deltaMs;

    if (coords.y < WORLD_MIN_Y) {
        coords.y = WORLD_MIN_Y;
    } else if (coords.y > WORLD_MAX_Y - 10) {
        coords.y = WORLD_MAX_Y - 10;
    }
}

// Velocity Methods
void entity::setVelocity(vector2 newVelocity) {
    vel = newVelocity;
};

vector2 entity::getVelocity() {
    return vel;
}
