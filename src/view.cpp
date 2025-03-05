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

////
//// Created by xerxe on 2/6/2025.
////



#include "view.h"

// Process Functions
int view::initialize() {
    print("View Initialize");
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        error("SDL could not initialize!", SDL_GetError());
        return 0;
    }

    // Create window
    window = SDL_CreateWindow( "UFO", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( window == NULL ) {
        error("Window could not be created!", SDL_GetError());
        return 0;
    };

    // Small delay to allow the system to create the window.
    SDL_Delay(100);

    // Create renderer
    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
    if (renderer == nullptr) error("Unable to create renderer: ", SDL_GetError());
    print("View Renderer: ", renderer);


    // Get window surface
    screenSurface = SDL_GetWindowSurface( window );
    if(screenSurface == nullptr) {
        error("Unable to get window surface!", SDL_GetError());
        return 0;
    }

    // Make the surface blue
    SDL_FillRect( screenSurface, nullptr, SDL_MapRGB( screenSurface->format, 0, 0, 255 ) );

    // Update the surface
    SDL_UpdateWindowSurface( window );


    // Events List
    AddEventHandler("SDL::Render::DrawRect", [this](int x, int y, int w, int h) {
        drawRect(x, y, w, h);
    });

    AddEventHandler("SDL::Render::DrawLines", [this](std::vector<vector2> points, int thickness) {
        SDL_Point p[5] = {
                {static_cast<int>(points[0].x), static_cast<int>(points[0].y)},
                {static_cast<int>(points[1].x), static_cast<int>(points[1].y)},
                {static_cast<int>(points[2].x), static_cast<int>(points[2].y)},
                {static_cast<int>(points[3].x), static_cast<int>(points[3].y)},
                {static_cast<int>(points[0].x), static_cast<int>(points[0].y)}
        };
        if (SDL_RenderDrawLines(renderer, p, 5)) {
            error("SDL_RenderDrawLines Error: ", SDL_GetError());
        }
    });

    AddEventHandler("SDL::Render::SetDrawColor", [this](int r, int g, int b, int a) {
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
    });

    AddEventHandler("SDL::Render::ResetDrawColor", [this]() {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    });


    running = true;


    return 1;
}

void view::update(float deltaMs) {
    if (!running) return;

    // Clear screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
    SDL_RenderClear(renderer);


    // Handle events on queue
    while (SDL_PollEvent(&e))
    {
        // User requests quit
        if( e.type == SDL_QUIT ) running = false;

        // User presses a key
        if( e.type == SDL_KEYDOWN )
        {
            if( e.key.keysym.sym == SDLK_q ) running = false;
        }
        TriggerEvent("SDL::OnPollEvent", e.type, e.key.keysym.sym);
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color
    TriggerEvent("SDL::OnUpdate", deltaMs);
    SDL_RenderPresent(renderer);
}

bool view::isDone() {
    return !running;
}

void view::postSuccess() {
    print("View Post Success");
}

void view::postFail() {
    print("View Post Fail");
}

void view::postAbort() {
    print("View Post Abort");

    // Destroy renderer
    SDL_DestroyRenderer( renderer );

    // Destroy window
    SDL_DestroyWindow( window );

    // Quit SDL subsystems
    SDL_Quit();
}


void view::drawRect(int x, int y, int w, int h) {
    SDL_Rect fillRect = { x, y, w, h };
    SDL_RenderFillRect( renderer, &fillRect );
}