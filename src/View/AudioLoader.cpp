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
// Created by xerxe on 3/22/2025.
//

#include "AudioLoader.h"

int AudioLoader::initialize_SDL_process(SDL_Window* window) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        error("SDL audio could not initialize!", SDL_GetError());
        return 0;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        error("SDL_mixer could not initialize!", Mix_GetError());
        return 0;
    }

    chunk = Mix_LoadWAV(audioPath.c_str());
    if (!chunk) {
        error("Failed to load sound effect!", Mix_GetError());
        return 0;
    }

    running = true;
    return 1;
}

void AudioLoader::update(float deltaMs) {
    // No real-time updates needed unless you add looping or fade-outs.
}

bool AudioLoader::isDone() {
    return !running;
}

void AudioLoader::play(float volume) {
    if (chunk) {
        int sdlVolume = static_cast<int>(volume * MIX_MAX_VOLUME);
        Mix_VolumeChunk(chunk, sdlVolume);
        Mix_PlayChannel(-1, chunk, 0);
    }
}

