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
int curRoomIndex = -1;
int targetFPS = 90;

int BASE_SCREEN_WIDTH = 1024; // DO NOT CHANGE THESE VALUES (WHAT THE GAME IS BASED ON IN INIT DEVELOPMENT)
int BASE_SCREEN_HEIGHT = 768; // DO NOT CHANGE THESE VALUES (WHAT THE GAME IS BASED ON IN INIT DEVELOPMENT)

int SCREEN_WIDTH = 1920; // 1024 | 1920 | 2400 | 3840
int SCREEN_HEIGHT = 1080; // 768 | 1080 | 1600 | 2160

float VOLUME_MUSIC = 50.0f;
float VOLUME_SFX = 50.0f;

vector2 WORLD_MIN = vector2(5.0f * -SCREEN_WIDTH, 5.0f * -SCREEN_HEIGHT);
vector2 WORLD_MAX = vector2(5.0f * SCREEN_WIDTH, 5.0f * SCREEN_HEIGHT);
float WORLD_MIN_X = WORLD_MIN.x;
float WORLD_MIN_Y = WORLD_MIN.y;
float WORLD_MAX_X = WORLD_MAX.x;
float WORLD_MAX_Y = WORLD_MAX.y;

// Function to update settings dynamically
void updateSettings() {

}