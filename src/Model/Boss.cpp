//
// Created by xerxe on 3/28/2025.
//

#include "Boss.h"


void Boss::update(float deltaMs) {

}

bool Boss::inRageMode() const {
    return inRage > 0;
}

bool Boss::isMinion(entity* e) const {
    return std::find(minions.begin(), minions.end(), e) != minions.end();
}

bool Boss::isBossProjectile(entity* e) const {
    return std::find(projectiles.begin(), projectiles.end(), e) != projectiles.end();
}

void Boss::attachMinion(entity* e) {
    if (minionCount < maxMinions) {
        minions.push_back(e);
        minionCount++;
    }
}

void Boss::attachProjectile(entity* e) {
    if (projectileCount < maxProjectiles) {
        projectiles.push_back(e);
        projectileCount++;
    }
}

void Boss::removeMinion(entity* e) {
    auto it = std::find(minions.begin(), minions.end(), e);
    if (it != minions.end()) {
        minions.erase(it);
        minionCount--;
    }
}

void Boss::removeProjectile(entity* e) {
    auto it = std::find(projectiles.begin(), projectiles.end(), e);
    if (it != projectiles.end()) {
        projectiles.erase(it);
        projectileCount--;
    }
}

int Boss::getToSpawnMinionCount() const {
    int minionToSpawn = maxMinions - minionCount;
    return minionToSpawn;
}

bool Boss::canSpawnMinion() {
    // if the time is more than the interval and the count is less than
    // the max, then spawn a minion
    if (lastMinionSpawnTime > timeBetweenSpawns && minionCount < maxMinions) {
        return true;
    }
    return false;
}

// Gamemanager is going to calculate where the minion should spawn at and then call this function
entity* Boss::spawnMinion(vector2 coords) {
    if (minionCount < maxMinions) {
        auto* e = new entity(passFunc, entity::ENEMY, 3, coords);
        attachMinion(e);
        return e;
    }
    return nullptr;
}






