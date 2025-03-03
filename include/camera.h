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

#ifndef CSCI437_CAMERA_H
#define CSCI437_CAMERA_H


#include "xProcess.h"
class camera : public xProcess {
private:
    bool gameRunning = false;

    // SDL Vars
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    // Camera Variables
    float cameraX = 0;
    float cameraY = 0;
    vector2 CAM_MIN;
    vector2 CAM_MAX;


public:
    explicit camera(const std::function<void(const std::string& eventName, const json& eventData)>& func) : xProcess(true, func) {}
    ~camera() override = default;

    int initialize_SDL_process(SDL_Window* window) override;
    void update(float deltaMs) override;
    bool isDone() override {return !gameRunning;};
    void postSuccess() override {};
    void postFail() override {};
    void postAbort() override {};

    void updateCamera(float x, float y);

    void updateCamera(vector2 pos);

    bool isPointInView(vector2 pos) const;
    bool isPointInView(float x, float y) const;

    vector2 worldToScreenCoords(vector2 pos) const;
    vector2 worldToScreenCoords(float x, float y) const;
};


#endif //CSCI437_CAMERA_H
