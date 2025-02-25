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
// Created by xerxe on 2/11/2025.
//

#include "config.h"

// Define global variables
bool unlimitedFrames = false;
int debugMode = 0;

bool centerStraightened = true; // if the ball hits dead in the center then it straightens out kinda like a put in baseball
bool interestingBounces = true; // if the ball hits the paddle at an angle then it will bounce off at an angle
bool rampUpPaddleSpeed = true; // paddle speeds up if held down

int SCREEN_WIDTH = 1024;
int SCREEN_HEIGHT = 768;

int PADDLE_WIDTH = 15;
int PADDLE_HEIGHT = 100;
float PADDLE_SPEED = 1.0f;

int BALL_SIZE = 15;
float BALL_SPEED = 0.4f;

// Function to update settings dynamically
void updateSettings() {
    PADDLE_SPEED = unlimitedFrames ? 60.0f : 1.0f;
    BALL_SPEED = unlimitedFrames ? 80.0f : 0.4f;
}