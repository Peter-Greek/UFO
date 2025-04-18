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

#ifndef CSCI437_AUDIOLOADER_H
#define CSCI437_AUDIOLOADER_H


#include <SDL.h>
#include <SDL_mixer.h>
#include <string>
#include <unordered_map>
#include <utility>
#include "xProcess.h"
#include <vector>

class AudioLoader : public xProcess {
private:
    std::string audioPath;
    Mix_Chunk* chunk = nullptr;
    Mix_Music* music = nullptr;

    int volume = static_cast<int>(0.5 * MIX_MAX_VOLUME); // Default volume
    int channel = -1;

    bool isMusic = false;
    bool running = false;
    bool isPlaying = false;

    struct AudioTrack {
        std::string path;
        Mix_Chunk* chunk = nullptr;
        int channel = -1;
        int volume = MIX_MAX_VOLUME;
        bool enabled = false;
    };

    std::vector<AudioTrack> layeredTracks;
    std::vector<std::string> pendingTrackPaths;

public:
    explicit AudioLoader(passFunc_t passFunc, std::string path, bool isMusic_p = false)
            : xProcess(true, std::move(passFunc)), audioPath(std::move(path)), isMusic(isMusic_p) {}
    ~AudioLoader() override;

    int initialize_SDL_process(SDL_Window* window) override;
    void update(float deltaMs) override;
    bool isDone() override;

    void play(float volume);
    void play();
    void stop();
    void setVolume(float vol);
    double getPlaybackTime();

    void attachTrack(const std::string& path);
    void enableTrack(int index);
    void disableTrack(int index);
    void setTrackVolume(int index, float vol);
    void stopAllTracks();

    void setVolume(int vol);

    void refreshVolume();
};


#endif //CSCI437_AUDIOLOADER_H
