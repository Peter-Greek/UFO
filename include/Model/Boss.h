//
// Created by xerxe on 3/28/2025.
//

#ifndef CSCI437_BOSS_H
#define CSCI437_BOSS_H

#include "entity.h"
class Boss : public entity {
private:
    int timeBetweenSpawns = 15000;
    int lastMinionSpawnTime = 0;
    int inRage = 0;
    int rageTime = 0;
    std::vector<entity*> minions;
    std::vector<entity*> projectiles;
    int minionCount = 0;
    int projectileCount = 0;
    int maxMinions = 5;
    int maxProjectiles = 5;
    passFunc_t passFunc;
public:
    explicit Boss(
            passFunc_t& func,
            vector2 position
    ) : entity(func, entity::ENEMY_BOSS, 15, position, 128, 128), passFunc(func) {};
    void update(float deltaMs) override;

    [[nodiscard]] bool inRageMode() const;
    bool isMinion(entity* e) const;
    bool isBossProjectile(entity* e) const;
    void attachMinion(entity* e);
    void attachProjectile(entity* e);
    void removeMinion(entity* e);
    void removeProjectile(entity* e);
    [[nodiscard]] int getToSpawnMinionCount() const;
    bool canSpawnMinion() const;
    entity* spawnMinion(vector2 coords);
};


#endif //CSCI437_BOSS_H
