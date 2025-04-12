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
#include <string>

#include <nlohmann/json.hpp>
#include "vector2.h"
#include "heading.h"
#include "config.h"

using json = nlohmann::json;
using UUID_t = std::string;
using passFunc_t = std::function<void(const std::string& eventName, const json& eventData)>;
using vectorList_t = std::vector<vector2>;
using sList_t = std::vector<std::string>;

template <typename T>
using sh_ptr = std::shared_ptr<T>;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

template<class... Args>
void print(Args&&... args)
{
    ((std::cout << args << '\t'), ...) << '\n';
    std::cout.flush();  // Ensures output is printed immediately
}

bool isDebug();

template<class... Args>
void error(Args... args)
{
    std::cout << "SCRIPT ERROR: ";
    ((std::cout << args << '\t'), ...) << '\n';
    std::cout.flush();  // Ensures output is printed immediately

    exit(69420);
}

float map_range(float s, float a1, float a2, float b1, float b2);
double map_range(double s, double a1, double a2, double b1, double b2);
int map_range(int s, int a1, int a2, int b1, int b2);

// Vector2 JSON Serialization
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

// Vector and Heading Functions
Heading getHeadingFromVector(const vector2& v);
Heading getHeadingFromVectors(const vector2& v1, const vector2& v2);
vector2 angleToVector2(float angle);
vector2 angleToVector2(Heading angle);
vectorList_t calculateBoundingBox(const vector2& min, const vector2& max);
std::pair<float, float> calculateDimensions(const std::vector<vector2>& points);
bool isPointInBounds(const vector2& point, const vectorList_t& polygon);
bool isRectangleInRectangle(const vectorList_t& rect1, const vectorList_t& rect2);
bool doLinesIntercept(vector2 v1, vector2 v2, vector2 v3, vector2 v4);
int getInterceptDist(vector2 v1, vector2 v2, vector2 v3, vector2 v4);
float getPerpendicularDistance(vector2 point, vector2 lineStart, vector2 lineEnd);
vectorList_t getMinMaxFromVectors(vector2 v, vector2 v2);

// random number generator
float random(float min, float max);
int random(int min, int max);
vector2 random(vector2 minVec, vector2 maxVec);

sList_t split(const std::string& str, const std::string& delimiter);

uint32_t jenkinsOneAtATimeHash(const std::string& key);
int mapHashToRange(uint32_t hash, int min, int max);

float pHeight(float num);
float pWidth(float num);

int getScaledPixelWidth(int num);
int getScaledPixelHeight(int num);
float getScaledPixelWidth(float num);
float getScaledPixelHeight(float num);

#endif //CSCI437_UTIL_H
