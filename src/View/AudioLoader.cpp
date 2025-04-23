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
static bool SDL_AUDIO_INITIALIZED = false;

int AudioLoader::initialize_SDL_process(SDL_Window* window) {
    print("[AudioLoader] Initializing SDL audio for: ", audioPath);
    if (!SDL_AUDIO_INITIALIZED) {
        if (SDL_Init(SDL_INIT_AUDIO) < 0) {
            error("SDL audio could not initialize!", SDL_GetError());
            return 0;
        }

        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            error("SDL_mixer could not initialize!", Mix_GetError());
            return 0;
        }

        SDL_AUDIO_INITIALIZED = true;
        print("[AudioLoader] SDL Audio initialized");
    }

    if (!isMusic) {
        chunk = Mix_LoadWAV(audioPath.c_str());
        if (!chunk) {
            error("Failed to load sound effect!", Mix_GetError());
            return 0;
        }
    }else {
        music = Mix_LoadMUS(audioPath.c_str());
        if (!music) {
            error("Failed to load music!", Mix_GetError());
            return 0;
        }
    }

    for (auto& track : layeredTracks) {
        if (track.chunk == nullptr) {
            Mix_Chunk* ck = Mix_LoadWAV(track.path.c_str());
            if (!ck) {
                error("Failed to load layered track:", track.path, Mix_GetError());
                continue;
            }
            track.chunk = ck;
            print("[AudioLoader] Attached track (deferred):", track.path);
        }
    }
    pendingTrackPaths.clear();

    running = true;

    if (isPlaying) {
        play();
    }

    AddEventHandler("UFO::OnConfigUpdate", [this](const std::string configName) {
        if (configName == "AUDIO_ENABLED") {
            refreshVolume();
        }else if (configName == "VOLUME_MUSIC") {
            if (isMusic) {
                print("[AudioLoader] Attempt Music Volume changed:", configName, VOLUME_MUSIC);
                setVolume(VOLUME_MUSIC / 100);
            }
        }else if (configName == "VOLUME_SFX") {
            if (!isMusic) {
                setVolume(VOLUME_SFX / 100);
            }
        }
    });

    return 1;
}

void AudioLoader::update(float deltaMs) {
    // No real-time updates needed unless you add looping or fade-outs.
}

bool AudioLoader::isDone() {
    return !running;
}

void AudioLoader::play(float vol) {
    setVolume(vol);
    play();
}

void AudioLoader::play() {
    isPlaying = true;
    if (!running) {
        print("AudioLoader Not Init; Playing next frame!");
        return;
    }

    print("[AudioLoader] Playing audio:", audioPath);

    refreshVolume();

    // Play main music or sound effect
    if (isMusic && music) {
        if (Mix_FadeInMusic(music, -1, 1000) == -1) {
            error("Mix_FadeInMusic failed:", Mix_GetError());
        } else {
            print("  - Music Volume (0-128):", volume);
            print("  - Music Playing:", Mix_PlayingMusic() ? "Yes" : "No");
        }
    } else if (chunk) {
        channel = Mix_PlayChannel(-1, chunk, 0);
        if (channel == -1) {
            error("Mix_PlayChannel failed:", Mix_GetError());
        } else {
            print("  - Chunk Volume (0-128):", volume);
            print("  - Channel:", channel);
            print("  - Playing:", Mix_Playing(channel) ? "Yes" : "No");
        }
    }

    // Loop through all attached tracks and start them muted (or at correct volume if enabled)
    for (auto& track : layeredTracks) {
        if (!track.chunk) continue;

        if (track.channel == -1 || !Mix_Playing(track.channel)) {
            track.channel = Mix_PlayChannel(-1, track.chunk, -1); // Loop
        }

        int volToApply = track.enabled ? track.volume : 0;
        if (!AUDIO_ENABLED) {
            volToApply = 0; // Mute if audio is disabled
        }


        Mix_Volume(track.channel, volToApply);

        print("[Layered Track] ", track.path);
        print("  - Channel:", track.channel);
        print("  - Volume:", volToApply);
        print("  - Playing:", Mix_Playing(track.channel) ? "Yes" : "No");
    }
}

void AudioLoader::stop() {
    if (!running || !isPlaying) return;

    print("[AudioLoader] Stopping audio:", audioPath);

    if (isMusic && music) {
        Mix_HaltMusic();
    } else if (chunk && channel != -1) {
        Mix_HaltChannel(channel);
    }

    // Stop sub-tracks explicitly
    for (auto& track : layeredTracks) {
        if (track.channel != -1) {
            Mix_HaltChannel(track.channel);
            track.channel = -1;
        }
        track.enabled = false;
    }

    isPlaying = false;
}

void AudioLoader::setVolume(float vol) {
    vol = std::clamp(vol, 0.0f, 1.0f); // Ensure volume is between 0 and 1
    int sdlVolume = static_cast<int>(vol * MIX_MAX_VOLUME);
    volume = sdlVolume;
    refreshVolume();
}

void AudioLoader::setVolume(int vol) {
    vol = std::clamp(vol, 0, MIX_MAX_VOLUME); // Ensure volume is between 0 and 1
    volume = vol;
    refreshVolume();
}

void AudioLoader::refreshVolume() {
    if (!AUDIO_ENABLED) {
        Mix_VolumeMusic(0);
        Mix_VolumeChunk(chunk, 0);
        for (auto& track : layeredTracks) {
            if (!track.chunk) continue;
            if (track.channel != -1) {
                Mix_Volume(track.channel, 0);
            }
        }
        return;
    }

    if (isPlaying) {
        if (isMusic && music) {
            Mix_VolumeMusic(volume);
        } else if (chunk) {
            Mix_VolumeChunk(chunk, volume);
        }

        for (auto& track : layeredTracks) {
            if (!track.chunk) continue;
            if (track.channel != -1) {
                Mix_Volume(track.channel, track.enabled ? track.volume : 0);
            }
        }
    }
}




double AudioLoader::getPlaybackTime() {
    if (isMusic && music) {
        return Mix_GetMusicPosition(music);
    }
    return 0.0;
}

AudioLoader::~AudioLoader() {
    if (chunk) Mix_FreeChunk(chunk);
    if (music) Mix_FreeMusic(music);
    for (auto& t : layeredTracks) {
        if (t.chunk) Mix_FreeChunk(t.chunk);
    }
//    Mix_CloseAudio();
}

void AudioLoader::attachTrack(const std::string& path) {
    if (!running) {
        layeredTracks.push_back({path, nullptr, -1, MIX_MAX_VOLUME, false});
        return;
    }
    Mix_Chunk* newChunk = Mix_LoadWAV(path.c_str());
    if (!newChunk) {
        error("Failed to load layered track:", path, Mix_GetError());
        return;
    }
    layeredTracks.push_back({path, newChunk, -1, MIX_MAX_VOLUME, false});
    print("[AudioLoader] Attached track:", path);
}

void AudioLoader::enableTrack(int index) {
    if (index >= layeredTracks.size()) return;
    auto& track = layeredTracks[index];
    track.enabled = true;
    if (track.channel != -1) {
        if (!AUDIO_ENABLED) {Mix_Volume(track.channel, 0); return;};
        Mix_Volume(track.channel, track.volume);
    }
}

void AudioLoader::disableTrack(int index) {
    if (index >= layeredTracks.size()) return;
    auto& track = layeredTracks[index];
    track.enabled = false;
    if (track.channel != -1) {
        Mix_Volume(track.channel, 0); // mute, don't halt
    }
}

void AudioLoader::setTrackVolume(int index, float vol) {
    if (index >= layeredTracks.size()) return;
    print("[AudioLoader] Setting track volume:", index, vol);
    int volInt = static_cast<int>(std::clamp(vol, 0.0f, 1.0f) * MIX_MAX_VOLUME);
    layeredTracks[index].volume = volInt;
    if (layeredTracks[index].channel != -1) {
        if (!AUDIO_ENABLED) {volInt = 0;};
        Mix_Volume(layeredTracks[index].channel, volInt);
    }
}

void AudioLoader::stopAllTracks() {
    for (auto& track : layeredTracks) {
        if (track.channel != -1) {
            Mix_HaltChannel(track.channel);
            track.channel = -1;
            track.enabled = false;
        }
    }
}