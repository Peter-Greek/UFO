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

#include "Laser.h"

void Laser::update(float deltaMs) {
    if (!inWorld()) {print("laser not in world"); return;}
//    print("Laser Update: ", inFire, lastFired);
    if (inFire == 0) {
        if (lastFired > interval) {
            lastFired = 0;
            fire();
        }else {
            lastFired += deltaMs;
        }
    }else {
        if (inFire > duration) {
            inFire = 0;
            stopFire();
        }else {
            inFire += deltaMs;
            if (spinning) {
                dir.set(dir.get() + (clockWise ? 1 : -1) * speed);
            }
        }
    }
}

void Laser::fire() {
    inFire = 1;
}

void Laser::stopFire() {
    inFire = 0;
}

bool Laser::isFiring() {
    return inFire > 0;
}

Heading Laser::getHeading() {
    return dir;
}

void Laser::setHeading(Heading h) {
    dir.set(h.get());
}

void Laser::setSpin(bool s) {
    spinning = s;
}

int Laser::getDamage() const {
    return damage;
}
void Laser::setDamage(int d) {
    damage = d;
}

bool Laser::isSpinning() const {
    return spinning;
}

int Laser::getSpeed() const {
    return speed;
}
void Laser::setSpeed(int s) {
    speed = s;
}

float Laser::getInterval() const {
    return interval;
}

float Laser::getDuration() const {
    return duration;
}


float Laser::timeLeft() {
    return duration - inFire;
}




