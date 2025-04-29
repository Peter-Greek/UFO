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

#ifndef CSCI437_CONFIG_H
#define CSCI437_CONFIG_H

#include <cmath>
#include "Util.h"



// Declare global variables with extern
extern bool unlimitedFrames;
extern int debugMode;
extern int curRoomIndex;
extern int targetFPS;
extern int seed; // Random seed for world generation

extern int BASE_SCREEN_WIDTH;
extern int BASE_SCREEN_HEIGHT;

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

extern bool AUDIO_ENABLED;
extern float VOLUME_MUSIC;
extern float VOLUME_SFX;

extern bool FULL_SCREEN_ENABLED;
extern std::pair<int, int> SCREEN_RESOLUTION;


extern vector2 WORLD_MIN;
extern vector2 WORLD_MAX;
extern float WORLD_MIN_X;
extern float WORLD_MIN_Y;
extern float WORLD_MAX_X;
extern float WORLD_MAX_Y;

// Function to update settings dynamically
void updateSettings();

#endif // CSCI437_CONFIG_H