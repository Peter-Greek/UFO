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

int Player::localInit() {
    // Chat State To prevent key presses from doing anything when the chat box is open
    AddEventHandler("UFO::Chat::State", [this](bool state) {
        chatState = state;
    });

    RegisterCommand("setHearts", [this](std::string command, sList_t args, std::string message) {
        if (args.empty()) {
            TriggerEvent("UFO::Chat::AddMessage", "Incorrect Usage: setHearts <int>");
            return;
        }
        int hearts = std::stoi(args[0]);
        setHearts(hearts);
    });

    initalized = true;
    return 1;
}

void Player::update(float deltaMs) {
    if (!initalized) {localInit();}
    updateInvincibility(deltaMs);
    updateInvisibility(deltaMs);
    updateOxygen(deltaMs);
    updateATCannonFire(deltaMs);


    const Uint8 *keyboard_state_array = SDL_GetKeyboardState(nullptr);

    // Use upgrades
    bool invisibilityPressed = keyboard_state_array[UPGRADE_KEYS::INVISIBILITY_KEY];
    bool atCannonPressed = keyboard_state_array[UPGRADE_KEYS::AT_CANNON_KEY];

    // idk if the bellow will do anything when pressed
    bool shieldPressed = keyboard_state_array[UPGRADE_KEYS::SHIELD_KEY];
    bool speedPressed = keyboard_state_array[UPGRADE_KEYS::SPEED_KEY];
    bool oxygenPressed = keyboard_state_array[UPGRADE_KEYS::OXYGEN_KEY];

    bool upPressed = keyboard_state_array[SDL_SCANCODE_W] || keyboard_state_array[SDL_SCANCODE_UP];
    bool downPressed = keyboard_state_array[SDL_SCANCODE_S] || keyboard_state_array[SDL_SCANCODE_DOWN];
    bool leftPressed = keyboard_state_array[SDL_SCANCODE_A] || keyboard_state_array[SDL_SCANCODE_LEFT];
    bool rightPressed = keyboard_state_array[SDL_SCANCODE_D] || keyboard_state_array[SDL_SCANCODE_RIGHT];


    if (chatState) {
        upPressed = false;
        downPressed = false;
        leftPressed = false;
        rightPressed = false;
        invisibilityPressed = false;
        atCannonPressed = false;
        shieldPressed = false;
        speedPressed = false;
        oxygenPressed = false;
    }


    //TODO: eventually make it so each key is changeable
    if (invisibilityPressed) {
        if (getUpgradeLevel(Player::UPGRADES::INVISIBILITY) > 0 && !isInvisible()) {
            print("Player is invisible");
            setInvisible(true);
        }
    }

    if (atCannonPressed) {
        if (getUpgradeLevel(Player::UPGRADES::AT_CANNON) > 0 && !isATCannonFire()) {
            if (getATCount() > 0) {
                print("Player is firing AT Cannon");
                setATCannonFire(true);
            }else {
                print("Player has no AT to fire");
            }
        }
    }

    if (isKnockedBack()) {
        upPressed = false;
        downPressed = false;
        leftPressed = false;
        rightPressed = false;
    }

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
            facingLeft = true;
            if (curVel.x <= -PLAYER_SPEED) {
                newVel.x = -PLAYER_SPEED;
            } else {
                newVel.x = (curVel.x < -0.1f ? curVel.x : -0.1f) * 3;
                if (newVel.x < -PLAYER_SPEED) {
                    newVel.x = -PLAYER_SPEED;
                }
            }
        } else {
            facingLeft = false;
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

    /*newVel.x == 0.0f && newVel.y == 0.0f*/
    if (remainingKnockback() - deltaMs > 0 && isKnockedBack()) {
        setKnockedBack(true, remainingKnockback() - deltaMs);
        newVel = curVel;
    } else {
        setKnockedBack(false, 0.0f);
    }

    setVelocity(newVel);
}

bool Player::isFacingLeft() {
    return facingLeft;
}

// Upgrade functions
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
        INVISIBILITY_DURATION += level;
    } else if (upgrade == Player::UPGRADES::AT_CANNON) {
        // increase attack cannon damage
        if (level > 1) {
            AT_CANNON_DAMAGE = 2;
        }
    }

}
int Player::getUpgradeLevel(Player::UPGRADES upgrade) {
    return upgradeLevels[upgrade];
}

bool Player::doesPlayerHaveUpgrade(Player::UPGRADES upgrade) {
    return upgradeLevels[upgrade] > 0;
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

// Oxygen functions
float Player::getOxygenLevel() {
    return OXYGEN_LEVEL;
}
void Player::setOxygenLevel(float newOxygenLevel) {
    OXYGEN_LEVEL = newOxygenLevel;
    if (OXYGEN_LEVEL > MAX_OXYGEN_TIME) {
        OXYGEN_LEVEL = MAX_OXYGEN_TIME;
    } else if (OXYGEN_LEVEL < 0) {
        OXYGEN_LEVEL = 0;
    }
}
void Player::addOxygen(float oxygenToAdd) {
    OXYGEN_LEVEL += oxygenToAdd;
    if (OXYGEN_LEVEL > MAX_OXYGEN_TIME) {
        OXYGEN_LEVEL = MAX_OXYGEN_TIME;
    } else if (OXYGEN_LEVEL < 0) {
        OXYGEN_LEVEL = 0;
    }
}
void Player::updateOxygen(float ms) {
    if (OXYGEN_LEVEL > 0) {
        OXYGEN_LEVEL -= ms;
        if (OXYGEN_LEVEL <= 0) {
            // player dies
            print("Player died from lack of oxygen");
        }
    }
}
std::string Player::getOxygenString() const {
    int seconds = (int) (OXYGEN_LEVEL / 1000);
    int minutes = seconds / 60;
    seconds = seconds % 60;
    std::string sec = std::to_string(seconds);
    if (seconds < 10) {
        sec = "0" + sec;
    }
    return std::to_string(minutes) + ":" + sec;
}

// AT functions
void Player::addATCount() {
    AT_COUNT++;
    print("AT Count: ", AT_COUNT);
}
int Player::getATCount() const {
    return AT_COUNT;
}
void Player::removeATCount(int count) {
    AT_COUNT -= count;
    if (AT_COUNT < 0) {
        AT_COUNT = 0;
    }
}

// Invisibility functions
void Player::setInvisible(bool invisible) {
    isInvisible_v = invisible;
    if (isInvisible_v) {
        invisibilityTime = 0;
    }
}
bool Player::isInvisible() const {
    return isInvisible_v;
}
void Player::setInvisibilityTime(float time) {
    invisibilityTime = time;
}
float Player::getInvisibilityTime() const {
    return invisibilityTime;
}
void Player::updateInvisibility(float ms) {
    if (isInvisible()) {
        invisibilityTime += ms;
        if (invisibilityTime >= INVISIBILITY_DURATION * 1000) {
            setInvisible(false);
        }
    }
}

// AT Cannon functions
void Player::setATCannonFire(bool fire) {
    inATCannonFire = fire;
    if (inATCannonFire) {
        atCannonFireTime = atCannonFireInterval;
    }else {
        atCannonFireTime = 0;
    }
    projectileCreated = false;
}
bool Player::isATCannonFire() const {
    return inATCannonFire;
}
void Player::updateATCannonFire(float ms) {
    if (isATCannonFire()) {
        atCannonFireTime -= ms;
        if (atCannonFireTime <= 0) {
            setATCannonFire(false);
        }
    }
}
float Player::getATCannonFireTime() const {
    return atCannonFireTime;
}
void Player::setProjectileCreated(bool created) {
    projectileCreated = created;
}
bool Player::isProjectileCreated() const {
    return projectileCreated;
}
int Player::getATCannonDamage() const {
    return AT_CANNON_DAMAGE;
}


