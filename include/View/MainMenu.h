#ifndef CSCI437_MAINMENU_H
#define CSCI437_MAINMENU_H

#include "xProcess.h"
#include <SDL_ttf.h>
class MainMenu : public xProcess {
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
    vector2 mainMenuSize = {static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT)};
    int AT;

    bool running = false;
    bool isHidden = true;

    SDL_Rect cbox = {
        static_cast<int>(position.x),
        static_cast<int>(position.y),
        static_cast<int>(mainMenuSize.x),
        static_cast<int>(mainMenuSize.y)
    };

    struct UpgradeText {
        SDL_Texture* texture;
        SDL_Rect dst;
        std::string text;
        bool isCut = false;
    };

    SDL_Rect ATBox = {cbox.x + 2*(SCREEN_WIDTH/3), cbox.y+ 10, SCREEN_WIDTH - (10+ATBox.x), SCREEN_HEIGHT/7};
    SDL_Rect ATBoxText = {ATBox.x+30, ATBox.y+30, ATBox.w-60, ATBox.h-60};
    UpgradeText ATText = {nullptr, ATBoxText, "AT Count: 0"};
    int fontSize = 50;
public:
    explicit MainMenu(passFunc_t& func) : xProcess(true, func){
        setFontColor(255, 255, 255, 255);
    }
    ~MainMenu() override = default;

    int initialize_SDL_process(SDL_Window* window) override;
    void update(float deltaMs) override;
    bool isDone() override{return !running;};
    void showMainMenu();
    void closeMainMenu();
    void setMainMenuPosition(vector2 pos);
    void setMainMenuPosition(float x, float y);
    void setMainMenuSize(vector2 size);
    void setMainMenuSize(float x, float y);
    void updateMainMenuPositioning();
    void displayATCount(int ATCount);
    void setATCount(int ATCount);
    void setFontColor(int r, int g, int b, int a);
};
#endif //CSCI437_MAINMENU_H
