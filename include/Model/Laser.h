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
// Created by xerxe on 3/4/2025.
//

#ifndef CSCI437_LASER_H
#define CSCI437_LASER_H

#include "entity.h"
class Laser : public entity {
private:
    Heading dir;
    float interval;
    float duration;
    int damage;
    int speed;
    bool spinning = false;
    bool clockWise = false;

    // Time vars
    float lastFired = 0;
    float inFire = 0;
public:
    explicit Laser(
            passFunc_t& func,
            vector2 position,
            Heading h,
            int length,
            int width,
            float interval,
            float duration,
            int damage,
            int speed
    ) : entity(func, entity::LASER, damage, position, getScaledPixelWidth(length), getScaledPixelHeight(width)),
        dir(h),
        interval(interval),
        duration(duration),
        damage(damage),
        speed(speed)
    {}

    void update(float deltaMs) override;
    void fire();
    void stopFire();
    bool isFiring();
    Heading getHeading();
    void setHeading(Heading h);
    void setSpin(bool s);
    bool isSpinning() const;
    int getDamage() const;
    void setDamage(int d);
    int getSpeed() const;
    void setSpeed(int s);
    float getInterval() const;
    float getDuration() const;
    float timeLeft();
};


#endif //CSCI437_LASER_H
