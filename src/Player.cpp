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

#include "Player.h"

void Player::update(float deltaMs) {
    const Uint8 *keyboard_state_array = SDL_GetKeyboardState(nullptr);

    bool upPressed = keyboard_state_array[SDL_SCANCODE_W] || keyboard_state_array[SDL_SCANCODE_UP];
    bool downPressed = keyboard_state_array[SDL_SCANCODE_S] || keyboard_state_array[SDL_SCANCODE_DOWN];
    bool leftPressed = keyboard_state_array[SDL_SCANCODE_A] || keyboard_state_array[SDL_SCANCODE_LEFT];
    bool rightPressed = keyboard_state_array[SDL_SCANCODE_D] || keyboard_state_array[SDL_SCANCODE_RIGHT];

    bool changeY = (upPressed  ^ downPressed);
    bool changeX = (leftPressed ^ rightPressed);

    vector2 newVel = vector2(0.0, 0.0);
    vector2 curVel = getVelocity();

    if (changeY) {
        if (upPressed) {
            if (curVel.y <= -PLAYER_SPEED) {
                newVel.y = -PLAYER_SPEED;
            } else {
                newVel.y = (curVel.y < -0.1f ? curVel.y : -0.1f) * 3;
                if (newVel.y < -PLAYER_SPEED) {
                    newVel.y = -PLAYER_SPEED;
                }
            }
        } else {
            if (curVel.y >= PLAYER_SPEED) {
                newVel.y = PLAYER_SPEED;
            } else {
                newVel.y = (curVel.y > 0.1f ? curVel.y : 0.1f) * 3;
                if (newVel.y > PLAYER_SPEED) {
                    newVel.y = PLAYER_SPEED;
                }
            }
        }
    }else {
        if (curVel.y != 0.0f) {
            newVel.y = curVel.y * 0.25f; // reduce velocity
            if (newVel.y < 0.1f && newVel.y > -0.1f) {
                newVel.y = 0.0f; // stop the paddle
            }
        }
    }

    if (changeX) {
        if (leftPressed) {
            if (curVel.x <= -PLAYER_SPEED) {
                newVel.x = -PLAYER_SPEED;
            } else {
                newVel.x = (curVel.x < -0.1f ? curVel.x : -0.1f) * 3;
                if (newVel.x < -PLAYER_SPEED) {
                    newVel.x = -PLAYER_SPEED;
                }
            }
        } else {
            if (curVel.x >= PLAYER_SPEED) {
                newVel.x = PLAYER_SPEED;
            } else {
                newVel.x = (curVel.x > 0.1f ? curVel.x : 0.1f) * 3;
                if (newVel.x > PLAYER_SPEED) {
                    newVel.x = PLAYER_SPEED;
                }
            }
        }
    }else {
        if (curVel.x != 0.0f) {
            newVel.x = curVel.x * 0.25f; // reduce velocity
            if (newVel.x < 0.1f && newVel.x > -0.1f) {
                newVel.x = 0.0f; // stop the paddle
            }
        }
    }

    setVelocity(newVel);
    updateCoordsFromVelocity(deltaMs);
}

void Player::applyUpgrade(Player::UPGRADES upgrade, int level) {
    upgradeLevels[upgrade] = level;

    if (inWorld()) { // player already spawned in, upgrades only apply before player is spawned
        return;
    }

    if (upgrade == Player::UPGRADES::OXYGEN) {
        // increase oxygen capacity
        OXYGEN_LEVEL = BASE_OXYGEN_TIME + (level * 30 * 1000);
        MAX_OXYGEN_TIME = OXYGEN_LEVEL;
    } else if (upgrade == Player::UPGRADES::SHIELD) {
        // increase shield capacity
        SHIELD_COUNT = 0 + level;
    } else if (upgrade == Player::UPGRADES::SPEED) {
        // increase speed
        PLAYER_SPEED += (level * 0.05f);
    } else if (upgrade == Player::UPGRADES::INVISIBILITY) {
        // increase invisibility duration
        INVISIBILITY_DURATION += INVISIBILITY_DURATION + level;
    } else if (upgrade == Player::UPGRADES::AT_CANNON) {
        // increase attack cannon damage
        if (level > 1) {
            AT_CANNON_DAMAGE = 2;
        }
    }

}

// Shield functions
bool Player::doesPlayerHaveShield() {
    return SHIELD_COUNT > 0;
}

void Player::hitShield() {
    if (doesPlayerHaveShield()) {
        SHIELD_COUNT--;
    }
}

void Player::removeShield() {
    SHIELD_COUNT = 0;
}

// Player Speed functions
float Player::getPlayerSpeed() {
    return PLAYER_SPEED;
}

int Player::getOxygenLevel() {
    return OXYGEN_LEVEL;
}

void Player::setOxygenLevel(int newOxygenLevel) {
    OXYGEN_LEVEL = newOxygenLevel;
    if (OXYGEN_LEVEL > MAX_OXYGEN_TIME) {
        OXYGEN_LEVEL = MAX_OXYGEN_TIME;
    } else if (OXYGEN_LEVEL < 0) {
        OXYGEN_LEVEL = 0;
    }
}

void Player::addOxygen(int oxygenToAdd) {
    OXYGEN_LEVEL += oxygenToAdd;
    if (OXYGEN_LEVEL > MAX_OXYGEN_TIME) {
        OXYGEN_LEVEL = MAX_OXYGEN_TIME;
    } else if (OXYGEN_LEVEL < 0) {
        OXYGEN_LEVEL = 0;
    }
}




