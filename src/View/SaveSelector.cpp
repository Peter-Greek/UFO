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
// Created by xerxe on 4/8/2025.
//

#include "SaveSelector.h"

void reloadTextures();

int SaveSelector::initialize_SDL_process(SDL_Window *passed_window) {
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

    // init TTF
    if( TTF_Init() < 0 ) {
        error("Unable to initialize TTF!", TTF_GetError());
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

    fontSize = (int) getScaledCoords({12, 12}).length(); // base is 24

    // Load font
    font = TTF_OpenFont("../resource/Arial.ttf", fontSize);
    if (font == nullptr) {
        error("Unable to open font! ", SDL_GetError());
        return 0;
    }

    AddEventHandler("SDL::OnUpdate", [this](float deltaMs) {
        if (!running) return;
        if (renderer == nullptr) { return; }

        if (menuTxd != nullptr && menuTxd->state() == xProcess::RUNNING) {
            menuTxd->render(srcRect, destRect, 0, SDL_FLIP_NONE);
        }

        for (int i = 0; i < max_display; i++) {
            int pad = (padding * 2) + ((padding + templateBox.w) * i);
            drawRect(templateBox.x + pad, templateBox.y, templateBox.w, templateBox.h);
        }

        if (pagination > 0) {
            drawRect(leftArrow.x, leftArrow.y, leftArrow.w, leftArrow.h);
        }

        if (pagination + max_display <= saveData.size()) {
            drawRect(rightArrow.x, rightArrow.y, rightArrow.w, rightArrow.h);
        }

        for (auto& texture : textures) {
            SDL_SetTextureColorMod(texture.first, 255, 255, 255);
            SDL_RenderCopy(renderer, texture.first, nullptr, &texture.second);
        }
    });

    AddEventHandler("SDL::OnPollEvent", [this](int eventType, int key) {
        if (!running) return;
        if (renderer == nullptr) { return; }

        if (eventType == SDL_KEYDOWN) {
            if (key == SDLK_ESCAPE) {
                TriggerEvent("UFO::SaveSelector::Close");
                running = false;
                return;
            }
        }

        if (eventType == SDL_MOUSEBUTTONDOWN) {
            isMouseDown = true;
            return;
        }

        if (eventType == SDL_MOUSEBUTTONUP && isMouseDown) {
            isMouseDown = false;
            int x, y;
            SDL_GetMouseState(&x, &y);

            // if click within a save box then trigger UFO::SaveSelector::Select
            for (int i = 0; i < max_display; i++) {
                int pad = (padding * 2) + ((padding + templateBox.w) * i);
                if (x >= templateBox.x + pad && x <= templateBox.x + pad + templateBox.w &&
                    y >= templateBox.y && y <= templateBox.y + templateBox.h) {
                    if (i + pagination >= saveData.size()) {
                        // if we are at the end of the list then create a new save
                        TriggerEvent("UFO::SaveSelector::Create");
                        return;
                    }else {
                        TriggerEvent("UFO::SaveSelector::Select", i + pagination);
                        return;
                    }
                }
            }

            print("Checking for arrows");
            if (x >= leftArrow.x && x <= leftArrow.x + leftArrow.w &&
                y >= leftArrow.y && y <= leftArrow.y + leftArrow.h) {
                print("Clicking left arrow");
                // if we are at the start of the list then go back
                if (pagination > 0) {
                    print("Going back");
                    pagination -= max_display;
                    reloadTextures();
                }
                return;
            }else if (x >= rightArrow.x && x <= rightArrow.x + rightArrow.w &&
                      y >= rightArrow.y && y <= rightArrow.y + rightArrow.h) {
                print("Clicking right arrow");
                // if we are at the end of the list then go forward
                if (pagination + max_display <= saveData.size()) {
                    print("Going forward");
                    pagination += max_display;
                    reloadTextures();
                }
                return;
            }

        }
    });

    running = true;
    reloadTextures();

    return 1;
}

void SaveSelector::reloadTextures() {
    textures.clear();

    auto pH = [this](float num) -> float {
        return templateBox.h * (num / 100);
    };

    auto pW = [this](float num) -> float {
        return templateBox.w * (num / 100);
    };

    auto tot_saves = saveData.size();
    for (int i = 0; i < max_display; i++) {
        if (i + pagination >= tot_saves) break;

        auto save = saveData[i + pagination];
        auto name = save["name"].get<std::string>();
        auto atCount = save["ATCount"].get<int>();
        auto time = save["time"].get<float>();
        auto date = save["date"].get<int>();

        int hours = static_cast<int>(time / 3600000);
        int minutes = static_cast<int>((time - (hours * 3600000)) / 60000);
        int seconds = static_cast<int>((time - (hours * 3600000) - (minutes * 60000)) / 1000);

        auto t = static_cast<std::time_t>(date);
        std::tm tm = *std::localtime(&t);
        std::string dateStr =   std::to_string(tm.tm_mon + 1) + "-" +
                                std::to_string(tm.tm_mday)+ "-" +
                                std::to_string(tm.tm_year + 1900);
//                                + " " +
//                                std::to_string(tm.tm_hour) + ":" +
//                                std::to_string(tm.tm_min) + ":" +
//                                std::to_string(tm.tm_sec);

        for (int j = 0; j < 4; j++) {
            std::string text;
            switch (j) {
                case 0:
                    text = name;
                    break;
                case 1:
                    text = "AT Count: " + std::to_string(atCount);
                    break;
                case 2:
                    // convert time in milliseconds to hours, minutes, seconds
                    text = "Time: " + std::to_string(hours) + ":" + std::to_string(minutes) + ":" + std::to_string(seconds);
                    break;
                case 3:
                    text = "Date: " + dateStr;
                    break;
            }


            SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), {255, 255, 255});
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_FreeSurface(textSurface);

            int textWidth, textHeight;
            SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);

            int pad = (padding * 2) + ((padding + templateBox.w) * i);

            SDL_Rect rec = {
                    static_cast<int>(templateBox.x + pad + pW(50) - textWidth / 2),
                    static_cast<int>(templateBox.y + pH(10) +  pH(2 * (j + 1)) + (textHeight * j)),
                    textWidth,
                    textHeight
            };

            std::pair <SDL_Texture*, SDL_Rect> textureList = {textTexture, rec};
            textures.push_back(textureList);
        }
    }

    // get remaining empty, so if we have 5 that means 1 empty of % 3
    for (int i = 0; i < max_display; i++) {
        print("Checking for empty slots", i+pagination, "of", tot_saves);
        if (i + pagination < tot_saves) { continue; }

        SDL_Surface* textSurface = TTF_RenderText_Solid(font, "New Save", {255, 255, 255});
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface);

        int textWidth, textHeight;
        SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);

        int pad = (padding * 2) + ((padding + templateBox.w) * i);

        SDL_Rect rec = {
                static_cast<int>(templateBox.x + pad + pW(50) - textWidth / 2),
                static_cast<int>(templateBox.y + pH(10) +  pH(2) + (textHeight)),
                textWidth,
                textHeight
        };

        std::pair <SDL_Texture*, SDL_Rect> textureList = {textTexture, rec};
        textures.push_back(textureList);
    }
}

void SaveSelector::update(float deltaMs) {
    if (!running) return;
}

void SaveSelector::drawRect(int x, int y, int w, int h) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderDrawRect(renderer, &rect);
}