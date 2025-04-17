#ifndef CSCI437_UPGRADEMENU_H
#define CSCI437_UPGRADEMENU_H

#include "xProcess.h"
#include <SDL_ttf.h>
#include "TxdLoader.h"

class UpgradeMenu : public xProcess {
private:
    enum GAME_RESULT {
        NONE,
        LOSE,
        ESCAPE,
        WIN_NO_ESCAPE,
        WIN_ESCAPE,
        WIN_ESCAPE_TOP_SCORE
    };

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
    int AT = 0;
    int speed = 0;
    int oxygen =0;

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

    SDL_Rect ATBox = {2*(SCREEN_WIDTH/3), 10, SCREEN_WIDTH - (10+ATBox.x), SCREEN_HEIGHT/7};
    SDL_Rect ATBoxText = {ATBox.x+30, ATBox.y+30, ATBox.w-60, ATBox.h-60};
    UpgradeText ATText = {nullptr, ATBoxText, "AT Count: 0"};
    
    SDL_Rect PlayButton = {7*(SCREEN_WIDTH/10), 3*(SCREEN_HEIGHT/4), SCREEN_WIDTH/4, SCREEN_HEIGHT/7};
    SDL_Rect PlayButtonText = {PlayButton.x+30, PlayButton.y+30, PlayButton.w-60, PlayButton.h-60};
    UpgradeText PlayText = {nullptr, PlayButtonText, "Play"};   

    SDL_Rect SpeedButton = {SCREEN_WIDTH/11, SCREEN_HEIGHT/10, SCREEN_WIDTH/5, SCREEN_HEIGHT/7};
    SDL_Rect SpeedButtonText = {SpeedButton.x+30, SpeedButton.y+30, SpeedButton.w-60, SpeedButton.h-60};
    UpgradeText SpeedText = {nullptr, SpeedButtonText, "Play"};   

    SDL_Rect OxygenButton = {SCREEN_WIDTH/11, 3*(SCREEN_HEIGHT/10), SCREEN_WIDTH/5, SCREEN_HEIGHT/7};
    SDL_Rect OxygenButtonText = {OxygenButton.x+30, OxygenButton.y+30, OxygenButton.w-60, OxygenButton.h-60};
    UpgradeText OxygenText = {nullptr, OxygenButtonText, "Play"}; 

    SDL_Rect ShieldButton = {SCREEN_WIDTH/11, SCREEN_HEIGHT/2, SCREEN_WIDTH/4, SCREEN_HEIGHT/6};
    SDL_Rect ShieldButtonText = {ShieldButton.x+30, ShieldButton.y+30, ShieldButton.w-60, ShieldButton.h-60};
    UpgradeText ShieldText = {nullptr, ShieldButtonText, "Play"}; 

    SDL_Rect InvisButton = {2*(SCREEN_WIDTH/5), SCREEN_HEIGHT/2, SCREEN_WIDTH/4, SCREEN_HEIGHT/6};
    SDL_Rect InvisButtonText = {InvisButton.x+30, InvisButton.y+30, InvisButton.w-60, InvisButton.h-60};
    UpgradeText InvisibilityText = {nullptr, InvisButtonText, "Play"}; 

    SDL_Rect CannonButton = {SCREEN_WIDTH/9, 3*(SCREEN_HEIGHT/4), SCREEN_WIDTH/2, SCREEN_HEIGHT/5};
    SDL_Rect CannonButtonText = {CannonButton.x+30, CannonButton.y+30, CannonButton.w-60, CannonButton.h-60};
    UpgradeText CannonText = {nullptr, CannonButtonText, "Play"}; 

    struct UpgradeTracker {
        int height;
        SDL_Rect rects[5] = {
            {SCREEN_WIDTH/3, height, SCREEN_WIDTH/17, SCREEN_HEIGHT/7},  // one
            {SCREEN_WIDTH/3+SCREEN_WIDTH/17, height, SCREEN_WIDTH/17, SCREEN_HEIGHT/7}, // two
            {SCREEN_WIDTH/3+2*SCREEN_WIDTH/17, height, SCREEN_WIDTH/17, SCREEN_HEIGHT/7}, // three
            {SCREEN_WIDTH/3+3*SCREEN_WIDTH/17, height, SCREEN_WIDTH/17, SCREEN_HEIGHT/7}, // four
            {SCREEN_WIDTH/3+4*SCREEN_WIDTH/17, height, SCREEN_WIDTH/17, SCREEN_HEIGHT/7}  // five
        };
    };

    UpgradeTracker SpeedTracker = {SCREEN_HEIGHT/10};
    UpgradeTracker OxygenTracker = {3*(SCREEN_HEIGHT/10)};

    int fontSize = 50;
    sh_ptr<TxdLoader> deathTxd;
    sh_ptr<TxdLoader> escapeTxd;
    sh_ptr<TxdLoader> winTxd;
    sh_ptr<TxdLoader> uMenuTxd;
    sh_ptr<TxdLoader> rocketTxd;


    int gameResult = GAME_RESULT::NONE;
    bool displayingResult = false;

    SDL_Rect srcRect = {1, 1, 1024, 1024}; // load the entire texture, 1 pixel in since there is white line
    SDL_Rect destRect = {
            static_cast<int>(0),
            static_cast<int>(0),
            static_cast<int>(SCREEN_WIDTH),
            static_cast<int>(SCREEN_HEIGHT) // down scale the texture
    };
    SDL_Rect destRect2 = {
            7*(SCREEN_WIDTH/10),
            SCREEN_HEIGHT/5,
            SCREEN_WIDTH/4,
            SCREEN_HEIGHT/2 // down scale the texture
    };

public:
    explicit UpgradeMenu(passFunc_t& func, int res,  sh_ptr<TxdLoader> txd1, sh_ptr<TxdLoader> txd2, sh_ptr<TxdLoader> txd3, sh_ptr<TxdLoader> txd4, sh_ptr<TxdLoader> txd5)
        : xProcess(true, func), gameResult(res), deathTxd(txd1), escapeTxd(txd2), winTxd(txd3), uMenuTxd(std::move(txd4)), rocketTxd(std::move(txd5))
    {
        if (res != GAME_RESULT::NONE) {
            displayingResult = true;
        }
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
    void displayOxygenCount(int oxygen);
    void displaySpeedCount(int speed);
    void setATCount(int ATCount);
    void setOxygenCount(int oxygen);
    void setSpeedCount(int speed);
    void setFontColor(int r, int g, int b, int a);
};
#endif //CSCI437_UPGRADEMENU_H
