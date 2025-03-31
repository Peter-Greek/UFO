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
// Created by xerxe on 3/31/2025.
//

#ifndef CSCI437_PROJECTILE_H
#define CSCI437_PROJECTILE_H

#include "entity.h"
#include <memory>

class Projectile : public entity {
private:
    std::weak_ptr<entity> owner;
    float range = 1000;

public:
    explicit Projectile(passFunc_t& func, vector2 position)
            : entity(func, entity::PROJECTILE, 1, position) {}

    Projectile(passFunc_t& func, vector2 position, const std::shared_ptr<entity>& parent)
            : entity(func, entity::PROJECTILE, 1, position), owner(parent) {}

    Projectile(passFunc_t& func, vector2 position, int damage)
            : entity(func, entity::PROJECTILE, damage, position) {}

    Projectile(passFunc_t& func, vector2 position, int damage, const std::shared_ptr<entity>& parent)
            : entity(func, entity::PROJECTILE, damage, position), owner(parent) {}

    // Owner Management
    void setOwner(const std::shared_ptr<entity>& newOwner) {
        owner = newOwner;
    }

    [[nodiscard]] std::shared_ptr<entity> getOwner() const {
        return owner.lock();  // returns nullptr if expired
    }

    [[nodiscard]] bool hasOwner() const {
        return !owner.expired();
    }

    // Range Check
    void setRange(float r) { range = r; }
    [[nodiscard]] float getRange() const { return range; }
    bool isOutOfRange() {
        return (getPosition() - getSpawnCoords()).length() > range;
    }
};

#endif // CSCI437_PROJECTILE_H
