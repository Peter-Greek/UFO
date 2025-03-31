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
    int length;
    int width;
    Heading heading;
    std::vector<vector2> corners;

    wall(const vector2& pos, int len, int w, int h)
            : position(pos), length(len), width(w), heading(h) {
        corners = getCorners();
    }

    wall(const vector2& pos, int len, int w, Heading h)
            : position(pos), length(len), width(w), heading(h) {
        corners = getCorners();
    }

    wall(): heading(0), length(0), width(0) {
        corners = getCorners();
    };

    [[nodiscard]] std::vector<vector2> getCorners() const {
        vector2 dir = angleToVector2(heading); // Convert angle to unit direction
        vector2 perp(-dir.y, dir.x); // Perpendicular for width

        vector2 p1 = position - (perp * (width / 2)); // Bottom-left
        vector2 p2 = position + (perp * (width / 2)); // Bottom-right
        vector2 p3 = p2 + (dir * length); // Top-right
        vector2 p4 = p1 + (dir * length); // Top-left

        return {p1, p2, p3, p4};
    }

    [[nodiscard]] bool isPointInWall(vector2 vec) const {
        return isPointInBounds(vec, corners);
    }

    [[nodiscard]] bool isRectangleInWall(const vectorList_t& rect) const {
        return isRectangleInRectangle(rect, corners);
    }
};

inline void to_json(json& j, wall &s) {
    j = json {
            {"coords", s.position},
            {"l", s.length},
            {"w", s.width},
            {"h", s.heading.get()}
    };
}

inline void from_json(const json& j, wall &s) {
    s.position = j.at("coords").get<vector2>();
    s.length = j.at("l").get<int>();
    s.width = j.at("w").get<int>();
    s.heading = Heading(j.at("h").get<int>());
    s.corners = s.getCorners();
}

inline void to_json(json& j, wall* s) {
    j = json {
            {"coords", s->position},
            {"l", s->length},
            {"w", s->width},
            {"h", s->heading.get()}
    };
}

inline void from_json(const json& j, wall* s) {
    s->position = j.at("coords").get<vector2>();
    s->length = j.at("l").get<int>();
    s->width = j.at("w").get<int>();
    s->heading = Heading(j.at("h").get<int>());
    s->corners = s->getCorners();
}

#endif //CSCI437_WALL_H
