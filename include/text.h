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
// Created by xerxe on 2/6/2025.
//

#ifndef CSCI437_TEXT_H
#define CSCI437_TEXT_H


#include "xProcess.h"
#include <SDL_ttf.h>
class text : public xProcess {
public:
    explicit text(const std::function<void(const std::string& eventName, const json& eventData)>& func, std::string textContentInput) : xProcess(true, func) {
        print("Text Constructor: ", textContentInput);
        textContent = textContentInput;
        setTextRelativePosition(0.0, 0.5);
    }
    explicit text(const std::function<void(const std::string& eventName, const json& eventData)>& func, std::string textContentInput, int toFontSize) : xProcess(true, func) {
        print("Text Constructor: ", textContentInput);
        textContent = textContentInput;
        setTextRelativePosition(0.0, 0.5);
        fontSize = toFontSize;
    }
    ~text() override = default;

    int initialize_SDL_process(SDL_Window* window) override;
    void update(float deltaMs) override;
    bool isDone() override;
    void postSuccess() override;
    void postFail() override;
    void postAbort() override;

    void setText(std::string basicString);
    void setTextPosition(int x, int y);
    void setTextPosition(vector2 pos);
    void setTextRelativePosition(float x, float y);
    void setFontSize(int toFontSize);
    void setCurrentPositionBasedOnRelativePosition();

    void resetRelativePosition();
    std::string getText();
    void hideText();
    void showText();
    void showText(std::string new_text);
private:
    SDL_Event e;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Surface* sText;
    TTF_Font* font;
    SDL_Color color;
    SDL_Rect dst;
    SDL_Point rot;
    bool red=true, green=true, blue=true;
    float angle = 0;
    bool running = false;
    bool isHidden = false;
    std::string textContent;
    vector2 position;
    vector2 relativePosition;
    int fontSize = 50;

    int textWidth;
    int textHeight;


};

#endif //CSCI437_TEXT_H
