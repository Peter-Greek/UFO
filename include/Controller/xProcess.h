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
// Created by xerxe on 2/8/2025.
//

#ifndef CSCI437_XPROCESS_H
#define CSCI437_XPROCESS_H

#include "config.h"
#include "Util.h"

#include <SDL.h>
#include <EventManager.h>
#include <utility>
#include <iostream>
#include <cstdio>

class xProcess : public EventManager {
public:
    enum State {
        UNINITIALIZED,
        RUNNING,
        RUNNING_MANUAL,
        SUCCESS,
        FAIL,
        ABORT
    };
private:
    UUID_t id;
    State state_;
    xProcess* child_;
    bool isSDLProcess = false;
public:
    xProcess(bool isSDL, passFunc_t func) : EventManager(std::move(func)) {
        state_ = UNINITIALIZED;
        child_ = nullptr;
        isSDLProcess = isSDL;
    }
    virtual ~xProcess() = default;

    UUID_t getId() const { return id; }
    void setId(UUID_t UID) { id = UID; }

    bool isSDLSubProcess() const { return isSDLProcess; }

    void initialized() {state_ = RUNNING;}
    virtual int initialize() { state_ = RUNNING; return 1;}
    virtual int initialize_manual() { state_ = RUNNING_MANUAL; return 1;}
    virtual int initialize_SDL_process(SDL_Window* window) { state_ = RUNNING; return 1;}
    virtual void update(float deltaMs) = 0;
    virtual bool isDone() = 0;

    State state() { return state_; }
    bool dead() const { return state_ == SUCCESS || state_ == FAIL || state_ == ABORT; }

    void succeed() { state_ = SUCCESS; }
    void fail() { state_ = FAIL; }
    void abort() { state_ = ABORT; }

    virtual void postSuccess() {}
    virtual void postFail() {}
    virtual void postAbort() {}

    bool hasChild() const { return child_ != nullptr; }
    xProcess* getChild() { return child_; }
    void attachChild(xProcess* child) { child_ = child; }
};


#endif //CSCI437_XPROCESS_H
