//
// Created by xerxe on 2/6/2025.
//

#ifndef CSCI437_VIEW_H
#define CSCI437_VIEW_H

#include "xProcess.h"
class view : public xProcess {
public:
    explicit view(passFunc_t& func) : xProcess(false, func) {};
    ~view() override = default;

    SDL_Window* getWindow() { return window; }

    int initialize() override;
    void update(float deltaMs) override;
    bool isDone() override;
    void postSuccess() override;
    void postFail() override;
    void postAbort() override;
private:
    bool chatState = false;
    bool running = false;
    SDL_Event e;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* screenSurface;

    void drawRect(int x, int y, int w, int h);
};


#endif //CSCI437_VIEW_H
