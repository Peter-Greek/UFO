#include "UpgradeMenu.h"

int UpgradeMenu::initialize_SDL_process(SDL_Window* passed_window) {
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



    running = true;
    // Using Layer 2 for rendering so it is on top of everything else
    AddEventHandler("SDL::OnUpdate::Layer2", [this](float deltaMs) {
        // While application is running
        if (!running) return;
        if (isHidden) {return;}

        if (renderer == nullptr) {return;}

        SDL_SetRenderDrawColor(renderer, 0, 0, 75, 255);
        SDL_RenderFillRect(renderer, &cbox);
    });

    AddEventHandler("SDL::OnPollEvent", [this](int eventType, int key) {
        if (isHidden) {
            if (eventType == SDL_KEYUP) {
                if (key == SDLK_u) {
                    showUpgradeMenu();
                }
            }
        }else {
            if (eventType == SDL_KEYDOWN) {
                if (key == SDLK_ESCAPE) {
                    closeUpgradeMenu();
                }
            }
        }
});
    return 1;
}

void UpgradeMenu::update(float deltaMs) {
    // While application is running
    if (!running) return;
    if (renderer == nullptr) { return; }
    if (isHidden) { return; }
    const Uint8 *keyboard_state_array = SDL_GetKeyboardState(nullptr);
}

void UpgradeMenu::showUpgradeMenu() {
    isHidden = false;
    if (!running) return;
    TriggerEvent("UFO::UpgradeMenu::State", true);

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "upgrade menu", color);
}

void UpgradeMenu::closeUpgradeMenu() {
    isHidden = true;
    if (!running) return;
    TriggerEvent("UFO::UpgradeMenu::State", false);
}

void UpgradeMenu::updateUpgradeMenuPositioning() {
    cbox = {
            static_cast<int>(position.x),
            static_cast<int>(position.y),
            static_cast<int>(upgradeMenuSize.x),
            static_cast<int>(upgradeMenuSize.y)
    };
}

void UpgradeMenu::setUpgradeMenuPosition(vector2 pos) {
    upgradeMenuSize = pos;
    updateUpgradeMenuPositioning();
}

void UpgradeMenu::setUpgradeMenuPosition(float x, float y) {
    upgradeMenuSize = {x, y};
    updateUpgradeMenuPositioning();
}

void UpgradeMenu::setUpgradeMenuSize(vector2 size) {
    upgradeMenuSize = size;
    updateUpgradeMenuPositioning();
}

void UpgradeMenu::setUpgradeMenuSize(float x, float y) {
    upgradeMenuSize = {x, y};
    updateUpgradeMenuPositioning();
}