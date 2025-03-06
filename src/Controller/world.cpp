//
// Created by xerxe on 3/6/2025.
//

#include "world.h"

int world::initialize() {

    AddEventHandler("SDL::OnUpdate", [this](float deltaMs) {
        if (!gameRunning) {return;}
    });

    gameRunning = true;
    return 1;
}

void world::update(float deltaMs) {

}

void world::loadWorld() {
    worldData.update();
}

void world::updateWorld() {
    worldData.update();
}

void world::saveWorld() {
    worldData.save();
}

jsonLoader world::getWorldData() {
    return worldData;
}
