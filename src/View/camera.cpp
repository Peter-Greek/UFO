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
// Created by xerxe on 3/2/2025.
//

#include "camera.h"

int camera::initialize_SDL_process(SDL_Window *passed_window) {

    print("Text Initialize SDL Process: ", passed_window);
    window = passed_window;

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        error("SDL could not initialize!", SDL_GetError());
        return 0;
    }

    // Check window
    if( window == nullptr ) {
        error("Window could not be created!", SDL_GetError());
        return 0;
    }

    // Create renderer
    renderer = SDL_GetRenderer( window );
    if (renderer == nullptr) {
        renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
        if (renderer == nullptr) {
            error("Unable to create renderer! ", SDL_GetError());
            return 0;
        }
    }

    CAM_MIN = vector2(cameraX - SCREEN_WIDTH, cameraY - SCREEN_HEIGHT);
    CAM_MAX = vector2(cameraX + SCREEN_WIDTH, cameraY + SCREEN_HEIGHT);

    /*** Main Loop ***/
    AddEventHandler("SDL::OnUpdate", [this](float deltaMs) {
        // While application is running
        if (!gameRunning) return;
        TriggerEvent("Camera::OnUpdate", CAM_MIN, CAM_MAX);
    });

    gameRunning = true;

    return 1;
}

void camera::update(float deltaMs) {
    if (!gameRunning) return;


}

void camera::updateCamera(float x, float y) {
    cameraX = x;
    cameraY = y;
    CAM_MIN = vector2(cameraX - SCREEN_WIDTH, cameraY - SCREEN_HEIGHT);
    CAM_MAX = vector2(cameraX + SCREEN_WIDTH, cameraY + SCREEN_HEIGHT);
}
void camera::updateCamera(vector2 pos) {
    cameraX = pos.x;
    cameraY = pos.y;
    CAM_MIN = vector2(cameraX - SCREEN_WIDTH, cameraY - SCREEN_HEIGHT);
    CAM_MAX = vector2(cameraX + SCREEN_WIDTH, cameraY + SCREEN_HEIGHT);
}

bool camera::isPointInView(vector2 pos) const {
    if (pos.x < CAM_MIN.x || pos.x > CAM_MAX.x || pos.y < CAM_MIN.y || pos.y > CAM_MAX.y) {
        return false;
    }
    return true;
}

bool camera::isPointInView(float x, float y) const {
    if (x < CAM_MIN.x || x > CAM_MAX.x || y < CAM_MIN.y || y > CAM_MAX.y) {
        return false;
    }
    return true;
}

vector2 camera::worldToScreenCoords(vector2 pos) const {
    return {pos.x - cameraX, pos.y - cameraY};
}

vector2 camera::worldToScreenCoords(float x, float y) const {
    return {x - cameraX, y - cameraY};
}

vector2 camera::screenToWorldCoords(vector2 screenPos) const {
    return {screenPos.x + cameraX, screenPos.y + cameraY};
}










