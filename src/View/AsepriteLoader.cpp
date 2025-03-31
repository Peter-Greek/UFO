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

#include "AsepriteLoader.h"
#include <SDL_image.h>

int AsepriteLoader::initialize_SDL_process(SDL_Window *passed_window) {
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

    // Load the image
    SDL_Surface* tempSurface = IMG_Load(asepritePath.c_str());
    if (!tempSurface) {
        error("Failed to load image: %s\n", IMG_GetError());
        return 0;
    }
    texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    gameRunning = true;
    return 1;
}

void AsepriteLoader::update(float deltaMs) {

}

int AsepriteLoader::LoadJSON() {
    // Get the JSON data file
    std::ifstream file(asepriteJsonPath);
    if (!file.is_open()) {
        error("Failed to open JSON file: %s\n", asepriteJsonPath.c_str());
        return 0;
    }

    // Parse the JSON data
    asepriteData = json::parse(file);
    // Check if valid json
    if (asepriteData.empty()) {
        error("Failed to parse JSON file: %s\n", asepriteJsonPath.c_str());
        return 0;
    }

    return 1;
}

void AsepriteLoader::setTextureAlpha(int alpha) {
    SDL_SetTextureAlphaMod(texture, alpha);
}

void AsepriteLoader::resetTextureAlpha() {
    SDL_SetTextureAlphaMod(texture, 255);
}

void AsepriteLoader::renderFrame(SDL_Rect srcRect, SDL_Rect destRect, bool flip, int angle) {
    SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, angle, nullptr, flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE );
}

SDL_Rect AsepriteLoader::getFrame(const std::string& frameName) {
    SDL_Rect frame;
    frame.y = asepriteData["frames"][frameName]["frame"]["y"];
    frame.x = asepriteData["frames"][frameName]["frame"]["x"];
    frame.w = asepriteData["frames"][frameName]["frame"]["w"];
    frame.h = asepriteData["frames"][frameName]["frame"]["h"];
    return frame;
}
