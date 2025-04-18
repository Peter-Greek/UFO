//
// Created by xerxe on 4/17/2025.
//

#ifndef CSCI437_CURSOR_H
#define CSCI437_CURSOR_H

#include "xProcess.h"
#include <SDL.h>
#include <map>
#include <string>

class Cursor : public xProcess {
public:
    explicit Cursor(passFunc_t passFunc);
    ~Cursor() override;

    int initialize_SDL_process(SDL_Window* window) override;
    void update(float deltaMs) override;
    bool isDone() override;

    void overrideCursor(const std::string& name);
    std::string getCurrentCursor() const;
    SDL_Point getCursorSize() const;

private:
    struct CursorData {
        SDL_Surface* surface;
        SDL_Point size;
        SDL_Point hotspot;
    };

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Surface* fullSheet = nullptr;
    SDL_Cursor* activeCursor = nullptr;

    std::string currentCursor = "default";
    const std::string cursorPath = "../resource/GFX/sprites/cursor.png";

    std::map<std::string, SDL_Rect> cursorRegions;      // Regions defined for cropping
    std::map<std::string, CursorData> cursors;           // Cropped surface per cursor

    void defineCursorRegions();
    void createCursors();
    void cleanup();
};

#endif //CSCI437_CURSOR_H
