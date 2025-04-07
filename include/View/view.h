//
// Created by xerxe on 2/6/2025.
//

#ifndef CSCI437_VIEW_H
#define CSCI437_VIEW_H

#include "xProcess.h"
#include "ChatBox.h"
#include "ProcessManager.h"
#include "MainMenu.h"
#include <memory>

class view : public xProcess {
private:
    bool chatState = false;
    bool running = false;
    SDL_Event e;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Surface* screenSurface = nullptr;

    void drawRect(int x, int y, int w, int h);

    std::shared_ptr<ProcessManager> pM;
    passFunc_t& passFunc;

public:
    view(passFunc_t& func, std::shared_ptr<ProcessManager> pPm)
            : xProcess(false, func), pM(std::move(pPm)), passFunc(func) {}

    ~view() override = default;

    SDL_Window* getWindow() { return window; }

    int initialize() override;
    void update(float deltaMs) override;
    bool isDone() override;
    void postSuccess() override;
    void postFail() override;
    void postAbort() override;
};

#endif //CSCI437_VIEW_H
