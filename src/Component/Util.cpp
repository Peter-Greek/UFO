// The MIT License (MIT)
//
// Copyright (c) 2025 Peter Greek
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Proper permission is grated by the copyright holder.
//
// Credit is attributed to the copyright holder in some form in the product.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

//
// Created by xerxe on 2/11/2025.
//

#include "Util.h"

double map_range(double s, double a1, double a2, double b1, double b2) {
    return b1 + (s - a1) * (b2 - b1) / (a2 - a1);
}

float map_range(float s, float a1, float a2, float b1, float b2) {
    return b1 + (s - a1) * (b2 - b1) / (a2 - a1);
}

int map_range(int s, int a1, int a2, int b1, int b2) {
    return b1 + (s - a1) * (b2 - b1) / (a2 - a1);
}

vector2 angleToVector2(Heading angle) {
    float a = static_cast<float>(angle.get());
    float radians = a * (M_PI / 180.0f);

    // Calculate x and y components
    float x = std::cos(radians);
    float y = std::sin(radians);

    // Return normalized vector2
    return vector2(x, y).normalize();
}

vector2 angleToVector2(float angle) {
    // Convert degrees to radians
    float radians = angle * (M_PI / 180.0f);

    // Calculate x and y components
    float x = std::cos(radians);
    float y = std::sin(radians);

    // Return normalized vector2
    return vector2(x, y).normalize();
}

Heading getHeadingFromVector(const vector2& v) {
    int j = std::atan2(v.y, v.x) * 180.0 / M_PI;
    auto h = Heading(j);
    return h;
}

Heading getHeadingFromVectors(const vector2& v1, const vector2& v2) {
    // Compute delta x and delta y
    float dX = v2.x - v1.x;
    float dY = v2.y - v1.y;

    // Calculate heading using atan2 (returns radians)
    float hd = std::atan2(dY, dX) * (180.0f / M_PI);
    auto h = Heading(hd);
    return h;
}

vectorList_t calculateBoundingBox(const vector2& min, const vector2& max) {
    return {
        {min.x, min.y}, // Bottom left
        {max.x, min.y}, // Bottom right
        {max.x, max.y}, // Top right
        {min.x, max.y}  // Top left
    };
}

std::pair<float, float> calculateDimensions(const std::vector<vector2>& points) {
    vector2 p1 = points[0], p2 = points[1], p3 = points[2], p4 = points[3];
    float halfWidth = ((p2.x - p1.x) + (p3.x - p4.x)) / 4.0f;
    float halfHeight = ((p3.y - p1.y) + (p4.y - p2.y)) / 4.0f;
    float width = halfWidth * 2.0f;
    float height = halfHeight * 2.0f;
    return {width, height};
}

bool isPointInBounds(const vector2& point, const vectorList_t& polygon) {
    bool oddNodes = false;
    size_t j = polygon.size() - 1;
    for (size_t i = 0; i < polygon.size(); i++) {
        if ((polygon[i].y < point.y && polygon[j].y >= point.y) ||
            (polygon[j].y < point.y && polygon[i].y >= point.y)) {

            if (polygon[i].x + ((point.y - polygon[i].y) / (polygon[j].y - polygon[i].y)) *
                               (polygon[j].x - polygon[i].x) < point.x) {
                oddNodes = !oddNodes;
            }
        }
        j = i; // Move j to the previous index
    }
    return oddNodes;
}

// Singleton random engine
std::random_device rd;
std::mt19937 gen(rd());

float random(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

int random(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

vector2 random(const vector2& vec1, const vector2& vec2) {
    float t = random(0.0f, 1.0f);
    return vec1 + (vec2 - vec1) * t;
}