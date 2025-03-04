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
// Created by xerxe on 2/6/2025.
//

#ifndef CSCI437_UTIL_H
#define CSCI437_UTIL_H

#include <cstdarg>
#include <iostream>
#include <random>
#include <cmath>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

template<class... Args>
void print(Args&&... args)
{
    ((std::cout << args << '\t'), ...) << '\n';
    std::cout.flush();  // Ensures output is printed immediately
}


template<class... Args>
void error(Args... args)
{
    std::cout << "SCRIPT ERROR: ";
    ((std::cout << args << '\t'), ...) << '\n';
    std::cout.flush();  // Ensures output is printed immediately
    exit(0);
}

float map_range(float s, float a1, float a2, float b1, float b2);

class vector2 {
public:
    vector2(): x(0.0f), y(0.0f) {}

    vector2(float x, float y): x(x), y(y){}

    vector2 operator+(float f) const
    {
        return {x + f, y + f};
    }

    vector2 operator-(float f) const
    {
        return {x - f, y - f};
    }

    vector2 operator*(float f) const
    {
        return {x * f, y * f};
    }

    vector2 operator/(float f) const
    {
        return {x / f, y / f};
    }

    vector2 operator-() const
    {
        return {-x, -y};
    }

    vector2 operator+(vector2 v2) const
    {
        return {x + v2.x, y + v2.y};
    }

    vector2 operator-(vector2 v2) const
    {
        return {x - v2.x, y - v2.y};
    }

    vector2 operator*(vector2 v2) const
    {
        return {x * v2.x, y * v2.y};
    }

    vector2 operator/(vector2 v2) const
    {
        return {x / v2.x, y / v2.y};
    }

    vector2& operator+=(const vector2 v2)
    {
        x += v2.x;
        y += v2.y;
        return *this;
    }

    vector2& operator-=(const vector2 v2)
    {
        x -= v2.x;
        y -= v2.y;
        return *this;
    }

    vector2& operator*=(const vector2 v2)
    {
        x *= v2.x;
        y *= v2.y;
        return *this;
    }

    vector2& operator/=(const vector2 v2)
    {
        x /= v2.x;
        y /= v2.y;
        return *this;
    }

    bool operator==(const vector2 v2) const
    {
        return x == v2.x && y == v2.y;
    }

    bool operator!=(const vector2 v2) const
    {
        return x != v2.x || y != v2.y;
    }

    float dot(const vector2 v2) const
    {
        return x * v2.x + y * v2.y;
    }

    float cross(const vector2 v2) const
    {
        return x * v2.y - y * v2.x;
    }

    float length() const
    {
        return sqrt(x * x + y * y);
    }

    float len() const
    {
        return sqrt(x * x + y * y);
    }

    float lengthSquared() const {
        return x * x + y * y;
    }

    vector2 normalize() const
    {
        float l = length();
        return {x / l, y / l};
    }

    // tostring
    std::string to_string() const
    {
        return "vector2(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }

    friend std::ostream& operator<<(std::ostream& os, const vector2& v) {
        os << "(" << v.x << ", " << v.y << ")";
        return os;
    }

    float x, y;


};

inline void to_json(json &j, const vector2 &s)
{
    j["X"] = s.x;
    j["Y"] = s.y;
}

inline void from_json(const json &j, vector2 &s)
{
    s.x = j.at("X").get<float>();
    s.y = j.at("Y").get<float>();
}

float random(float min, float max);
int random(int min, int max);
vector2 random(vector2 minVec, vector2 maxVec);

#endif //CSCI437_UTIL_H
