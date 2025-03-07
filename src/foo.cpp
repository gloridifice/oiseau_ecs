//
// Created by Koiro on 06/03/2025.
//

#include "MutEntity.h"
#include "World.h"

struct Health {
    uint32_t value;
    uint32_t maxValue;
};

struct Attack {
    uint32_t value;
    uint32_t maxValue;
};

void foo() {
    auto world = std::make_shared<World>(World());
    (*world).Spawn(Health{1}, Attack{1});
    (*world).Spawn(Health{2}, Attack{2});
    auto id3 = (*world).Spawn(Health{30}, Attack{30}).Id();

    for (auto [heath, attack] : world->Query<Health, Attack>()) {
        heath.value -= 1;
    }

    world->RemoveComponent<Health>(id3);

    for (auto [health, attack] : world->Query<Health, Attack>()) {
        printf("Health: %d, Attack: %d", health.value, attack.value);
    }
}
