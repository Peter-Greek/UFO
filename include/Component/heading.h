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

#ifndef CSCI437_HEADING_H
#define CSCI437_HEADING_H

#include <iostream>

// Heading Class
class BoundedInt {
private:
    int value;
    const int min;
    const int max;

    // Helper function to enforce bounds
    [[nodiscard]] int clamp(int v) const {
        if (v < min) return min;
        if (v > max) return max;
        return v;
    }

    // Helper function to wrap within bounds (for cyclic values like heading)
    [[nodiscard]] int wrap(int v) const {
        int range = max - min + 1;
        return ((v - min) % range + range) % range + min;
    }

public:
    BoundedInt(int v, int minVal, int maxVal) : min(minVal), max(maxVal) {
        value = wrap(v);
    }

    // Getters
    [[nodiscard]] int get() const { return value; }

    // Set value with clamping
    void set(int v) { value = wrap(v); }

    // Operators
    BoundedInt& operator=(int v) {
        set(v);
        return *this;
    }

    BoundedInt& operator+=(int v) {
        set(value + v);
        return *this;
    }

    BoundedInt& operator-=(int v) {
        set(value - v);
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const BoundedInt& bi) {
        os << bi.value;
        return os;
    }
};

// Specialization for heading (0-360)
class Heading : public BoundedInt {
public:
    explicit Heading(int v = 0) : BoundedInt(v, 0, 360) {}
};

#endif //CSCI437_HEADING_H
