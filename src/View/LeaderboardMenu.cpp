#include "LeaderboardMenu.h"
#include <algorithm>
#include <sstream>

int LeaderboardMenu::initialize_SDL_process(SDL_Window *passed_window) {
    window = passed_window;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        error("SDL could not initialize!", SDL_GetError());
        return 0;
    }

    if (window == nullptr) {
        error("Window could not be created!", SDL_GetError());
        return 0;
    }

    if (TTF_Init() < 0) {
        error("Unable to initialize TTF!", TTF_GetError());
        return 0;
    }

    renderer = SDL_GetRenderer(window);
    if (!renderer) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            error("Unable to create renderer!", SDL_GetError());
            return 0;
        }
    }

    fontSize = (int)getScaledCoords({12, 12}).length();
    font = TTF_OpenFont("../resource/font/Arial.ttf", fontSize);
    if (!font) {
        error("Unable to open font!", SDL_GetError());
        return 0;
    }

    AddEventHandler("SDL::OnUpdate", [this](float deltaMs) {
        if (!running) return;
        if (!renderer) return;

        if (menuTxd && menuTxd->state() == xProcess::RUNNING) {
            menuTxd->render(srcRect, destRect, 0, SDL_FLIP_NONE);
        }

        for (int i = 0; i < max_display; i++) {
            int pad = (padding * 2) + ((padding + templateBox.w) * i);
            drawRect(templateBox.x + pad, templateBox.y, templateBox.w, templateBox.h);
        }

        if (pagination > 0) {
            drawRect(leftArrow.x, leftArrow.y, leftArrow.w, leftArrow.h);
        }

        if (pagination + max_display < leaderboardTemplate.size()) {
            drawRect(rightArrow.x, rightArrow.y, rightArrow.w, rightArrow.h);
        }

        for (auto& [tex, rect] : textures) {
            SDL_SetTextureColorMod(tex, 255, 255, 255);
            SDL_RenderCopy(renderer, tex, nullptr, &rect);
        }
    });

    AddEventHandler("SDL::OnPollEvent", [this](int eventType, int key) {
        if (!running) return;
        if (renderer == nullptr) { return; }

        if (eventType == SDL_KEYDOWN) {
            if (key == SDLK_ESCAPE) {
                TriggerEvent("UFO::LeaderboardMenu::Close");
                running = false;
                return;
            }
        }else if (eventType == SDL_MOUSEBUTTONDOWN) {
            isMouseDown = true;
            return;
        }else if (eventType == SDL_MOUSEBUTTONUP && isMouseDown) {
            isMouseDown = false;
            int x, y;
            SDL_GetMouseState(&x, &y);

//            // if click within a save box then trigger UFO::SaveSelector::Select
//            for (int i = 0; i < max_display; i++) {
//                int pad = (padding * 2) + ((padding + templateBox.w) * i);
//                if (x >= templateBox.x + pad && x <= templateBox.x + pad + templateBox.w &&
//                    y >= templateBox.y && y <= templateBox.y + templateBox.h) {
//                    if (i + pagination >= saveData.size()) {
//                        // if we are at the end of the list then create a new save
//                        TriggerEvent("UFO::SaveSelector::Create");
//                        return;
//                    }else {
//                        TriggerEvent("UFO::SaveSelector::Select", i + pagination);
//                        return;
//                    }
//                }
//            }

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
        }else if (eventType == SDL_MOUSEMOTION) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            bool inZone = false;
//            for (int i = 0; i < max_display; i++) {
//                int pad = (padding * 2) + ((padding + templateBox.w) * i);
//                if (x >= templateBox.x + pad && x <= templateBox.x + pad + templateBox.w &&
//                    y >= templateBox.y && y <= templateBox.y + templateBox.h) {
//                    inZone = true;
//                    break;
//                }
//            }
            if (!inZone) {
                if (x >= leftArrow.x && x <= leftArrow.x + leftArrow.w &&
                    y >= leftArrow.y && y <= leftArrow.y + leftArrow.h) {
                    inZone = true;
                }else if (x >= rightArrow.x && x <= rightArrow.x + rightArrow.w &&
                          y >= rightArrow.y && y <= rightArrow.y + rightArrow.h) {
                    inZone = true;
                }
            }

            if (inZone) {
                TriggerEvent("UFO::Cursor::Change", "pointer");
            }else {
                TriggerEvent("UFO::Cursor::Change", "default");
            }
        }
    });

    running = true;
    reloadTextures();

    return 1;
}

void LeaderboardMenu::reloadTextures() {
    textures.clear();

    auto pH = [this](float num) -> float {
        return templateBox.h * (num / 100);
    };

    auto pW = [this](float num) -> float {
        return templateBox.w * (num / 100);
    };

    int index = 0;
    auto it = leaderboardTemplate.begin();
    std::advance(it, pagination);

    for (int i = 0; i < max_display && it != leaderboardTemplate.end(); ++i, ++it) {
        std::string leaderboardTitle = it->first;
        auto criteria = it->second;
        std::vector<json> entries(saveData.begin(), saveData.end());

        std::sort(entries.begin(), entries.end(), [criteria](const json& a, const json& b) {
            for (auto& [field, order] : criteria) {
                if (!a.contains(field) || !b.contains(field)) continue;
                int aVal = a[field].get<int>();
                int bVal = b[field].get<int>();
                if (order == 0 && aVal != bVal) return aVal > bVal;
                if (order == 1 && aVal != bVal) return aVal < bVal;
                if (order == 2) {
                    int diffA = a["TotalAT"].get<int>() - a["ATCount"].get<int>();
                    int diffB = b["TotalAT"].get<int>() - b["ATCount"].get<int>();
                    return diffA > diffB;
                }
            }
            return false;
        });

        int pad = (padding * 2) + ((padding + templateBox.w) * i);

        auto renderText = [&](const std::string& text, int line) {
            SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), {255, 255, 255});
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
            int w, h;
            SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
            SDL_Rect dst = {
                    static_cast<int>(templateBox.x + pad + pW(50) - w / 2),
                    static_cast<int>(templateBox.y + pH(5 + (line * 15))),
                    w,
                    h
            };
            textures.emplace_back(texture, dst);
        };

        renderText(leaderboardTitle, 0);
        for (int j = 0; j < std::min((int)entries.size(), 5); ++j) {
            const auto& entry = entries[j];
            std::ostringstream oss;
            oss << j + 1 << ". " << entry["name"].get<std::string>();
            renderText(oss.str(), j + 1);
        }
    }
}

void LeaderboardMenu::update(float deltaMs) {
    if (!running) return;
}

void LeaderboardMenu::drawRect(int x, int y, int w, int h) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderDrawRect(renderer, &rect);
}
