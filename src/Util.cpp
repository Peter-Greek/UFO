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

//double map_range(double s, double a1, double a2, double b1, double b2) {
//    return b1 + (s - a1) * (b2 - b1) / (a2 - a1);
//}

float map_range(float s, float a1, float a2, float b1, float b2) {
    return b1 + (s - a1) * (b2 - b1) / (a2 - a1);
}
//
//int map_range(int s, int a1, int a2, int b1, int b2) {
//    return b1 + (s - a1) * (b2 - b1) / (a2 - a1);
//}


float random(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

int random(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

vector2 random(const vector2& vec1, const vector2& vec2) {
    float t = random(0.0f, 1.0f); // Random interpolation factor between 0 and 1
    return vec1 + (vec2 - vec1) * t;   // Linear interpolation
}