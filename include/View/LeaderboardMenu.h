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
// Created by xerxe on 4/21/2025.
//

#ifndef CSCI437_LEADERBOARDMENU_H
#define CSCI437_LEADERBOARDMENU_H

#include "xProcess.h"
#include "TxdLoader.h"
#include <SDL_ttf.h>

class LeaderboardMenu : public xProcess {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;

    json saveData;
    bool running = false;
    int fontSize = 24;

    int max_display = 3;
    int pagination = 0;

    bool isMouseDown = false;

    SDL_Rect templateBox = {
            0,
            static_cast<int>(SCREEN_HEIGHT/6),
            SCREEN_WIDTH / 4,
            static_cast<int>(SCREEN_HEIGHT / 6 * 4)
    };
    int padding = ((SCREEN_WIDTH / 4) / 6);

    std::list<std::pair<SDL_Texture*, SDL_Rect>> textures;

    SDL_Rect leftArrow = {
            templateBox.x + padding - templateBox.w/8,
            templateBox.y + templateBox.h/2 - templateBox.h/16,
            templateBox.w/8, templateBox.h/8
    };

    SDL_Rect rightArrow = {
            templateBox.x + (padding * 2) + ((padding + templateBox.w) * 3),
            templateBox.y + templateBox.h/2 - templateBox.h/16,
            templateBox.w/8, templateBox.h/8
    };

    using L_t = std::list<
            std::pair<
                    std::string, // Name of the leaderboard type
                    std::list<
                        std::pair<
                                std::string, // Name of data to sort by
                                int // ascending = 1 or descending = 0 or difference = 2
                        >
                    > // list of things to categorize this leaderboard by
            >
    >;

    L_t leaderboardTemplate = {
            {"Quickest", {
                {"time", 1},
            }},
            {"TotalAT", {
                {"TotalAT", 0},
            }},
            {"Attempts", {
                {"LoadCount", 1},
            }},
            {"Time Played", {
                {"time", 0},
            }},
            {"Efficiency", { // difference between total AT collected and the amount used decending
                {"TotalAT", 2},
                {"ATCount", 0},
            }},
    };

    sh_ptr<TxdLoader> menuTxd;
    SDL_Rect srcRect = {1, 1, 1024, 768}; // load the entire texture, 1 pixel in since there is white line
    SDL_Rect destRect = {
            static_cast<int>(0),
            static_cast<int>(0),
            static_cast<int>(SCREEN_WIDTH),
            static_cast<int>(SCREEN_HEIGHT) // down scale the texture
    };
public:
    LeaderboardMenu(passFunc_t p1, json saveData, sh_ptr<TxdLoader> menuTxd_p): xProcess(true, p1), saveData(std::move(saveData)), menuTxd(menuTxd_p) {}
    ~LeaderboardMenu() override = default;

    int initialize_SDL_process(SDL_Window* passed_window) override;
    void update(float deltaMs) override;
    bool isDone() override { return !running; };
    void postSuccess() override {};
    void postFail() override {};
    void postAbort() override {};

    void drawRect(int x, int y, int w, int h);
    void reloadTextures();
};


#endif //CSCI437_LEADERBOARDMENU_H
