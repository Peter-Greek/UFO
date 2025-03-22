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

//
// Created by xerxe on 3/21/2025.
//

#ifndef CSCI437_CHATBOX_H
#define CSCI437_CHATBOX_H

#include "xProcess.h"
#include <SDL_ttf.h>
class ChatBox : public xProcess {
private:
    SDL_Event e;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Surface* sText;
    TTF_Font* font;
    SDL_Color color;
    SDL_Point rot;
    vector2 position = {50, 50};
    vector2 chatBoxSize = {static_cast<float>(SCREEN_WIDTH - 100), static_cast<float>(SCREEN_HEIGHT - 100)};

    bool running = false;
    bool isHidden = true;

    SDL_Rect cbox = {
        static_cast<int>(position.x),
        static_cast<int>(position.y),
        static_cast<int>(chatBoxSize.x),
        static_cast<int>(chatBoxSize.y)
    };

    struct ChatMessage {
        SDL_Texture* texture;
        SDL_Rect dst;
        std::string text;
        bool isCut = false;
    };
    // list of messages
    std::vector<ChatMessage> messages;
    static const int MAX_VISIBLE_MESSAGES = 8;
    static const int MESSAGE_PADDING = 5;



    int fontSize = 50;
    int textWidth;
    int textHeight;

    int CHAT_INPUT_HEIGHT = fontSize * 2 + MESSAGE_PADDING;
    SDL_Rect inputBox = {cbox.x + 10, cbox.y + cbox.h - CHAT_INPUT_HEIGHT, cbox.w - 20, CHAT_INPUT_HEIGHT - 20};
    SDL_Rect inputBoxText = {inputBox.x + inputBox.w - 50, inputBox.y + 15, 0, 0};
    ChatMessage inputMessage = {nullptr, inputBoxText, "> "};
public:
    explicit ChatBox(passFunc_t& func) : xProcess(true, func) {
        setFontColor(255, 255, 255, 255);
    }
    ~ChatBox() override = default;

    int initialize_SDL_process(SDL_Window* window) override;
    void update(float deltaMs) override;
    bool isDone() override{return !running;};
    void postSuccess() override{};
    void postFail() override{};
    void postAbort() override{};

    void updateMessagesPositioning();
    void addMessage(const std::string& message);
    void clearMessages();

    void hideChatBox();
    void showChatBox();
    void toggleChatBox();
    [[nodiscard]] bool isHiddenChatBox() const { return isHidden; }

    void setChatBoxPosition(vector2 pos);
    void setChatBoxPosition(float x, float y);
    void setChatBoxSize(vector2 size);
    void setChatBoxSize(float x, float y);
    void updateChatBoxPositioning();

    void setFontSize(int toFontSize);
    void setFontColor(int r, int g, int b, int a);


    std::vector<std::string> wrapText(const std::string &text, int maxWidth);

    void createInputMessage();

    void hideInputMessage();
};


#endif //CSCI437_CHATBOX_H
