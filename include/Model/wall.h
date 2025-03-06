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
// Created by xerxe on 3/6/2025.
//

#ifndef CSCI437_WALL_H
#define CSCI437_WALL_H

#include "Util.h"

class wall {
public:
    vector2 position;
    float length;
    float width;
    float angle; // Rotation in degrees

    wall(const vector2& pos, float len, float w, float a)
            : position(pos), length(len), width(w), angle(a) {}

    [[nodiscard]] std::array<vector2, 4> getCorners() const {
        vector2 dir = angleToVector2(angle); // Convert angle to unit direction
        vector2 perp(-dir.y, dir.x); // Perpendicular for width

        vector2 p1 = position - (perp * (width / 2)); // Bottom-left
        vector2 p2 = position + (perp * (width / 2)); // Bottom-right
        vector2 p3 = p2 + (dir * length); // Top-right
        vector2 p4 = p1 + (dir * length); // Top-left

        return {p1, p2, p3, p4};
    }
};

#endif //CSCI437_WALL_H
