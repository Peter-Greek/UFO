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
// Created by xerxe on 3/30/2025.
//

#include "WorldCreator.h"
#include <SDL_image.h>
#include "wall.h"
#include <queue>
#include <set>
#include <fstream>
#include <filesystem>

int WorldCreator::initialize_SDL_process(SDL_Window* pass_window) {
    TriggerEvent("UFO::Chat::AddMessage", "Initializing WorldCreator...");

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        error("IMG_Init failed: ", IMG_GetError());
        return 0;
    }

    RegisterCommand("createRoomFromPng", [this](std::string command, sList_t args, std::string message) {
        if (args.size() != 1) {
            TriggerEvent("UFO::Chat::AddMessage", "Usage: createRoomFromPng <filename>");
            print("Usage: createRoomFromPng <filename>");
            return;
        }

        if (!loadFromPng(args[0])) {
            TriggerEvent("UFO::Chat::AddMessage", "Failed to load image.");
            print("Failed to load image.");
        }
    });

    return 1;
}

std::vector<wall> WorldCreator::detectWallsFromImage(SDL_Surface* surface) {
    std::vector<wall> detectedWalls;
    int w = surface->w, h = surface->h;
    std::vector<std::vector<bool>> visited(h, std::vector<bool>(w, false));

    auto isBlack = [&](int x, int y) -> bool {
        Uint32* pixels = (Uint32*)surface->pixels;
        Uint32 pixel = pixels[(y * surface->w) + x];
        Uint8 r, g, b;
        SDL_GetRGB(pixel, surface->format, &r, &g, &b);
        return (r == 0 && g == 0 && b == 0);
    };

    auto floodFill = [&](int sx, int sy) -> std::vector<vector2> {
        std::vector<vector2> blob;
        std::queue<std::pair<int, int>> q;
        q.push({sx, sy});
        visited[sy][sx] = true;

        while (!q.empty()) {
            auto [x, y] = q.front(); q.pop();
            blob.emplace_back(x, y);

            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    int nx = x + dx, ny = y + dy;
                    if (nx >= 0 && ny >= 0 && nx < w && ny < h && !visited[ny][nx] && isBlack(nx, ny)) {
                        visited[ny][nx] = true;
                        q.push({nx, ny});
                    }
                }
            }
        }
        return blob;
    };

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (!visited[y][x] && isBlack(x, y)) {
                auto pixels = floodFill(x, y);
                if (pixels.empty()) continue;

                // bounding box
                float minX = x, maxX = x, minY = y, maxY = y;
                for (auto& p : pixels) {
                    minX = std::min(minX, p.x);
                    maxX = std::max(maxX, p.x);
                    minY = std::min(minY, p.y);
                    maxY = std::max(maxY, p.y);
                }

                vector2 center(minX, minY);
                int length = static_cast<int>(std::max(maxX - minX, maxY - minY));
                int width = static_cast<int>(std::min(maxX - minX, maxY - minY));

                float angle = atan2(maxY - minY, maxX - minX) * 180.0f / M_PI;
                if (angle < 0) angle += 360.0f;

                detectedWalls.emplace_back(center, length, width, Heading(angle));
            }
        }
    }

    return detectedWalls;
}

json WorldCreator::generateRoomJson(const std::string& name, const std::string& id, const std::vector<wall>& walls) {
    json room;
    room["id"] = id;
    room["name"] = name;
    room["type"] = "room";
    room["walls"] = json::array();

    for (const auto& w : walls) {
        json wall;
        wall["center"] = w.position;
        wall["length"] = w.length;
        wall["width"] = w.width;
        wall["heading"] = w.heading.get();
        room["walls"].push_back(wall);
    }

    room["doors"] = json::array();
    room["entities"] = json::array();

    // Simple center: average of wall centers
    vector2 center(0, 0);
    for (const auto& w : walls) center += w.position;
    if (!walls.empty()) center = center / static_cast<float>(walls.size());

    room["center"] = center;
    return room;
}





int WorldCreator::loadFromPng(std::string fileName) {
    std::string imagePath = "../resource/" + fileName;
    SDL_Surface* surface = IMG_Load(imagePath.c_str());
    if (!surface) {
        error("Failed to load image: ", IMG_GetError());
        return 0;
    }

    auto walls = detectWallsFromImage(surface);
    SDL_FreeSurface(surface);

    json roomJson = generateRoomJson("Generated Room", "generated_room", walls);

    // Save to temp.json
    std::string outputPath = "temp.json";
    std::ofstream out(outputPath);
    out << std::setw(4) << roomJson << std::endl;
    out.close();

    // Copy to clipboard
//    SDL_SetClipboardText(roomJson.dump(2).c_str());
    print(roomJson.dump(2).c_str());

    print("Room JSON copied to clipboard and written to temp.json");
    TriggerEvent("UFO::Chat::AddMessage", "Room JSON copied to clipboard and written to temp.json");
    return 1;
}

void WorldCreator::postSuccess() {
    print("WorldCreator finished successfully.");
}

void WorldCreator::postFail() {
    print("WorldCreator failed.");
}