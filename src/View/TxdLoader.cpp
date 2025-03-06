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
// Created by xerxe on 3/4/2025.
//

#include "TxdLoader.h"
#include <SDL_image.h>

#include <utility>

int TxdLoader::initialize_SDL_process(SDL_Window *passed_window) {
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
    SDL_Surface* tempSurface = IMG_Load(txdPath.c_str());
    if (!tempSurface) {
        error("Failed to load image: %s\n", IMG_GetError());
        return 0;
    }
    texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    running = true;

    return 1;
}

void TxdLoader::update(float deltaMs) {}
bool TxdLoader::isDone() {return !running;}
void TxdLoader::postSuccess() {running = false;}
void TxdLoader::postFail() {running = false;}
void TxdLoader::postAbort() {running = false;}

void TxdLoader::setTxdPath(std::string txdPathNew) {
    TxdLoader::txdPath = std::move(txdPathNew);

    // Load the image
    SDL_Surface* tempSurface = IMG_Load(txdPath.c_str());
    if (!tempSurface) {
        error("Failed to load image: %s\n", IMG_GetError());
    }
    texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
}

std::string TxdLoader::GetTxdPath() {
    return txdPath;
}

void TxdLoader::render(SDL_Rect srcRect, SDL_Rect destRect, int angle) {
    SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, angle, nullptr, SDL_FLIP_NONE);
}

void TxdLoader::render(SDL_Rect srcRect, SDL_Rect destRect, Heading angle) {
    SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, angle.get(), nullptr, SDL_FLIP_NONE);
}

void TxdLoader::render(SDL_Rect srcRect, SDL_Rect destRect, Heading angle, SDL_RendererFlip flip) {
    SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, angle.get(), nullptr, flip);
}

void TxdLoader::render(SDL_Rect srcRect, SDL_Rect destRect, int angle, SDL_RendererFlip flip) {
    SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, angle, nullptr, flip);
}

void TxdLoader::render(SDL_Rect srcRect, SDL_Rect destRect, int angle, SDL_Point center, SDL_RendererFlip flip) {
    SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, angle, &center, flip);
}

void TxdLoader::render(SDL_Rect srcRect, SDL_Rect destRect, Heading angle, SDL_Point center, SDL_RendererFlip flip) {
    SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, angle.get(), &center, flip);
}


//void TxdLoader::setSrcRect(SDL_Rect srcRect) {
//    TxdLoader::srcRect = srcRect;
//}
//
//SDL_Rect TxdLoader::GetSrcRect() {
//    return srcRect;
//}
//
//void TxdLoader::setDestRect(SDL_Rect rect) {
//    destRect = rect;
//}
//
//SDL_Rect TxdLoader::GetDestRect() {
//    return destRect;
//}
//
//void TxdLoader::setAngle(int angleSet) {
//    angle = angleSet;
//}
//
//int TxdLoader::GetAngle() {
//    return angle;
//}
//
//void TxdLoader::setCenter(SDL_Point point) {
//    center = point;
//}
//
//void TxdLoader::setCenter(vector2 point) {
//    center = {static_cast<int>(point.x), static_cast<int>(point.y)};
//}
//
//SDL_Point TxdLoader::GetCenter() {
//    return center;
//}
//
//void TxdLoader::setFlip(SDL_RendererFlip flipDef) {
//    flip = flipDef;
//}
//
//SDL_RendererFlip TxdLoader::GetFlip() {
//    return flip;
//}








