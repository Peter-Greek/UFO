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

bool isDebug() {
    return debugMode == 1;
}

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

vector2 projectRectangle(const vectorList_t& rect, const vector2& axis) {
    float minProj = rect[0].dot(axis);
    float maxProj = rect[0].dot(axis);
    
    for (const auto& point : rect) {
        float projection = point.dot(axis);
        if (projection < minProj) 
            minProj = projection;
        if (projection > maxProj) 
            maxProj = projection;
    }

    return vector2(minProj, maxProj);
}

bool isRectangleInRectangle(const vectorList_t& rect1, const vectorList_t& rect2) {
    vectorList_t edges;

    for (int i = 0; i < 4; i++) {
        edges.push_back((rect1[(i + 1) % 4] - rect1[i]).perpendicular().normalize());
        edges.push_back((rect2[(i + 1) % 4] - rect2[i]).perpendicular().normalize());
    }

    for (const auto& axis : edges) {
        vector2 proj1 = projectRectangle(rect1, axis);
        vector2 proj2 = projectRectangle(rect2, axis);

        if (proj1.getY() < proj2.getX() || proj2.getY() < proj1.getX()) {
            return false;
        }
    }

    return true;
}

bool doLinesIntercept(vector2 v1, vector2 v2, vector2 v3, vector2 v4) {
    float d = (v4.y - v3.y) * (v2.x - v1.x) - (v4.x - v3.x) * (v2.y - v1.y);
    float n_a = (v4.x - v3.x) * (v1.y - v3.y) - (v4.y - v3.y) * (v1.x - v3.x);
    float n_b = (v2.x - v1.x) * (v1.y - v3.y) - (v2.y - v1.y) * (v1.x - v3.x);

    if (d == 0) {
        return false;
    }

    float ua = n_a / d;
    float ub = n_b / d;

    return ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1;
}

int getInterceptDist(vector2 v1, vector2 v2, vector2 v3, vector2 v4) {
    float d = (v4.y - v3.y) * (v2.x - v1.x) - (v4.x - v3.x) * (v2.y - v1.y);
    float n_a = (v4.x - v3.x) * (v1.y - v3.y) - (v4.y - v3.y) * (v1.x - v3.x);
    float n_b = (v2.x - v1.x) * (v1.y - v3.y) - (v2.y - v1.y) * (v1.x - v3.x);

    if (d == 0) {
        return -1;
    }

    float ua = n_a / d;
    float ub = n_b / d;

    if (ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1) {
        return ua * (v2 - v1).length();
    }

    return -1;
}

float getPerpendicularDistance(vector2 point, vector2 lineStart, vector2 lineEnd) {
    float numerator = std::abs((lineEnd.x - lineStart.x) * (lineStart.y - point.y) -
                               (lineStart.x - point.x) * (lineEnd.y - lineStart.y));

    float denominator = (lineEnd - lineStart).length(); // Distance between start and end

    return (denominator == 0.0f) ? 0.0f : numerator / denominator;
}

vectorList_t getMinMaxFromVectors(vector2 v, vector2 v2) {
    return {
        {std::min(v.x, v2.x), std::min(v.y, v2.y)},
        {std::max(v.x, v2.x), std::max(v.y, v2.y)}
    };
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

sList_t split(const std::string& str, const std::string& delimiter) {
    sList_t result;
    size_t start = 0;
    size_t end;

    while ((end = str.find(delimiter, start)) != std::string::npos) {
        result.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
    }

    result.push_back(str.substr(start)); // Add the last piece
    return result;
}

uint32_t jenkinsOneAtATimeHash(const std::string& key) {
    uint32_t hash = 0;

    for (char c : key) {
        hash += static_cast<uint8_t>(c);
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

int mapHashToRange(uint32_t hash, int min, int max) {
    return (hash % (max - min + 1)) + min;
}

float pHeight(float num) {
    return SCREEN_HEIGHT * (num / 100);
}

float pWidth(float num) {
    return SCREEN_WIDTH * (num / 100);
}

int getScaledPixelWidth(int num) {
    return static_cast<int>(num * (SCREEN_WIDTH / BASE_SCREEN_WIDTH));
}

int getScaledPixelHeight(int num) {
    return static_cast<int>(num * (SCREEN_HEIGHT / BASE_SCREEN_HEIGHT));
}

float getScaledPixelWidth(float num) {
    return num * (SCREEN_WIDTH / BASE_SCREEN_WIDTH);
}

float getScaledPixelHeight(float num) {
    return num * (SCREEN_HEIGHT / BASE_SCREEN_HEIGHT);
}