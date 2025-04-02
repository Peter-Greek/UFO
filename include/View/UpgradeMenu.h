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
    int AT;

    bool running = false;
    bool isHidden = true;

    SDL_Rect cbox = {
        static_cast<int>(position.x),
        static_cast<int>(position.y),
        static_cast<int>(upgradeMenuSize.x),
        static_cast<int>(upgradeMenuSize.y)
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
    
    SDL_Rect PlayButton = {2*(SCREEN_WIDTH/3), 6*(SCREEN_HEIGHT/7), SCREEN_WIDTH - (10+PlayButton.x), SCREEN_HEIGHT - (10+PlayButton.y)};
    SDL_Rect PlayButtonText = {PlayButton.x+30, PlayButton.y+30, PlayButton.w-60, PlayButton.h-60};
    UpgradeText PlayText = {nullptr, PlayButtonText, "Play"};    
    int fontSize = 50;
public:
    explicit UpgradeMenu(passFunc_t& func) : xProcess(true, func){
        setFontColor(255, 255, 255, 255);
    }
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
    void displayATCount(int ATCount);
    void setATCount(int ATCount);
    void setFontColor(int r, int g, int b, int a);
};
#endif //CSCI437_UPGRADEMENU_H
