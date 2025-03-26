#ifndef CSCI437_UPGRADEMENU_H
#define CSCI437_UPGRADEMENU_H

#include "xProcess.h"
#include <SDL_ttf.h>
class UpgradeMenu : public xProcess {
private:
    SDL_Event e;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Surface* sText;
    TTF_Font* font;
    SDL_Color color;
    SDL_Point rot;
    vector2 position = {0, 0};
    vector2 upgradeMenuSize = {static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT)};

    bool running = false;
    bool isHidden = true;

    SDL_Rect cbox = {
        static_cast<int>(position.x),
        static_cast<int>(position.y),
        static_cast<int>(upgradeMenuSize.x),
        static_cast<int>(upgradeMenuSize.y)
    };
    int fontSize = 50;

public:
    explicit UpgradeMenu(passFunc_t& func) : xProcess(true, func){}
    ~UpgradeMenu() override = default;

    int initialize_SDL_process(SDL_Window* window) override;
    void update(float deltaMs) override;
    bool isDone() override{return !running;};
    void showUpgradeMenu();
    void closeUpgradeMenu();
    void setUpgradeMenuPosition(vector2 pos);
    void setUpgradeMenuPosition(float x, float y);
    void setUpgradeMenuSize(vector2 size);
    void setUpgradeMenuSize(float x, float y);
    void updateUpgradeMenuPositioning();

};
#endif //CSCI437_UPGRADEMENU_H
