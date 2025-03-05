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
// Created by xerxe on 3/3/2025.
//

#ifndef CSCI437_ASEPRITELOADER_H
#define CSCI437_ASEPRITELOADER_H

#include "xProcess.h"
#include <fstream>


class Animation {
public:
    Animation(nlohmann::json& jsonData, const std::string& animPrefix): currentFrame(0), elapsedTime(0), jsonData(jsonData) {
        for (int i = 0; i < 8; ++i) { // Assumes 8 frames per animation
            std::string frameName = animPrefix + " " + std::to_string(i) + ".ase";
            frames.push_back(getFrame(frameName));
            durations.push_back(jsonData["frames"][frameName]["duration"].get<int>());
        }
    }

    SDL_Rect getFrame(const std::string& frameName) {
        SDL_Rect frame;
        frame.x = jsonData["frames"][frameName]["frame"]["x"].get<int>();
        frame.y = jsonData["frames"][frameName]["frame"]["y"].get<int>();
        frame.w = jsonData["frames"][frameName]["frame"]["w"].get<int>();
        frame.h = jsonData["frames"][frameName]["frame"]["h"].get<int>();
        return frame;
    }

    SDL_Rect getCurrentFrame(float deltaTime) {
        elapsedTime += deltaTime;
        if (elapsedTime > durations[currentFrame]) {
            elapsedTime = 0;
            currentFrame = (currentFrame + 1) % frames.size();
        }
        return frames[currentFrame];
    }
private:
    std::vector<SDL_Rect> frames;
    std::vector<int> durations;
    int currentFrame;
    float elapsedTime;
    nlohmann::json& jsonData;
};

class AsepriteLoader : public xProcess {
private:
    // SDL Vars
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;


    // file path to png and json
    json asepriteData;
    std::string asepritePath;
    std::string asepriteJsonPath;

    bool gameRunning = false;
public:
    AsepriteLoader(passFunc_t passFunc, std::string imgPath, std::string jsonPath) : xProcess(true, passFunc) {
        asepritePath = imgPath;
        asepriteJsonPath = jsonPath;
    }
    ~AsepriteLoader() override = default;

    int initialize_SDL_process(SDL_Window* window) override;
    void update(float deltaMs) override;
    bool isDone() override {return !gameRunning;};
    void postSuccess() override {};
    void postFail() override {};
    void postAbort() override {};

    SDL_Rect getFrame(const std::string &frameName);
    json& getJSONData() { return asepriteData; }

    void renderFrame(SDL_Rect srcRect, SDL_Rect destRect, bool flip, int angle);

    void setTextureAlpha(int alpha);

    void resetTextureAlpha();
};


#endif //CSCI437_ASEPRITELOADER_H
