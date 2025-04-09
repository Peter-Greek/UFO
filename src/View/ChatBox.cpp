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
#include <sstream>

void ChatBox::Init() {

    print("Chatbox Initialize");
    AddEventHandler("SDL::OnPollEvent", [this](int eventType, int key) {
        if (isDone()) {return;}
        if (isHiddenChatBox()) {
            if (eventType == SDL_KEYUP) {
                if (key == SDLK_t) {
                    toggleChatBox();
                }
            }
        }else {
            if (eventType == SDL_KEYDOWN) {
                if (key == SDLK_ESCAPE) {
                    toggleChatBox();
                }
            }
        }
    });

    AddEventHandler("__internal_chat_register_command", [this](std::string command) {
        print("Registering command: ", command);
        commands.push_back(command);
    });

    AddEventHandler("UFO::Chat::AddMessage", [this](std::string message) {
        if (isDone()) {return;}
        addMessage(message);
    });

    return;
}

int ChatBox::onInputMessage(std::string message) {
    if (isDone()) {return 1;}
    sList_t args = split(message, " ");
    std::string command = args[0];
    if (std::find(commands.begin(), commands.end(), command) == commands.end()) {
        UserInput::addMessage("Unknown/Unregistered command: " + command);
        return 1;
    }
    args.erase(args.begin());
    TriggerEvent("__internal_command_" + command, "chat", args, message);
    UserInput::addMessage("Executed Command: " + command);
    return 1;
}

void ChatBox::addMessage(const std::string& message) {
    if (isDone()) {return;}
    UserInput::addMessage(message);
    TriggerEvent("UFO::Chat::MessageAdded", message);
}

void ChatBox::onStateChange(bool state) {
    TriggerEvent("UFO::Chat::State", state);
}