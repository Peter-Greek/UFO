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
// Created by xerxe on 4/14/2025.
//

#include "SettingsMenu.h"

int SettingsMenu::initialize_SDL_process(SDL_Window *passed_window) {
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

    // Load font
    font = TTF_OpenFont("../resource/Arial.ttf", fontSize);
    if (font == nullptr) {
        error("Unable to open font! ", SDL_GetError());
        return 0;
    }

    AddEventHandler("SDL::OnUpdate", [this](float deltaMs) {
        if (!running) return;
        if (renderer == nullptr) { return; }

        int y_offset = 50; // starting Y position for rendering
        const int spacing = 50;

        for (const auto& setting : settings) {
            std::visit([&](auto&& s) {
                using T = std::decay_t<decltype(s)>;
                std::string label;

                if constexpr (std::is_same_v<T, dropdown_t>) {
                    label = s.first + ": ";
                    for (const auto& [option, selected] : s.second) {
                        if (selected) {
                            label += option;
                            break;
                        }
                    }
                } else if constexpr (std::is_same_v<T, toggle_t>) {
                    label = s.first + ": " + (s.second.second ? "On" : "Off");
                } else if constexpr (std::is_same_v<T, slider_t>) {
                    label = s.name + ": " + std::to_string(s.setting.second);
                }

                // Create surface and texture
                SDL_Surface* textSurface = TTF_RenderText_Solid(font, label.c_str(), {255, 255, 255});
                if (!textSurface) return;

                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
                SDL_Rect rect = {50, y_offset, textSurface->w, textSurface->h};
                textures.emplace_back(texture, rect);
                SDL_FreeSurface(textSurface);

                y_offset += spacing;
            }, setting);
        }

        for (auto& texture : textures) {
            SDL_SetTextureColorMod(texture.first, 255, 255, 255);
            SDL_RenderCopy(renderer, texture.first, nullptr, &texture.second);
        }
    });

    AddEventHandler("SDL::OnPollEvent", [this](int eventType, int key) {
        if (!running) return;
        if (renderer == nullptr) { return; }

        if (eventType == SDL_MOUSEBUTTONDOWN) {
            isMouseDown = true;
            return;
        }

        if (eventType == SDL_MOUSEBUTTONUP && isMouseDown) {
            isMouseDown = false;
            int x, y;
            SDL_GetMouseState(&x, &y);

        }

        if (eventType == SDL_KEYDOWN) {
            if (key == SDLK_ESCAPE) {
                TriggerEvent("UFO::SetSettingsState", false);
                running = false;
                return;
            }
        }
    });


    running = true;
    return 1;
}

void SettingsMenu::update(float deltaMs) {

}
