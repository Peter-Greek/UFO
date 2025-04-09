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

#include "UserInput.h"
#include <SDL_ttf.h>
class ChatBox : public UserInput {
private:
    sList_t commands;
public:
    explicit ChatBox(passFunc_t func) : UserInput(func) {
        Init();
    }
    ~ChatBox() override = default;

    void Init();
    void addMessage(const std::string& message) override;

    std::pair<std::vector<ChatMessage>, sList_t> getMessagesAndCommands() {
        return {UserInput::getMessages(), commands};
    }
    void setMessagesAndCommands(std::pair<std::vector<ChatMessage>, sList_t> messagesAndCommands) {
        if (messagesAndCommands.first.empty()) {
            return;
        }
        UserInput::setMessages(messagesAndCommands.first);
        commands = messagesAndCommands.second;
    }

    int onInputMessage(std::string message) override;
    void onStateChange(bool state) override;
};


#endif //CSCI437_CHATBOX_H
