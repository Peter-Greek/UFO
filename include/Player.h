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

class Player : public entity {
private:
    int upgradeLevels[5] = {0, 0, 0, 0, 0}; // upgrade levels for each upgrade
    int BASE_OXYGEN_TIME = 3 * 60 * 1000; // base time for the player without any upgrades
    int MAX_OXYGEN_TIME = BASE_OXYGEN_TIME; // max oxy time the player has during this run after upgrades

    int AT_COUNT = 0; // count of how much AT has been collected this run

    int OXYGEN_LEVEL = 3 * 60 * 1000; // current time left max is MAX_OXYGEN_TIME during a run
    int SHIELD_COUNT = 0; // count of how many shields the player has
    float PLAYER_SPEED = 0.2; // base speed of the player
    int INVISIBILITY_DURATION = 2; // duration of invisibility
    int AT_CANNON_DAMAGE = 1; // damage of the AT cannon

    bool facingLeft = false; // facing left when idle after moving left
public:
    enum UPGRADES { // DONT CHANGE ORDER
        OXYGEN,
        SHIELD,
        SPEED,
        INVISIBILITY,
        AT_CANNON
    };
    explicit Player(
            const std::function<void(const std::string& eventName, const json& eventData)>& func
    ) : entity(func, entity::PLAYER, 5, {0.0f, 0.0f}) {}
    explicit Player(
            const std::function<void(const std::string& eventName, const json& eventData)>& func, const int upgradesSet[5]
    ) : entity(func, entity::PLAYER, 5, {0.0f, 0.0f}) {
        // for each enum upgrade, set the upgrade level to the level passed in
        for (int i = 0; i < 5; i++) {
            upgradeLevels[i] = upgradesSet[i];
        }
    }

    void applyUpgrade(UPGRADES upgrade, int level);

    bool doesPlayerHaveShield();
    void removeShield();
    void hitShield();

    float getPlayerSpeed();

    int getOxygenLevel();
    void setOxygenLevel(int newOxygenLevel);
    void addOxygen(int oxygenToAdd);

    void update(float deltaMs) override;

    void addATCount() {
        AT_COUNT++;
        print("AT Count: ", AT_COUNT);
    }

    int getATCount() const {
        return AT_COUNT;
    }

    bool isFacingLeft();
};


#endif //CSCI437_PLAYER_H
