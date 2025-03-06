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

#ifndef CSCI437_TXDLOADER_H
#define CSCI437_TXDLOADER_H

#include <utility>

#include "xProcess.h"
class TxdLoader : public xProcess {
private:
    std::string txdPath;
    bool running;

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Surface* surface;
public:
    TxdLoader(
            passFunc_t passFunc, std::string txdPath
    ): xProcess(true, passFunc), txdPath(std::move(txdPath)) {}

//    TxdLoader(
//            passFunc_t passFunc, std::string txdPath,
//            SDL_Rect srcRect, SDL_Rect destRect, int angle, SDL_Point center, SDL_RendererFlip flip
//    ): xProcess(true, passFunc), txdPath(std::move(txdPath)), srcRect(srcRect), destRect(destRect),
//    angle(angle), center(center), flip(flip) {}

    int initialize_SDL_process(SDL_Window* passed_window) override;

    void update(float deltaMs) override;

    bool isDone() override;

    void postSuccess() override;

    void postFail() override;

    void postAbort() override;

    void setTxdPath(std::string txdPathStr);
    std::string GetTxdPath();

//    void setSrcRect(SDL_Rect rect);
//    SDL_Rect GetSrcRect();
//
//    void setDestRect(SDL_Rect rect);
//    SDL_Rect GetDestRect();
//
//    void setAngle(int angle);
//    int GetAngle();
//
//    void setCenter(SDL_Point point);
//    void setCenter(vector2 point);
//    SDL_Point GetCenter();
//
//    void setFlip(SDL_RendererFlip flipDef);
//    SDL_RendererFlip GetFlip();
    void render(SDL_Rect srcRect, SDL_Rect destRect, int angle, SDL_Point center, SDL_RendererFlip flip);

    void render(SDL_Rect srcRect, SDL_Rect destRect, int angle, SDL_RendererFlip flip);

    void render(SDL_Rect srcRect, SDL_Rect destRect, Heading angle, SDL_RendererFlip flip);

    void render(SDL_Rect srcRect, SDL_Rect destRect, Heading angle);

    void render(SDL_Rect srcRect, SDL_Rect destRect, int angle);

    void render(SDL_Rect srcRect, SDL_Rect destRect, Heading angle, SDL_Point center, SDL_RendererFlip flip);
};


#endif //CSCI437_TXDLOADER_H
