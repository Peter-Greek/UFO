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

#ifndef CSCI437_SETTINGSMENU_H
#define CSCI437_SETTINGSMENU_H

#include "xProcess.h"
#include "GameStorage.h"
#include "TxdLoader.h"
#include <SDL_ttf.h>
#include <list>
#include <string>
#include <variant>
#include <set>
#include <optional>

class SettingsMenu : public xProcess {

private:
    using setting_resolution_t = std::pair<int, int>;
    using resolution_map_t = std::map<std::string, setting_resolution_t>;


    using setting_t  = std::pair<std::string, bool>;
    using setting_int_t = std::pair<std::string, int>;
    using setting_f_t = std::pair<std::string, int>;
    using settings_t = std::list<setting_t>;

    using dropdown_t = std::pair<std::string, settings_t>;
    using toggle_t  = std::pair<std::string, setting_t>;
    struct slider_t {
        std::string name;
        setting_f_t setting;
        float min;
        float max;
        float step;
        bool includeMin;
        bool includeMax;
    };

    struct sliderHitbox_t {
        SDL_Rect barRect;
        std::string name;
    };

    using setting_variant_t = std::variant<dropdown_t, toggle_t, slider_t>;




    // define a map of resolution settings


    /*
     *
     *  int SCREEN_WIDTH = 1920; // 1024 | 1920 | 2400 | 3840
     *  int SCREEN_HEIGHT = 1080; // 768 | 1080 | 1600 | 2160
     */
    resolution_map_t resolution_map = {
            {"3840x2160", {3840, 2160}},
            {"2400x1600", {2400, 1600}},
            {"1920x1080", {1920, 1080}},
            {"1024x768", {1024, 768}}
    };

    std::vector<std::tuple<SDL_Rect, std::string, std::string>> dropdownHitboxes;
    std::set<std::string> openDropdowns;
    std::vector<sliderHitbox_t> sliderHitboxes;
    std::vector<sliderHitbox_t> handlerHitboxes;
    std::optional<std::string> activeSlider;

    static bool isResolutionActive(const std::string& resolution) {
        int height = SCREEN_HEIGHT;
        int width = SCREEN_WIDTH;
        sList_t args = split(resolution, "x");
        return std::stoi(args[0]) == width && std::stoi(args[1]) == height;
    }

    std::pair <int, int> getScreenResolution() {
        // get current display size and apply it as the args
        int displayIndex = SDL_GetWindowDisplayIndex(window);
        if (displayIndex < 0) {
            print("SDL_GetWindowDisplayIndex failed2: %s", SDL_GetError());
            return {0, 0};
        } else {
            SDL_DisplayMode mode;
            if (SDL_GetCurrentDisplayMode(displayIndex, &mode) == 0) {
                int displayWidth = mode.w;
                int displayHeight = mode.h;
                print("Display size2: %dx%d", displayWidth, displayHeight);
                return {displayWidth, displayHeight};
            } else {
                print("SDL_GetCurrentDisplayMode failed2: %s", SDL_GetError());
                return {0, 0};
            }
        }
    }


    std::list<setting_variant_t> settings = {
            setting_variant_t{std::in_place_type<dropdown_t>, "Windowed Resolution", settings_t{
                    {"3840x2160", isResolutionActive("3840x2160")},
                    {"2400x1600", isResolutionActive("2400x1600")},
                    {"1920x1080", isResolutionActive("1920x1080")},
                    {"1024x768", isResolutionActive("1024x768")}
            }},
            //toggle_t for fullscreen
            setting_variant_t{std::in_place_type<toggle_t>, "Fullscreen", setting_t{"fs", FULL_SCREEN_ENABLED}},
            setting_variant_t{std::in_place_type<toggle_t>, "VSync", setting_t{"vsync", false}},
            setting_variant_t{std::in_place_type<toggle_t>, "Audio", setting_t{"sound", AUDIO_ENABLED}},

            setting_variant_t{std::in_place_type<slider_t>, slider_t{"Music Volume", {"MusicVol", VOLUME_MUSIC}, 0.0f, 100.0f, 1.0f, true, true}},
            setting_variant_t{std::in_place_type<slider_t>, slider_t{"Effects Volume", {"SFXVolume", VOLUME_SFX}, 0.0f, 100.0f, 1.0f, true, true}},


            setting_variant_t{std::in_place_type<dropdown_t>, "Language", settings_t{
                    {"English", true},
                    {"Spanish", false}
            }},
    };


    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    TTF_Font* font_extra;

    json saveData;
    bool running = false;
    int fontSize = 24;
    int prevFontSize = -1;

    bool isMouseDown = false;

    std::list<std::pair<SDL_Texture*, SDL_Rect>> textures;

    sh_ptr<GameStorage> gS;
    sh_ptr<TxdLoader> menuTxd;
    SDL_Rect srcRect = {1, 1, 1024, 768}; // load the entire texture, 1 pixel in since there is white line


public:
    SettingsMenu(passFunc_t p1, sh_ptr<GameStorage> gS_p, sh_ptr<TxdLoader> mTxd)
        : xProcess(true, p1), gS(gS_p), menuTxd(mTxd)  {}
    ~SettingsMenu() override = default;

    int initialize_SDL_process(SDL_Window* passed_window) override;
    void update(float deltaMs) override;
    bool isDone() override { return !running; };
    void postSuccess() override {};
    void postFail() override {};
    void postAbort() override {};

    int reloadFont();

    void updateSliderValue(int x, int y);
};


#endif //CSCI437_SETTINGSMENU_H
