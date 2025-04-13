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
// Created by xerxe on 3/28/2025.
//

#ifndef CSCI437_SCHEDULER_H
#define CSCI437_SCHEDULER_H

#include "Util.h"
#include <SDL.h>
#include <chrono>
#include <thread>
#include <mutex>
#include <cmath>


/*
 * Based on techniques and explanations from:
 * "How to Make Your Game Run at 60fps"
 * Author: Tyler Glaiel
 * Source: https://medium.com/@tglaiel/how-to-make-your-game-run-at-60fps-24c61210fe75
 * Used for the run and wait methods.
 * This scheduler implementation borrows concepts like delta time averaging,
 * VSync snap correction, and spiral-of-death protection from the article above.
 */



using high_res_time_point_t = std::chrono::time_point<std::chrono::high_resolution_clock>;

class Scheduler {
private:
    bool running = true;
    float timeFactor = 1.0f;
    float gameTime = 0;
    high_res_time_point_t startTime = std::chrono::high_resolution_clock::now();
    std::mutex m;

    // Timing config
    double updateRate = static_cast<double>(targetFPS);
    double fixedDelta = 1.0 / updateRate;
    bool unlockFramerate = false;

    // SDL timing
    Uint64 clocksPerSecond = SDL_GetPerformanceFrequency();
    Uint64 desiredFrameTime = clocksPerSecond / updateRate;
    Uint64 vsyncMaxError = clocksPerSecond * 0.0002;
    Uint64 snapFrequencies[8] = {};

    // Averaging
    const int timeHistoryCount = 4;
    Uint64 timeAverager[4];
    Uint64 averagerResidual = 0;

    // Frame timing
    Uint64 prevFrameTime = SDL_GetPerformanceCounter();
    Uint64 frameAccumulator = 0;
    bool resync = true;
    Uint64 frameStartTime = 0;

public:
    Scheduler() {
        SDL_DisplayMode mode;
        int displayHz = (SDL_GetCurrentDisplayMode(0, &mode) == 0 && mode.refresh_rate > 0) ? mode.refresh_rate : 60;

        for (int i = 0; i < 8; ++i) {
            snapFrequencies[i] = (clocksPerSecond / displayHz) * (i + 1);
        }

        for (int i = 0; i < timeHistoryCount; ++i) {
            timeAverager[i] = desiredFrameTime;
        }
    }

    float elapsedTime() {
        auto curTime = std::chrono::high_resolution_clock::now();
        float diff = std::chrono::duration<float, std::chrono::milliseconds::period>(curTime - startTime).count();
        float timeElapsed = diff - gameTime;
        gameTime = diff;
        return timeElapsed;
    }

    [[nodiscard]] float getGameTime() const {
        return gameTime;
    };

    high_res_time_point_t getStartTime() {
        return startTime;
    }

    float run() {
        updateRate = static_cast<double>(targetFPS);
        fixedDelta = 1.0 / updateRate;
        desiredFrameTime = clocksPerSecond / updateRate;

        unlockFramerate = unlimitedFrames;
        elapsedTime();
        frameStartTime = SDL_GetPerformanceCounter();
        Uint64 deltaClocks = frameStartTime - prevFrameTime;
        prevFrameTime = frameStartTime;

        if (deltaClocks > desiredFrameTime * 8) deltaClocks = desiredFrameTime;
        if (deltaClocks < 0) deltaClocks = 0;

        for (Uint64 snap : snapFrequencies) {
            if ((deltaClocks > snap ? deltaClocks - snap : snap - deltaClocks) < vsyncMaxError) {
                deltaClocks = snap;
                break;
            }
        }

        for (int i = 0; i < timeHistoryCount - 1; ++i) {
            timeAverager[i] = timeAverager[i + 1];
        }
        timeAverager[timeHistoryCount - 1] = deltaClocks;

        Uint64 sum = 0;
        for (int i = 0; i < timeHistoryCount; ++i) sum += timeAverager[i];
        deltaClocks = sum / timeHistoryCount;
        averagerResidual += sum % timeHistoryCount;
        deltaClocks += averagerResidual / timeHistoryCount;
        averagerResidual %= timeHistoryCount;

        frameAccumulator += deltaClocks;
        if (frameAccumulator > desiredFrameTime * 8) resync = true;

        if (resync) {
            frameAccumulator = 0;
            deltaClocks = desiredFrameTime;
            resync = false;
        }

        float deltaMs = static_cast<float>(deltaClocks) * 1000.0f / clocksPerSecond;
        deltaMs *= timeFactor;
        return std::max(0.001f, deltaMs);
    }

    void wait() const {
        if (unlockFramerate) return;

        Uint64 frameEndTime = SDL_GetPerformanceCounter();
        Uint64 elapsed = frameEndTime - frameStartTime;
        float frameMs = static_cast<float>(elapsed) * 1000.0f / clocksPerSecond;

        float waitTime = (1000.0f / updateRate) - frameMs;
        if (waitTime > 1.0f) {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(waitTime - 1.0f)));
        }

        while (SDL_GetPerformanceCounter() - frameStartTime < desiredFrameTime) {}
    }

    void shutdown() { running = false; }
    bool isRunning() const { return running; }

    void setTimeFactor(float factor) { timeFactor = factor; }
    float getTimeFactor() const { return timeFactor; }

    void setUnlockFramerate(bool v) { unlockFramerate = v; }
    void setTargetFPS(int fps) {
        updateRate = static_cast<double>(fps);
        fixedDelta = 1.0 / updateRate;
        desiredFrameTime = clocksPerSecond / updateRate;
    }

    void setTimeout(int delay, std::function<void()> function) {
        std::thread t([delay, function, this]() {
            // mutex to lock the thread
            std::lock_guard<std::mutex> lock(m);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            function();
        });
        t.detach();
    }
};


#endif //CSCI437_SCHEDULER_H
