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

#ifndef CSCI437_PLAYER_H
#define CSCI437_PLAYER_H

#include "entity.h"
using upgradeList_t = std::array<int, 5>;
class Player : public entity {
private:
    bool initalized = false;

    upgradeList_t upgradeLevels = {0, 0, 0, 0, 0}; // upgrade levels for each upgrade

    float BASE_OXYGEN_TIME = 3 * 60 * 1000 + 0.0f; // base time for the player without any upgrades
    float MAX_OXYGEN_TIME = BASE_OXYGEN_TIME; // max oxy time the player has during this run after upgrades
    float OXYGEN_LEVEL = 3 * 60 * 1000; // current time left; max is MAX_OXYGEN_TIME during a run

    int SHIELD_COUNT = 0; // count of how many shields the player has
    float PLAYER_SPEED = 0.2; // base speed of the player
    int INVISIBILITY_DURATION = 2; // duration of invisibility
    int AT_CANNON_DAMAGE = 1; // damage of the AT cannon

    int AT_COUNT = 0; // count of how much AT has been collected this run

    bool facingLeft = false; // facing left when idle after moving left
    bool isInvisible_v = false; // is the player invisible
    float invisibilityTime = 0; // time the player has been invisible

    bool inATCannonFire = false; // is the player in the middle of firing the AT cannon
    float atCannonFireTime = 0; // time the player has been firing the AT cannon
    bool projectileCreated = false; // has the projectile been created for this fire
    const float atCannonFireInterval = 1000; // time the player has to wait between AT cannon shots

    bool chatState = false; // is the chat box open
public:
    enum UPGRADE_KEYS {
        INVISIBILITY_KEY = SDL_SCANCODE_1,
        AT_CANNON_KEY = SDL_SCANCODE_2,

        SHIELD_KEY = SDL_SCANCODE_3,
        SPEED_KEY = SDL_SCANCODE_4,
        OXYGEN_KEY = SDL_SCANCODE_5,
    };

    enum UPGRADES { // DONT CHANGE ORDER
        OXYGEN,
        SHIELD,
        SPEED,
        INVISIBILITY,
        AT_CANNON
    };
    explicit Player(
            passFunc_t& func
    ) : entity(func, entity::PLAYER, 5, {0.0f, 0.0f}) {}
    explicit Player(
            passFunc_t& func, upgradeList_t upgradesSet
    ) : entity(func, entity::PLAYER, 5, {0.0f, 0.0f}) {
        // for each enum upgrade, set the upgrade level to the level passed in
        for (int i = 0; i < 5; i++) {
            applyUpgrade((UPGRADES) i, upgradesSet[i]);
        }
    }

    int localInit();
    void update(float deltaMs) override;

    // Upgrade functions
    void applyUpgrade(UPGRADES upgrade, int level);
    int getUpgradeLevel(UPGRADES upgrade);
    bool doesPlayerHaveUpgrade(UPGRADES upgrade);

    // Shield functions
    bool doesPlayerHaveShield();
    void removeShield();
    void hitShield();
    float getPlayerSpeed();

    // Oxygen functions
    float getOxygenLevel();
    void updateOxygen(float ms);
    void setOxygenLevel(float newOxygenLevel);
    void addOxygen(float oxygenToAdd);
    [[nodiscard]] std::string getOxygenString() const;

    // Invisibility functions
    void setInvisible(bool invisible);
    [[nodiscard]] bool isInvisible() const;
    void setInvisibilityTime(float time);
    [[nodiscard]] float getInvisibilityTime() const;
    void updateInvisibility(float ms);

    // AT Cannon functions
    void setATCannonFire(bool fire);
    [[nodiscard]] bool isATCannonFire() const;
    void updateATCannonFire(float ms);
    [[nodiscard]] float getATCannonFireTime() const;
    void setProjectileCreated(bool created);
    [[nodiscard]] bool isProjectileCreated() const;
    [[nodiscard]] int getATCannonDamage() const;

    // AT functions
    void addATCount();
    [[nodiscard]] int getATCount() const;
    void removeATCount(int count);

    // Misc functions
    bool isFacingLeft();
};


#endif //CSCI437_PLAYER_H
