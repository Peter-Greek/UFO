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

//Using SDL and standard IO

#include "text.h"

#include <utility>

int text::initialize_SDL_process(SDL_Window* passed_window) {
    print("Text Initialize SDL Process: ", passed_window);
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

    const char* textInput = textContent.c_str();



    // render text
    color = { 255, 255, 255 };
    sText = TTF_RenderText_Solid( font, textInput, color );
    if ( sText == nullptr ) {
        error("Unable to render text! ", TTF_GetError());
        return 0;
    };

    // convert to texture
    texture = SDL_CreateTextureFromSurface( renderer, sText );
    if(texture == nullptr) {
        error("Could not create texture from surface! ", SDL_GetError());
        return 0;
    }


//    TTF_SizeText(font, textInput, &textWidth, &textHeight);
    SDL_QueryTexture(texture, nullptr, nullptr, &textWidth, &textHeight);



    print("Pre-Render Text: ", textInput, ' ', textWidth, ' ', textHeight);
    dst.x = position.x;
    dst.y = position.y;
    dst.w = textWidth;
    dst.h = textHeight;


    // delete image


    /*** Main Loop ***/
    print("Text: ", textInput, " is hidden: ", isHidden);
    running = !isHidden;

    AddEventHandler("SDL::OnPollEvent", [this](int eventType, int key) {});


    AddEventHandler("SDL::OnUpdate", [this](float deltaMs) {
        // While application is running
        if (!running) return;
        SDL_SetTextureColorMod(texture, red * 255, green * 255, blue * 255);
        SDL_RenderCopyEx(renderer, texture, nullptr, &dst, angle, &rot, SDL_FLIP_NONE);
    });



    if (isHidden) {
        SDL_FreeSurface( sText );
        SDL_DestroyTexture(texture);
        sText = nullptr;
    }

    return 1;
}

void text::update(float deltaMs) {
    // While application is running
    if (!running) return;
    setCurrentPositionBasedOnRelativePosition();
    dst.x = position.x;
    dst.y = position.y;
}


bool text::isDone() {
    return !running;
}

void text::postSuccess() {
    print("Text Post Success");
}

void text::postFail() {
    print("Text Post Fail");
}

void text::postAbort() {
    print("Text Post Abort");

    // Destroy texture
    if (!running) return;
    SDL_FreeSurface( sText );
    SDL_DestroyTexture( texture );
    sText = nullptr;
}



void text::setTextPosition(int x, int y) {
    resetRelativePosition();
    position = vector2(x, y);
};
void text::setTextPosition(vector2 pos) {
    resetRelativePosition();
    position = pos;
};
void text::setTextRelativePosition(float x, float y) {
    relativePosition = vector2(x, y);
}
void text::resetRelativePosition() {
    relativePosition = vector2(0.0f, 0.0f);
}
void text::setCurrentPositionBasedOnRelativePosition() {
    if (relativePosition.x == 0.0f && relativePosition.y == 0.0f) return;

    float x = relativePosition.x;
    float y = relativePosition.y;

    // 0,0 is the center of the image in SDL
    float screenW = static_cast<float>(SCREEN_WIDTH);
    float screenH = static_cast<float>(SCREEN_HEIGHT);
    float textW = static_cast<float>(textWidth);
    float textH = static_cast<float>(textHeight);

    float baseX = screenW / 2.0f - textW / 2.0f;
    float baseY = screenH / 2.0f - textH / 2.0f;

    float toX = baseX;
    float toY = baseY;

    if (x > 0.0f) {
        toX = map_range(x, 0.0f, 1.0f, baseX, screenW);
    } else if (x < 0.0f) {
        toX = map_range(x, -1.0f, 0.0f, 0.0f, baseX);
    }

    if (y > 0.0f) {
        toY = map_range(y, 0.0f, 1.0f, baseY, 0);
    } else if (y < 0.0f) {
        toY = map_range(y, -1.0f, 0.0f, screenH, baseY);
    }

    position = vector2(toX, toY);
}

std::string text::getText() {
    return textContent;
}

void text::hideText() {
    print("Attempt Hide Text: ", textContent);
    isHidden = true;
    if (!running) return;
    print("Hiding Text: ", textContent);
    running = false; // prevent further updates
    if (sText != nullptr) {
        SDL_FreeSurface( sText );
        SDL_DestroyTexture(texture);
    }
    sText = nullptr;
}
void text::showText() {
    print("Attempt Show Text: ", textContent);
    isHidden = false;
    if (running) return;
    print("Showing Text: ", textContent);
    running = true;
    setText(textContent);
}

void text::setFontSize(int toFontSize) {
    fontSize = toFontSize;
    font = TTF_OpenFont("../resource/Arial.ttf", 50);
    if (font == nullptr) {
        error("Unable to open font! ", SDL_GetError());
    }
    setText(textContent);
}


void text::setText(std::string basicString) {
    if (!running) return;


    if (sText != nullptr) {
        SDL_FreeSurface( sText );
        SDL_DestroyTexture(texture);
    }

    textContent = std::move(basicString);
    const char* textInput = textContent.c_str();

    int textWidth;
    int textHeight;
    TTF_SizeText(font, textInput, &textWidth, &textHeight);

    // render text
    color = { 255, 255, 255 };
    sText = TTF_RenderText_Solid( font, textInput, color );
    if ( sText == nullptr ) {
        error("Unable to render text! ", TTF_GetError());
        return;
    };

    // convert to texture
    texture = SDL_CreateTextureFromSurface( renderer, sText );
    if(texture == nullptr) {
        error("Could not create texture from surface! ", SDL_GetError());
        return;
    }

    SDL_SetTextureColorMod(texture, red * 255, green * 255, blue * 255);
    SDL_RenderCopyEx(renderer, texture, nullptr, &dst, angle, &rot, SDL_FLIP_NONE);
}
