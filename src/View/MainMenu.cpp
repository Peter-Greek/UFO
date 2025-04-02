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



    running = true;
    // Using Layer 2 for rendering so it is on top of everything else
    AddEventHandler("SDL::OnUpdate::Layer2", [this](float deltaMs) {
        // While application is running
        if (!running) return;
        if (isHidden) {return;}

        if (renderer == nullptr) {return;}

        //Drawing menu background
        SDL_SetRenderDrawColor(renderer, 0, 75, 0, 255);
        SDL_RenderFillRect(renderer, &cbox);
        
        //Drawing start button
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 240);
        SDL_RenderDrawRect(renderer, &ATBox);

        /*if (ATText.texture) {
            SDL_SetTextureColorMod(ATText.texture, 255, 255, 255);
            SDL_RenderCopy(renderer, ATText.texture, nullptr, &ATText.dst);
        }*/
    });

    AddEventHandler("SDL::OnPollEvent", [this](int eventType, int key) {
        int x, y;
        if (isHidden) {
            if (eventType == SDL_KEYUP) {
                if (key == SDLK_m) {
                    showMainMenu();
                }
            }
        }else {
            if (eventType == SDL_KEYDOWN) {
                if (key == SDLK_ESCAPE) {
                    closeMainMenu();
                }
            } else if (eventType == SDL_MOUSEBUTTONDOWN) {
                SDL_GetMouseState(&x, &y);
                if(x > ATBox.x && y > ATBox.y && x < ATBox.x + ATBox.w && y < ATBox.y + ATBox.h)
                    closeMainMenu();
            }
        }
});
    AddEventHandler("UFO::MainMenu::DisplayATCount", [this](int ATCount) {
        displayATCount(ATCount);
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
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "main menu", color);
    displayATCount(AT);
}

void MainMenu::displayATCount(int ATCount){
    if (ATText.texture) {
        SDL_DestroyTexture(ATText.texture);
    }
    std::string ATCountText = "AT Count: " + std::to_string(ATCount);
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, ATCountText.c_str(), color);
    ATText.texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
}

void MainMenu::setATCount(int ATCount){
    AT = ATCount;
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