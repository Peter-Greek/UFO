#include "Cursor.h"
#include <SDL_image.h>

Cursor::Cursor(passFunc_t passFunc)
        : xProcess(true, passFunc) {}

Cursor::~Cursor() {
    cleanup();
}

int Cursor::initialize_SDL_process(SDL_Window* passed_window) {
    window = passed_window;

    if (!window) {
        error("Cursor: Invalid window.");
        return 0;
    }

    renderer = SDL_GetRenderer(window);
    if (!renderer) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    }

    SDL_ShowCursor(SDL_ENABLE);

    fullSheet = IMG_Load(cursorPath.c_str());
    if (!fullSheet) {
        error("Cursor: Failed to load cursor sprite sheet: ", IMG_GetError());
    }

    // Convert for alpha support
    SDL_Surface* converted = SDL_ConvertSurfaceFormat(fullSheet, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(fullSheet);
    fullSheet = converted;

    defineCursorRegions();
    createCursors();
    overrideCursor("default");

    AddEventHandler("UFO::Cursor::Change", [this](const std::string name) {
        overrideCursor(name);
    });

    return 1;
}

void Cursor::defineCursorRegions() {
    // Define regions on the cursor sprite sheet
    // Example cursors from rows of 32x32 tiles
    cursorRegions["default"] = {105, 375, 178-105, 467-375};      // Default
    cursorRegions["default2"] = {98, 104, 178-98, 189-104};
    cursorRegions["crosshair"] = {784, 516, 871-784, 600-516};
    cursorRegions["pointer"] = {381, 110, 450 - 381, 189 - 100};
    // Add more as needed
}

void Cursor::createCursors() {
    for (const auto& [name, region] : cursorRegions) {
        SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, region.w, region.h, 32, SDL_PIXELFORMAT_RGBA32);
        SDL_Rect dst = {0, 0, region.w, region.h};
        SDL_BlitSurface(fullSheet, &region, surface, &dst);

        cursors[name] = {
                surface,
                {region.w, region.h},
                {0, 0} // Optional: set hotspot if needed
        };
    }
}

void Cursor::overrideCursor(const std::string& name) {
    auto it = cursors.find(name);
    if (it == cursors.end()) {
        print("Cursor: Unknown cursor name:", name);
        return;
    }

    if (currentCursor == name && activeCursor != nullptr) {
        return; // No need to change if it's already the current cursor
    }

    currentCursor = name;

    if (activeCursor) {
        SDL_FreeCursor(activeCursor);
    }

    const CursorData& data = it->second;
    activeCursor = SDL_CreateColorCursor(data.surface, data.hotspot.x, data.hotspot.y);
    if (!activeCursor) {
        error("Cursor: Failed to create SDL_ColorCursor: ", SDL_GetError());
    }
    SDL_SetCursor(activeCursor);
}

std::string Cursor::getCurrentCursor() const {
    return currentCursor;
}

SDL_Point Cursor::getCursorSize() const {
    auto it = cursors.find(currentCursor);
    if (it != cursors.end()) {
        return it->second.size;
    }
    return {0, 0};
}

void Cursor::update(float) {}

bool Cursor::isDone() {
    return false;
}

void Cursor::cleanup() {
    if (fullSheet) {
        SDL_FreeSurface(fullSheet);
        fullSheet = nullptr;
    }

    for (auto& [_, data] : cursors) {
        if (data.surface) SDL_FreeSurface(data.surface);
    }
    cursors.clear();

    if (activeCursor) {
        SDL_FreeCursor(activeCursor);
        activeCursor = nullptr;
    }
}
