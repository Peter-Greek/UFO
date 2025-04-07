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

    SDL_Surface* playSurface = TTF_RenderText_Solid(font, "Play", color);
    PlayText.texture = SDL_CreateTextureFromSurface(renderer, playSurface);
    SDL_Surface* speedSurface = TTF_RenderText_Solid(font, "2 AT: Speed", color);
    SpeedText.texture = SDL_CreateTextureFromSurface(renderer, speedSurface);
    SDL_Surface* oxygenSurface = TTF_RenderText_Solid(font, "2 AT: Oxygen", color);
    OxygenText.texture = SDL_CreateTextureFromSurface(renderer, oxygenSurface);
    SDL_Surface* shieldSurface = TTF_RenderText_Solid(font, "10 AT: Shield", color);
    ShieldText.texture = SDL_CreateTextureFromSurface(renderer, shieldSurface);
    SDL_Surface* invisSurface = TTF_RenderText_Solid(font, "10 AT: Invisibility", color);
    InvisibilityText.texture = SDL_CreateTextureFromSurface(renderer, invisSurface);
    SDL_Surface* cannonSurface = TTF_RenderText_Solid(font, "25 AT: Cannon", color);
    CannonText.texture = SDL_CreateTextureFromSurface(renderer, cannonSurface);

    running = true;
    // Using Layer 2 for rendering so it is on top of everything else
    AddEventHandler("SDL::OnUpdate::Layer2", [this](float deltaMs) {
        // While application is running
        if (!running) return;
        if (isHidden) {return;}

        if (renderer == nullptr) {return;}

        //Drawing menu background
        SDL_SetRenderDrawColor(renderer, 0, 0, 75, 255);
        SDL_RenderFillRect(renderer, &cbox);
        
        //Drawing box to show AT count
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 240);
        SDL_RenderDrawRect(renderer, &ATBox);

        if (ATText.texture) {
            SDL_SetTextureColorMod(ATText.texture, 255, 255, 255);
            SDL_RenderCopy(renderer, ATText.texture, nullptr, &ATText.dst);
        }

        //Draw Play Button
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &PlayButton);

        if (PlayText.texture) {
            SDL_SetTextureColorMod(PlayText.texture, 255, 255, 255);
            SDL_RenderCopy(renderer, PlayText.texture, nullptr, &PlayText.dst);
        }

        //Draw Speed Button
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &SpeedButton);

        if (SpeedText.texture) {
            SDL_SetTextureColorMod(SpeedText.texture, 255, 255, 255);
            SDL_RenderCopy(renderer, SpeedText.texture, nullptr, &SpeedText.dst);
        }

        //Draw Oxygen Button
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &OxygenButton);

        if (OxygenText.texture) {
            SDL_SetTextureColorMod(OxygenText.texture, 255, 255, 255);
            SDL_RenderCopy(renderer, OxygenText.texture, nullptr, &OxygenText.dst);
        }

        //Draw Shield Button
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &ShieldButton);

        if (ShieldText.texture) {
            SDL_SetTextureColorMod(ShieldText.texture, 255, 255, 255);
            SDL_RenderCopy(renderer, ShieldText.texture, nullptr, &ShieldText.dst);
        }

        //Draw Invisibility Button
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &InvisButton);

        if (InvisibilityText.texture) {
            SDL_SetTextureColorMod(InvisibilityText.texture, 255, 255, 255);
            SDL_RenderCopy(renderer, InvisibilityText.texture, nullptr, &InvisibilityText.dst);
        }

        //Draw AT Cannon Button
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &CannonButton);

        if (CannonText.texture) {
            SDL_SetTextureColorMod(CannonText.texture, 255, 255, 255);
            SDL_RenderCopy(renderer, CannonText.texture, nullptr, &CannonText.dst);
        }        

    });

    AddEventHandler("SDL::OnPollEvent", [this](int eventType, int key) {
        int x, y;
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
            } else if (eventType == SDL_MOUSEBUTTONDOWN) { //call appropriate action for each button clicked
                SDL_GetMouseState(&x, &y);
                if(x > PlayButton.x && y > PlayButton.y && x < PlayButton.x + PlayButton.w && y < PlayButton.y + PlayButton.h)
                    closeUpgradeMenu();
                else if(x > SpeedButton.x && y > SpeedButton.y && x < SpeedButton.x + SpeedButton.w && y < SpeedButton.y + SpeedButton.h)
                    TriggerEvent("UFO::UpgradePurchased", "speed", 2);
                else if(x > OxygenButton.x && y > OxygenButton.y && x < OxygenButton.x + OxygenButton.w && y < OxygenButton.y + OxygenButton.h)
                    TriggerEvent("UFO::UpgradePurchased", "oxygen", 2);
                else if(x > ShieldButton.x && y > ShieldButton.y && x < ShieldButton.x + ShieldButton.w && y < ShieldButton.y + ShieldButton.h)
                    TriggerEvent("UFO::UpgradePurchased", "shield", 10);
                else if(x > InvisButton.x && y > InvisButton.y && x < InvisButton.x + InvisButton.w && y < InvisButton.y + InvisButton.h)
                    TriggerEvent("UFO::UpgradePurchased", "invisibility", 10);
                else if(x > CannonButton.x && y > CannonButton.y && x < CannonButton.x + CannonButton.w && y < CannonButton.y + CannonButton.h)
                    TriggerEvent("UFO::UpgradePurchased", "at_cannon", 25);
            }
        }
});
    AddEventHandler("UFO::UpgradeMenu::DisplayATCount", [this](int ATCount) {
        displayATCount(ATCount);
    });

    displayATCount(AT);
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
    displayATCount(AT);
}

void UpgradeMenu::displayATCount(int ATCount){
    if (ATText.texture) {
        SDL_DestroyTexture(ATText.texture);
    }
    std::string ATCountText = "AT Count: " + std::to_string(ATCount);
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, ATCountText.c_str(), color);
    ATText.texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
}

void UpgradeMenu::setATCount(int ATCount){
    AT = ATCount;
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

void UpgradeMenu::setFontColor(int r, int g, int b, int a) {
    color = {static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a)};
}