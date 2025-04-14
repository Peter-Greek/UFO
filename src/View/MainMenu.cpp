#include "MainMenu.h"

int MainMenu::initialize_SDL_process(SDL_Window* passed_window) {
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

    // Render the texture
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "main menu", color);


    // Set the button coords
    StartBox = {
            static_cast<int>(414 * (SCREEN_WIDTH / 1024.0f)),
            static_cast<int>(475 * (SCREEN_HEIGHT / 768.0f)),
            static_cast<int>(195 * (SCREEN_WIDTH / 1024.0f)),
            static_cast<int>(84 * (SCREEN_HEIGHT / 768.0f))
    };

    // Set the button coords
    SettingsBox = {
        static_cast<int>(406 * (SCREEN_WIDTH / 1024.0f)),
        static_cast<int>(582 * (SCREEN_HEIGHT / 768.0f)),
        static_cast<int>(216 * (SCREEN_WIDTH / 1024.0f)),
        static_cast<int>(70 * (SCREEN_HEIGHT / 768.0f))
    };

    // Set the button coords
    LeaderboardBox = {
        static_cast<int>(360 * (SCREEN_WIDTH / 1024.0f)),
        static_cast<int>(674 * (SCREEN_HEIGHT / 768.0f)),
        static_cast<int>(308 * (SCREEN_WIDTH / 1024.0f)),
        static_cast<int>(68 * (SCREEN_HEIGHT / 768.0f))
    };


    running = true;
    isHidden = false;
    // Using Layer 2 for rendering so it is on top of everything else
    AddEventHandler("SDL::OnUpdate", [this](float deltaMs) {
        // While application is running
        if (!running) return;
        if (isHidden) {return;}

        if (renderer == nullptr) {return;}

        if (menuTxd != nullptr && menuTxd->state() == xProcess::RUNNING) {
            menuTxd->render(srcRect, destRect, 0, SDL_FLIP_NONE);
        }
        
        //Drawing start button
        if (isDebug()) {
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 240);
            SDL_RenderDrawRect(renderer, &StartBox);
        }
    });

    AddEventHandler("SDL::OnPollEvent", [this](int eventType, int key) {
        int x, y;
        if (eventType == SDL_MOUSEBUTTONDOWN) {
            SDL_GetMouseState(&x, &y);
            if(x > StartBox.x && y > StartBox.y && x < StartBox.x + StartBox.w && y < StartBox.y + StartBox.h) {
                TriggerEvent("UFO::StartGame");
            }
            else if(x > SettingsBox.x && y > SettingsBox.y && x < SettingsBox.x + SettingsBox.w && y < SettingsBox.y + SettingsBox.h) {
                TriggerEvent("UFO::SetSettingsState", true);
            }
            else if(x > LeaderboardBox.x && y > LeaderboardBox.y && x < LeaderboardBox.x + LeaderboardBox.w && y < LeaderboardBox.y + LeaderboardBox.h) {
                TriggerEvent("UFO::OpenLeaderboard", true);
            }
        }
    });
    return 1;
}

void MainMenu::update(float deltaMs) {
    // While application is running
    if (!running) return;
    if (renderer == nullptr) { return; }
    if (isHidden) { return; }
    const Uint8 *keyboard_state_array = SDL_GetKeyboardState(nullptr);
}

void MainMenu::showMainMenu() {
    isHidden = false;
    if (!running) return;
    TriggerEvent("UFO::MainMenu::State", true);

}

void MainMenu::closeMainMenu() {
    isHidden = true;
    if (!running) return;
    TriggerEvent("UFO::MainMenu::State", false);
}


void MainMenu::updateMainMenuPositioning() {
    cbox = {
            static_cast<int>(position.x),
            static_cast<int>(position.y),
            static_cast<int>(mainMenuSize.x),
            static_cast<int>(mainMenuSize.y)
    };
}

void MainMenu::setMainMenuPosition(vector2 pos) {
    mainMenuSize = pos;
    updateMainMenuPositioning();
}

void MainMenu::setMainMenuPosition(float x, float y) {
    mainMenuSize = {x, y};
    updateMainMenuPositioning();
}

void MainMenu::setMainMenuSize(vector2 size) {
    mainMenuSize = size;
    updateMainMenuPositioning();
}

void MainMenu::setMainMenuSize(float x, float y) {
    mainMenuSize = {x, y};
    updateMainMenuPositioning();
}

void MainMenu::setFontColor(int r, int g, int b, int a) {
    color = {static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a)};
}