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
#include "../Controller/WorldCreator.h"
#include "Cursor.h"

#ifdef __WIN32__
#include <windows.h>
#endif

std::pair<int, int> getPrimaryDisplayResolution() {
    SDL_DisplayMode mode;
    if (SDL_GetCurrentDisplayMode(0, &mode) == 0) {
        return {mode.w, mode.h};
    } else {
        print("SDL_GetCurrentDisplayMode failed: %s", SDL_GetError());
        return {0, 0};
    }
}

std::pair <int, int> view::getScreenResolution() {
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

// Process Functions
int view::initialize() {
    #ifdef _WIN32
        // Force DPI Awareness at runtime
        typedef HRESULT(WINAPI *SetProcessDpiAwarenessFunc)(int);
        typedef BOOL(WINAPI *SetProcessDPIAwareFunc)(void);

        HMODULE shcore = LoadLibraryA("Shcore.dll");
        if (shcore) {
            auto setDpiAwareness = (SetProcessDpiAwarenessFunc)GetProcAddress(shcore, "SetProcessDpiAwareness");
            if (setDpiAwareness) {
                setDpiAwareness(2); // PROCESS_PER_MONITOR_DPI_AWARE
            }
            FreeLibrary(shcore);
        } else {
            HMODULE user32 = LoadLibraryA("user32.dll");
            if (user32) {
                auto setDpiAware = (SetProcessDPIAwareFunc)GetProcAddress(user32, "SetProcessDPIAware");
                if (setDpiAware) {
                    setDpiAware();
                }
                FreeLibrary(user32);
            }
        }
    #endif

    print("View Initialize");

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        error("SDL could not initialize!", SDL_GetError());
        return 0;
    }

    auto wh = getPrimaryDisplayResolution();
    if (FULL_SCREEN_ENABLED) {
        print("Fullscreen enabled");
        // Get the current display size and apply it
        print("Display size: ", wh.first, wh.second);
        SCREEN_WIDTH = wh.first;
        SCREEN_HEIGHT = wh.second;
    }else {
        // Get the current display size and ensure set resolution is not larger
        if (SCREEN_RESOLUTION.first != 0 && SCREEN_RESOLUTION.second != 0) {
            print("Set resolution found: ", SCREEN_RESOLUTION.first, SCREEN_RESOLUTION.second);
            if (SCREEN_RESOLUTION.first > wh.first) {
                SCREEN_RESOLUTION.first = wh.first;
            }
            if (SCREEN_RESOLUTION.second > wh.second) {
                SCREEN_RESOLUTION.second = wh.second;
            }
            SCREEN_WIDTH = SCREEN_RESOLUTION.first;
            SCREEN_HEIGHT = SCREEN_RESOLUTION.second;
        }else {
            print("No set resolution found, using default");
            // if the set screen size does not exist then lets just go 1024x768 or fullscreen
            if (wh.first < 1024) {
                SCREEN_WIDTH = wh.first;
            }else {
                SCREEN_WIDTH = 1024;
            }
            if (wh.second < 768) {
                SCREEN_HEIGHT = wh.second;
            }else {
                SCREEN_HEIGHT = 768;
            }

            SCREEN_RESOLUTION.first = SCREEN_WIDTH;
            SCREEN_RESOLUTION.second = SCREEN_HEIGHT;
            TriggerEvent("UFO::ChangeConfigValue", "SCREEN_RESOLUTION"); // refresh settings fix
        }
    }

    // Create window with High DPI awareness
    print("Creating Window", SCREEN_WIDTH, SCREEN_HEIGHT);
    window = SDL_CreateWindow("UFO",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT,
                              SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN);

    if (!window) {
        error("Window could not be created!", SDL_GetError());
        return 0;
    }

    // Small delay to ensure surface is ready
    SDL_Delay(100);

    #ifdef __APPLE__
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    #else
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    #endif

    if (!renderer) {
        error("Unable to create renderer: ", SDL_GetError());
        return 0;
    }

    // Scale all rendering to logical screen resolution (1920x1080)
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);


    // Favicon

    // Load the full image
//    SDL_Surface* fullImage = IMG_Load("../resource/GFX/screens/MainMenuV2.png");
//    if (!fullImage) {
//        error("Failed to load image for icon: ", IMG_GetError());
//    }
//    // Define the subregion you want (x, y, width, height)
//    SDL_Rect iconRect = { 265, 22, 765 - 265, 427-22 };

//    SDL_Surface* fullImage = IMG_Load("../resource/GFX/screens/pixil-frame-0.png");
//    if (!fullImage) {
//        error("Failed to load image for icon: ", IMG_GetError());
//    }
//    // Define the subregion you want (x, y, width, height)
//    SDL_Rect iconRect = { 0, 0, 2000, 2000 };  // cropped from y = 4, height = 28

    SDL_Surface* fullImage = IMG_Load("../resource/GFX/sprites/FSS.png");
    if (!fullImage) {
        error("Failed to load image for icon: ", IMG_GetError());
    }
    // Define the subregion you want (x, y, width, height)
    SDL_Rect iconRect = { 0, 8, 32, 32 };  // cropped from y = 4, height = 28

    // Create a surface to hold the icon
    SDL_Surface* iconSurface = SDL_CreateRGBSurface(0, iconRect.w, iconRect.h,
                                                    fullImage->format->BitsPerPixel,
                                                    fullImage->format->Rmask,
                                                    fullImage->format->Gmask,
                                                    fullImage->format->Bmask,
                                                    fullImage->format->Amask);

    if (!iconSurface) {
        SDL_FreeSurface(fullImage);
        error("Failed to create surface for icon: ", SDL_GetError());
    }

    // Blit (copy) the region from full image into icon surface
    SDL_BlitSurface(fullImage, &iconRect, iconSurface, nullptr);

    // Set the window icon
    SDL_SetWindowIcon(window, iconSurface);

    // Clean up
    SDL_FreeSurface(iconSurface);
    SDL_FreeSurface(fullImage);



    // Only needed on Windows/Linux for visual debug background
    #ifndef __APPLE__
        screenSurface = SDL_GetWindowSurface(window);
        if (!screenSurface) {
            error("Unable to get window surface!", SDL_GetError());
            return 0;
        }
        SDL_FillRect(screenSurface, nullptr, SDL_MapRGB(screenSurface->format, 0, 0, 255));
        SDL_UpdateWindowSurface(window);
    #endif

    // Event Handlers
    AddEventHandler("SDL::Render::DrawRect", [this](int x, int y, int w, int h) {
        drawRect(x, y, w, h);
    });

    AddEventHandler("SDL::Render::DrawLines", [this](std::vector<vector2> points, int thickness) {
        SDL_Point p[5] = {
                {(int)points[0].x, (int)points[0].y},
                {(int)points[1].x, (int)points[1].y},
                {(int)points[2].x, (int)points[2].y},
                {(int)points[3].x, (int)points[3].y},
                {(int)points[0].x, (int)points[0].y}
        };
        SDL_RenderDrawLines(renderer, p, 5);
    });

    AddEventHandler("SDL::Render::DrawPoint", [this](int x, int y) {
        SDL_RenderDrawPoint(renderer, x, y);
    });

    AddEventHandler("SDL::Render::DrawPointFromVector", [this](vector2 point) {
        SDL_RenderDrawPoint(renderer, (int)point.x, (int)point.y);
    });

    AddEventHandler("SDL::Render::SetDrawColor", [this](int r, int g, int b, int a) {
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
    });

    AddEventHandler("SDL::Render::ResetDrawColor", [this]() {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    });

    AddEventHandler("SDL::Render::DrawCircle", [this](int x, int y, int radius) {
        SDL_RenderDrawCircle(x, y, radius);
    });

    AddEventHandler("SDL::Render::FillCircle", [this](int x, int y, int radius) {
        SDL_RenderFillCircle(x, y, radius);
    });

    AddEventHandler("UFO::Chat::State", [this](bool state) {
        chatState = state;
    });

    running = true;

    // Init other processes
    chatBox = std::make_shared<ChatBox>(passFunc);
    pM->attachProcess(chatBox);
    if (chatBox->initialize_SDL_process(window)) {
        chatBox->initialized();
    }
    chatBox->addMessage("Hello World");
    AddEventHandler("UFO::View::UpdateWindowSize", [this](int w, int h) {
        std::pair<std::vector<ChatBox::ChatMessage>, sList_t> messagesAndCommands;
        if (chatBox != nullptr) {
            messagesAndCommands = chatBox->getMessagesAndCommands();
            chatBox->abort();
            chatBox = nullptr;
            chatState = false;
        }
        chatBox = std::make_shared<ChatBox>(passFunc);
        pM->attachProcess(chatBox);
        if (chatBox->initialize_SDL_process(window)) {
            chatBox->initialized();
        }
        chatBox->setMessagesAndCommands(messagesAndCommands);
        chatBox->addMessage("Hello World");
    });



    auto* WC = new WorldCreator(passFunc);
    pM->attachProcess(WC);
    chatBox->addMessage("World Creator Attached");

    auto* cursor = new Cursor(passFunc);
    pM->attachProcess(cursor);

    // Debug print sizes
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    print("Window Size: ", w, "x", h);

    SDL_GetRendererOutputSize(renderer, &w, &h);
    print("Renderer Output Size: ", w, "x", h);

    float ddpi, hdpi, vdpi;
    if (SDL_GetDisplayDPI(0, &ddpi, &hdpi, &vdpi) == 0) {
        print("DPI: ", ddpi, " HDPI: ", hdpi, " VDPI: ", vdpi);
    }

    RegisterCommand("resizeWindow", [this](std::string command, sList_t args, std::string message) {
        if (args.empty()) {
            // get current display size and apply it as the args
            int displayIndex = SDL_GetWindowDisplayIndex(window);
            if (displayIndex < 0) {
                print("SDL_GetWindowDisplayIndex failed: %s", SDL_GetError());
            } else {
                SDL_DisplayMode mode;
                if (SDL_GetCurrentDisplayMode(displayIndex, &mode) == 0) {
                    int displayWidth = mode.w;
                    int displayHeight = mode.h;
                    print("Display size: %dx%d", displayWidth, displayHeight);
                    args.push_back(std::to_string(displayWidth));
                    args.push_back(std::to_string(displayHeight));
                } else {
                    print("SDL_GetCurrentDisplayMode failed: %s", SDL_GetError());
                }
            }
        }
        if (args.size() < 2) {
            TriggerEvent("UFO::Chat::AddMessage", "Incorrect Usage: resizeWindow <width> <height>");
            return;
        }

        int width = std::stoi(args[0]);
        int height = std::stoi(args[1]);
        TriggerEvent("UFO::Chat::AddMessage", "Resizing Window to: " + std::to_string(width) + "x" + std::to_string(height));
        resizeWindow(width, height, true);
        TriggerEvent("UFO::Chat::AddMessage", "Window Resized: " + std::to_string(width) + "x" + std::to_string(height));
    });

    AddEventHandler("UFO::SaveSelector::Create", [this]() {
        if (chatBox != nullptr) {
            chatBox->hideChatBox(); // hide chatbox when creating a new user
        }
    });

    AddEventHandler("UFO::View::ResizeWindow", [this](int width, int height) {
        if (width == 0 && height == 0) {
            // get current display size and apply it as the args
            int displayIndex = SDL_GetWindowDisplayIndex(window);
            if (displayIndex < 0) {
                print("SDL_GetWindowDisplayIndex failed: %s", SDL_GetError());
                return;
            } else {
                SDL_DisplayMode mode;
                if (SDL_GetCurrentDisplayMode(displayIndex, &mode) == 0) {
                    int displayWidth = mode.w;
                    int displayHeight = mode.h;
                    print("Display size: %dx%d", displayWidth, displayHeight);
                    width = displayWidth;
                    height = displayHeight;
                } else {
                    print("SDL_GetCurrentDisplayMode failed: %s", SDL_GetError());
                    return;
                }
            }
        }




        resizeWindow(width, height, true);
    });

    return 1;
}

void view::resizeWindow(int width, int height, bool center) {
    if (!window) return;
    if (!renderer) return;
    if (!running) return;

    if (width <= 0 || height <= 0) {
        return;
    }


    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;

    SDL_SetWindowSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);

    if (center) {
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }

    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    print("Window resized to: ", SCREEN_WIDTH, "x", SCREEN_HEIGHT);

    // Optional: log new renderer output size
    int rw, rh;
    SDL_GetRendererOutputSize(renderer, &rw, &rh);
    print("Renderer Output Size after resize: ", rw, "x", rh);

    TriggerEvent("UFO::View::UpdateWindowSize", SCREEN_WIDTH, SCREEN_HEIGHT);
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
            if (!chatState) {
                if( e.key.keysym.sym == SDLK_q ) {
                    TriggerEvent("UFO::Quit");
                    running = false;
                }
                if ( e.key.keysym.sym == SDLK_b ) {
                    TriggerEvent("UFO::ChangeConfigValue", "debugMode");
                }

                if ( e.key.keysym.sym == SDLK_f ) {
                    TriggerEvent("UFO::ChangeConfigValue", "unlimitedFrames");
                }

            }else {
                const Uint8 *keyboard_state_array = SDL_GetKeyboardState(nullptr);
                // if control q is pressed then quit
                if (keyboard_state_array[SDL_SCANCODE_LCTRL] && e.key.keysym.sym == SDLK_q) {
                    running = false;
                }
            }
        }
        TriggerEvent("SDL::OnPollEvent", e.type, e.key.keysym.sym);
        if (e.type == SDL_TEXTINPUT) {
            TriggerEvent("SDL::OnTextInput", e.text.text);
        }
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color
    TriggerEvent("SDL::OnUpdate", deltaMs);
    TriggerEvent("SDL::OnUpdate::Layer2", deltaMs); // second layer
    SDL_RenderPresent(renderer);
}

bool view::isDone() {
    return !running;
}

void view::postSuccess() {
}

void view::postFail() {
}

void view::postAbort() {

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


// Credit: https://gist.github.com/Gumichan01/332c26f6197a432db91cc4327fcabb1c
// Author: Gumichan01
// Slightly edited to remove CHECK_RENDERER_MAGIC
int
view::SDL_RenderDrawCircle(int x, int y, int radius)
{
    int offsetx, offsety, d;
    int status;

    offsetx = 0;
    offsety = radius;
    d = radius -1;
    status = 0;

    while (offsety >= offsetx) {
        status += SDL_RenderDrawPoint(renderer, x + offsetx, y + offsety);
        status += SDL_RenderDrawPoint(renderer, x + offsety, y + offsetx);
        status += SDL_RenderDrawPoint(renderer, x - offsetx, y + offsety);
        status += SDL_RenderDrawPoint(renderer, x - offsety, y + offsetx);
        status += SDL_RenderDrawPoint(renderer, x + offsetx, y - offsety);
        status += SDL_RenderDrawPoint(renderer, x + offsety, y - offsetx);
        status += SDL_RenderDrawPoint(renderer, x - offsetx, y - offsety);
        status += SDL_RenderDrawPoint(renderer, x - offsety, y - offsetx);

        if (status < 0) {
            status = -1;
            break;
        }

        if (d >= 2*offsetx) {
            d -= 2*offsetx + 1;
            offsetx +=1;
        }
        else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

    return status;
}


int view::SDL_RenderFillCircle(int x, int y, int radius)
{
    int offsetx, offsety, d;
    int status;

    offsetx = 0;
    offsety = radius;
    d = radius -1;
    status = 0;

    while (offsety >= offsetx) {

        status += SDL_RenderDrawLine(renderer, x - offsety, y + offsetx,
                                     x + offsety, y + offsetx);
        status += SDL_RenderDrawLine(renderer, x - offsetx, y + offsety,
                                     x + offsetx, y + offsety);
        status += SDL_RenderDrawLine(renderer, x - offsetx, y - offsety,
                                     x + offsetx, y - offsety);
        status += SDL_RenderDrawLine(renderer, x - offsety, y - offsetx,
                                     x + offsety, y - offsetx);

        if (status < 0) {
            status = -1;
            break;
        }

        if (d >= 2*offsetx) {
            d -= 2*offsetx + 1;
            offsetx +=1;
        }
        else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

    return status;
}