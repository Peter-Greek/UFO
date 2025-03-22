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

#include "ChatBox.h"

int ChatBox::initialize_SDL_process(SDL_Window* passed_window) {
    window = passed_window;

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        error("SDL could not initialize!", SDL_GetError());
        return 0;
    }

    // Check window
    if( window == nullptr ) {
        error("Window could not be created!", SDL_GetError());
        return 0;
    }

    // init TTF
    if( TTF_Init() < 0 ) {
        error("Unable to initialize TTF!", TTF_GetError());
        return 0;
    }

    // Create renderer
    renderer = SDL_GetRenderer( window );
    if (renderer == nullptr) {
        renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
        if (renderer == nullptr) {
            error("Unable to create renderer! ", SDL_GetError());
            return 0;
        }
    }

    // Load font
    font = TTF_OpenFont("../resource/Arial.ttf", fontSize);
    if (font == nullptr) {
        error("Unable to open font! ", SDL_GetError());
        return 0;
    }



    running = true;
    // Using Layer 2 for rendering so it is on top of everything else
    AddEventHandler("SDL::OnUpdate::Layer2", [this](float deltaMs) {
        print("ChatBox Update");
        // While application is running
        if (!running) return;
        if (isHidden) {return;}

        if (renderer == nullptr) {return;}

        SDL_SetRenderDrawColor(renderer, 96, 96, 96, 200);
        SDL_RenderFillRect(renderer, &cbox);

        if (messages.empty()) {return;}

        int count = 0;
        int rendered = 0;
        for (int i = static_cast<int>(messages.size()) - 1; i >= 0 && rendered < MAX_VISIBLE_MESSAGES; --i) {
            ChatMessage& message = messages[i];

            // Check if this would be the topmost visible message
            if (rendered == MAX_VISIBLE_MESSAGES - 1 && message.isCut) {
                // Skip rendering a cut-off top fragment
                continue;
            }

            SDL_SetTextureColorMod(message.texture, 255, 255, 255);
            SDL_RenderCopy(renderer, message.texture, nullptr, &message.dst);
            rendered++;
        }
    });

    return 1;
}

void ChatBox::update(float deltaMs) {
    // While application is running
    if (!running) return;
    if (messages.empty()) {return;}
    if (renderer == nullptr) {return;}
    if (isHidden) {return;}
}


void ChatBox::addMessage(const std::string& message) {
    // Create new message texture
    std::string c_message = "> " + message;
    SDL_Texture* txt;
    if (!isHidden && running) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, c_message.c_str(), color);
        txt = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface);
    }
    SDL_Rect pst = {0,0,0,0};
    ChatMessage newMessage = {txt, pst, c_message};
    messages.push_back(newMessage);
    updateMessagesPositioning();
}

void ChatBox::updateMessagesPositioning() {
    int y = cbox.y + cbox.h;
    std::vector<ChatMessage> newMessages;

    for (int i = 0; i < MAX_VISIBLE_MESSAGES; i++) {
        if (i >= messages.size()) break;

        ChatMessage& message = messages[messages.size() - 1 - i];
        if (!message.texture) continue;

        int w, h;
        SDL_QueryTexture(message.texture, nullptr, nullptr, &w, &h);

        // Check if wrapping is needed
        if (w > cbox.w) {
            std::vector<std::string> wrappedLines = wrapText(message.text, cbox.w);
            if (wrappedLines.empty()) continue;

            // Render from bottom to top
            for (int j = static_cast<int>(wrappedLines.size()) - 1; j >= 0; --j) {
                std::string lineText = wrappedLines[j];
                bool isCut = true;

                if (j == 0) {
                    // First line of the wrapped message
//                    lineText = "> " + lineText;
                    isCut = false;
                }

                SDL_Surface* lineSurface = TTF_RenderText_Solid(font, lineText.c_str(), color);
                if (!lineSurface) {
                    error("TTF_RenderText_Solid failed", TTF_GetError());
                    continue;
                }

                SDL_Texture* lineTexture = SDL_CreateTextureFromSurface(renderer, lineSurface);
                SDL_FreeSurface(lineSurface);

                if (!lineTexture) {
                    error("SDL_CreateTextureFromSurface failed", SDL_GetError());
                    continue;
                }

                SDL_QueryTexture(lineTexture, nullptr, nullptr, &w, &h);
                y -= h + MESSAGE_PADDING;

                SDL_Rect dst = {cbox.x, y, w, h};
                newMessages.push_back({lineTexture, dst, lineText, isCut});

                if (newMessages.size() >= MAX_VISIBLE_MESSAGES) break;
            }
        } else {
            y -= h + MESSAGE_PADDING;
            SDL_Rect dst = {cbox.x + cbox.w - w, y, w, h};
            newMessages.push_back({message.texture, dst, message.text, false});
        }

        if (newMessages.size() >= MAX_VISIBLE_MESSAGES) break;
    }

    // Destroy textures for old messages not being reused
    for (auto& msg : messages) {
        if (std::find_if(newMessages.begin(), newMessages.end(),
                         [&](const ChatMessage& m) { return m.texture == msg.texture; }) == newMessages.end()) {
            SDL_DestroyTexture(msg.texture);
        }
    }

    messages = std::vector<ChatMessage>(newMessages.rbegin(), newMessages.rend());
}


std::vector<std::string> ChatBox::wrapText(const std::string& text, int maxWidth) {
    std::vector<std::string> lines;
    std::string currentLine;
    std::istringstream words(text);
    std::string word;

    while (words >> word) {
        std::string testLine = currentLine + (currentLine.empty() ? "" : " ") + word;
        int w, h;
        TTF_SizeText(font, testLine.c_str(), &w, &h);
        if (w > maxWidth) {
            if (!currentLine.empty()) {
                lines.push_back(currentLine);
                currentLine.clear();
            }
            currentLine = word;
        } else {
            currentLine = testLine;
        }
    }
    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }
    return lines;
}

void ChatBox::hideChatBox() {
    isHidden = true;
    if (!running) return;

    for (auto& message : messages) {
        SDL_DestroyTexture(message.texture);
    }
}

void ChatBox::showChatBox() {
    isHidden = false;
    if (!running) return;

    for (auto& message : messages) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, message.text.c_str(), color);
        message.texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface);
    }

    updateMessagesPositioning();
}

void ChatBox::toggleChatBox() {
    if (isHidden) {
        showChatBox();
    }else {
        hideChatBox();
    }
}



void ChatBox::clearMessages() {
    for (auto& message : messages) {
        SDL_DestroyTexture(message.texture);
    }
    messages.clear();
}

void ChatBox::setFontSize(int toFontSize) {
    fontSize = toFontSize;
    font = TTF_OpenFont("../resource/Arial.ttf", fontSize);
    if (font == nullptr) {
        error("Unable to open font! ", SDL_GetError());
    }
    if (!running) return;
    if (isHidden) return;
    for (auto& message : messages) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, message.text.c_str(), color);
        message.texture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_FreeSurface(textSurface);
    }
    updateMessagesPositioning();
}


void ChatBox::updateChatBoxPositioning() {
    cbox = {
            static_cast<int>(position.x),
            static_cast<int>(position.y),
            static_cast<int>(chatBoxSize.x),
            static_cast<int>(chatBoxSize.y)
    };
}

void ChatBox::setChatBoxPosition(vector2 pos) {
    position = pos;
    updateChatBoxPositioning();
}

void ChatBox::setChatBoxPosition(float x, float y) {
    position = {x, y};
    updateChatBoxPositioning();
}

void ChatBox::setChatBoxSize(vector2 size) {
    chatBoxSize = size;
    updateChatBoxPositioning();
}

void ChatBox::setChatBoxSize(float x, float y) {
    chatBoxSize = {x, y};
    updateChatBoxPositioning();
}

void ChatBox::setFontColor(int r, int g, int b, int a) {
    color = {static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a)};
}


