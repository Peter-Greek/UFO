//
// Created by xerxe on 3/28/2025.
//

#ifndef CSCI437_BOSS_H
#define CSCI437_BOSS_H

#include "entity.h"
#include "Projectile.h"

class Boss : public entity, public std::enable_shared_from_this<Boss> {
private:
    int timeBetweenSpawnsMinion = 15000;
    int lastMinionSpawnTime = 0;
    int inRage = 0;
    int rageTime = 0;
    std::vector<sh_ptr_e> minions;
    std::vector<sh_ptr_e> projectiles;
    int minionCount = 0;
    int projectileCount = 0;
    int maxMinions = 5;
    int maxProjectiles = 10;
    passFunc_t passFunc;

    int timeBetweenSpawnsProjectile = 5000;
    int lastProjectileSpawnTime = 0;
public:
    explicit Boss(
            passFunc_t& func,
            vector2 position
    ) : entity(func, entity::ENEMY_BOSS, 15, position, 128, 128), passFunc(func) {};
    void update(float deltaMs) override;

    [[nodiscard]] bool inRageMode() const;
    bool isMinion(const sh_ptr_e& e) const;
    bool isBossProjectile(const sh_ptr_e& e) const;
    void attachMinion(const sh_ptr_e& e);
    void attachProjectile(const sh_ptr_e& e);
    void removeMinion(const sh_ptr_e& e);
    void removeProjectile(const sh_ptr_e& e);
    [[nodiscard]] int getToSpawnMinionCount() const;
    bool canSpawnMinion() const;
    sh_ptr_e spawnMinion(vector2 coords);

    bool isProjectile(const sh_ptr_e& e) const;

    bool canSpawnProjectile() const;

    int getToSpawnProjectileCount() const;

    sh_ptr_e spawnProjectile(vector2 coords);
};


#endif //CSCI437_BOSS_H
