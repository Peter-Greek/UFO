#include "SettingsMenu.h"

int SettingsMenu::reloadFont() {
    fontSize = getScaledCoords({12, 12}).length();
    font = TTF_OpenFont("../resource/font/Arial.ttf", fontSize);
    if (font == nullptr) {
        error("Unable to open font! ", SDL_GetError());
        return 0;
    }

    font_extra = TTF_OpenFont("../resource/font/symbol.ttf", fontSize);
    if (font_extra == nullptr) {
        error("Unable to open font! ", SDL_GetError());
        return 0;
    }
    return 1;
}

int SettingsMenu::initialize_SDL_process(SDL_Window *passed_window) {
    window = passed_window;

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        error("SDL could not initialize!", SDL_GetError());
        return 0;
    }

    if( window == nullptr ) {
        error("Window could not be created!", SDL_GetError());
        return 0;
    }

    if( TTF_Init() < 0 ) {
        error("Unable to initialize TTF!", TTF_GetError());
        return 0;
    }

    renderer = SDL_GetRenderer( window );
    if (renderer == nullptr) {
        renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
        if (renderer == nullptr) {
            error("Unable to create renderer! ", SDL_GetError());
            return 0;
        }
    }

    if (!reloadFont()) {
        error("Unable to load font! ", SDL_GetError());
        return 0;
    }


    AddEventHandler("SDL::OnUpdate", [this](float deltaMs) {
        if (!running || renderer == nullptr) return;

        // Dynamically reload font if resolution changes
        int newFontSize = getScaledCoords({12, 12}).length();
        if (newFontSize != prevFontSize) {
            fontSize = newFontSize;

            if (font) TTF_CloseFont(font);
            font = TTF_OpenFont("../resource/font/Arial.ttf", fontSize);

            if (font_extra) TTF_CloseFont(font_extra);
            font_extra = TTF_OpenFont("../resource/font/symbol.ttf", fontSize);

            if (!font || !font_extra) {
                error("Unable to open font! ", SDL_GetError());
                return;
            }

            prevFontSize = fontSize;
        }

        int x_offset = getScaledPixelWidth(50);
        int y_offset = getScaledPixelHeight(50);
        const int spacing = getScaledPixelHeight(50);

        textures.clear();
        dropdownHitboxes.clear();

        if (menuTxd != nullptr && menuTxd->state() == xProcess::RUNNING) {
            SDL_Rect destRect = {
                    0, 0,
                    SCREEN_WIDTH,
                    SCREEN_HEIGHT
            };
            menuTxd->render(srcRect, destRect, 0, SDL_FLIP_NONE);
        }

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
                    label += openDropdowns.count(s.first) ? " [-]" : " [+]";

                    SDL_Surface* textSurface = TTF_RenderText_Solid(font, label.c_str(), {255, 255, 255});
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
                    SDL_Rect rect = {x_offset, y_offset, textSurface->w, textSurface->h};
                    dropdownHitboxes.emplace_back(rect, s.first, "");
                    textures.emplace_back(texture, rect);
                    SDL_FreeSurface(textSurface);

                    y_offset += getScaledPixelHeight(35);

                    if (openDropdowns.count(s.first)) {
                        for (const auto& [option, selected] : s.second) {
                            SDL_Rect bgRect = {
                                    getScaledPixelWidth(100),
                                    y_offset,
                                    getScaledPixelWidth(300),
                                    getScaledPixelHeight(30)
                            };
                            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
                            SDL_RenderFillRect(renderer, &bgRect);

                            SDL_Color color = selected ? SDL_Color{0, 0, 255, 255} : SDL_Color{255, 255, 255, 255};
                            SDL_Surface* optSurface = TTF_RenderText_Solid(font, option.c_str(), color);
                            SDL_Texture* optTexture = SDL_CreateTextureFromSurface(renderer, optSurface);
                            SDL_Rect optRect = {
                                    getScaledPixelWidth(110),
                                    y_offset + getScaledPixelHeight(5),
                                    optSurface->w,
                                    optSurface->h
                            };
                            dropdownHitboxes.emplace_back(optRect, s.first, option);
                            textures.emplace_back(optTexture, optRect);
                            SDL_FreeSurface(optSurface);

                            y_offset += getScaledPixelHeight(35);
                        }
                    }

                    y_offset -= getScaledPixelHeight(35);
                    y_offset += spacing;
                    return;
                }

                SDL_Surface* textSurface = nullptr;
                if constexpr (std::is_same_v<T, toggle_t>) {
                    const char* mark = s.second.second ? "X" : "";
                    label = s.first + ": [" + mark + "]";
                    textSurface = TTF_RenderText_Solid(font, label.c_str(), {255, 255, 255});
                } else if constexpr (std::is_same_v<T, slider_t>) {
                    label = s.name + ": " + std::to_string(s.setting.second);
                    textSurface = TTF_RenderText_Solid(font, label.c_str(), {255, 255, 255});
                }

                if (!textSurface) return;

                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
                SDL_Rect rect = {
                        getScaledPixelWidth(50),
                        y_offset,
                        textSurface->w,
                        textSurface->h
                };

                if constexpr (std::is_same_v<T, toggle_t>) {
                    dropdownHitboxes.emplace_back(rect, "toggle::" + s.first, "");
                }

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
        if (!running || renderer == nullptr) return;

        if (eventType == SDL_MOUSEBUTTONDOWN) {
            isMouseDown = true;
            return;
        }

        if (eventType == SDL_MOUSEBUTTONUP && isMouseDown) {
            isMouseDown = false;
            int x, y;
            SDL_GetMouseState(&x, &y);

            for (auto& [rect, settingName, optionName] : dropdownHitboxes) {
                if (x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h) {

                    if (settingName.rfind("toggle::", 0) == 0) {
                        std::string realName = settingName.substr(8); // remove prefix
                        for (auto& setting : settings) {
                            if (auto* toggle = std::get_if<toggle_t>(&setting); toggle && toggle->first == realName) {
                                toggle->second.second = !toggle->second.second;

                                print(realName + " toggled to " + std::to_string(toggle->second.second));
                                if (realName == "Fullscreen") {
                                    FULL_SCREEN_ENABLED = toggle->second.second;
                                    TriggerEvent("UFO::ChangeConfigValue", "FULL_SCREEN_ENABLED");
                                    if (toggle->second.second) {
                                        TriggerEvent("UFO::View::ResizeWindow", 0, 0);
                                    }else {
                                        // get active res setting and apply it
                                        for (auto& setting : settings) {
                                            if (auto* drop = std::get_if<dropdown_t>(&setting);
                                                drop && drop->first == "Windowed Resolution") {
                                                for (auto& [opt, selected] : drop->second) {
                                                    if (selected) {
                                                        auto [w, h] = resolution_map[opt];
                                                        TriggerEvent("UFO::View::ResizeWindow", w, h);
                                                        break;
                                                    }
                                                }
                                            }
                                        }
                                    }

                                } else if (realName == "VSync") {
                                    TriggerEvent("UFO::View::ToggleVSync", toggle->second.second);
                                } else if (realName == "Audio") {
                                    AUDIO_ENABLED = toggle->second.second;
                                    TriggerEvent("UFO::ChangeConfigValue", "AUDIO_ENABLED");
                                }


                                break;
                            }
                        }
                        return;
                    }

                    if (optionName.empty()) {
                        if (openDropdowns.count(settingName)) openDropdowns.erase(settingName);
                        else openDropdowns.insert(settingName);
                        return;
                    }

                    for (auto& setting : settings) {
                        if (auto* drop = std::get_if<dropdown_t>(&setting); drop && drop->first == settingName) {
                            for (auto& [opt, selected] : drop->second) {
                                selected = (opt == optionName);
                            }

                            if (settingName == "Windowed Resolution") {
                                auto [w, h] = resolution_map[optionName];
                                SCREEN_RESOLUTION = {w, h};
                                TriggerEvent("UFO::ChangeConfigValue", "SCREEN_RESOLUTION");
                                TriggerEvent("UFO::View::ResizeWindow", w, h);

                                // set fullscreen toggle to false
                                for (auto& setting : settings) {
                                    if (auto* toggle = std::get_if<toggle_t>(&setting); toggle && toggle->first == "Fullscreen") {
                                        toggle->second.second = false;
                                        FULL_SCREEN_ENABLED = false;
                                        TriggerEvent("UFO::ChangeConfigValue", "FULL_SCREEN_ENABLED");
                                        break;
                                    }
                                }

                            }

                            break;
                        }
                    }

                    break;
                }
            }
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
    // Not used currently

    if (!running) return;

}
