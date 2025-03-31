//
// Created by xerxe on 3/28/2025.
//

#include "Boss.h"



void Boss::update(float deltaMs) {
    lastMinionSpawnTime += deltaMs;
    if (lastMinionSpawnTime > timeBetweenSpawnsMinion) {
        lastMinionSpawnTime = timeBetweenSpawnsMinion;
    }

    lastProjectileSpawnTime += deltaMs;
    if (lastProjectileSpawnTime > timeBetweenSpawnsProjectile) {
        lastProjectileSpawnTime = timeBetweenSpawnsProjectile;
    }

    std::vector<sh_ptr_e> removalList; // prevent segfaults
    for (auto& m : minions) {
        if (m->isDone() || m->state() == xProcess::State::ABORT || m->state() == xProcess::State::FAIL) {
            removalList.push_back(m);
        }
    }

    for (auto& p : projectiles) {
        if (p->isDone() || p->state() == xProcess::State::ABORT || p->state() == xProcess::State::FAIL) {
            removalList.push_back(p);
        }
    }

    for (auto& e : removalList) {
        if (e->isEntityAProjectile()) {
            removeProjectile(e);
        }else if (e->isEntityAnEnemy()) {
            removeMinion(e);
        }
    }

    if (getHearts() < getMaxHearts() / 2) {
        inRage += deltaMs;
    } else {
        inRage = 0;
    }
}

bool Boss::inRageMode() const {
    // I am thinking in rage mode the boss will shoot projectiles faster and spawn minions faster
    // also have it shoot projectiles in all directions (maybe make the projectiles slower so they stay on the screen longer)

    return inRage > 0;
}

bool Boss::isMinion(const sh_ptr_e& e) const {
    return std::find(minions.begin(), minions.end(), e) != minions.end();
}

bool Boss::isBossProjectile(const sh_ptr_e& e) const {
    return std::find(projectiles.begin(), projectiles.end(), e) != projectiles.end();
}

void Boss::attachMinion(const sh_ptr_e& e) {
    if (minionCount < maxMinions) {
        minions.push_back(e);
        minionCount++;
        lastMinionSpawnTime = 0;
    }
}

void Boss::attachProjectile(const sh_ptr_e& e) {
    if (projectileCount < maxProjectiles) {
        projectiles.push_back(e);
        projectileCount++;
        lastProjectileSpawnTime = 0;
    }
}

void Boss::removeMinion(const sh_ptr_e& e) {
    auto it = std::find(minions.begin(), minions.end(), e);
    if (it != minions.end()) {
        minions.erase(it);
        minionCount--;
    }
}

void Boss::removeProjectile(const sh_ptr_e& e) {
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

bool Boss::canSpawnMinion() const {
    // if the time is more than the interval and the count is less than
    // the max, then spawn a minion
    if (lastMinionSpawnTime >= timeBetweenSpawnsMinion && minionCount < maxMinions) {
        return true;
    }
    return false;
}

// Gamemanager is going to calculate where the minion should spawn at and then call this function
sh_ptr_e Boss::spawnMinion(vector2 coords) {
    if (minionCount < maxMinions) {
        auto e = std::make_shared<entity>(passFunc, entity::ENEMY, 3, coords);
        attachMinion(e);
        return e;
    }
    return nullptr;
}

bool Boss::isProjectile(const sh_ptr_e& e) const {
    return std::find(projectiles.begin(), projectiles.end(), e) != projectiles.end();
}

int Boss::getToSpawnProjectileCount() const {
    int projectileToSpawn = maxProjectiles - projectileCount;
    return projectileToSpawn;
}

bool Boss::canSpawnProjectile() const {
    return lastProjectileSpawnTime >= (inRageMode() ? timeBetweenSpawnsProjectile/3 : timeBetweenSpawnsProjectile) && projectileCount < maxProjectiles;
}

sh_ptr_e Boss::spawnProjectile(vector2 coords) {
    if (projectileCount < maxProjectiles) {
        // shared pointer to self to pass as the owner of the projectile
        std::shared_ptr<entity> b = shared_from_this(); // Safe shared_ptr to this
        auto e = std::make_shared<Projectile>(passFunc, coords, b);
        attachProjectile(e);
        return e;
    }
    return nullptr;
}



