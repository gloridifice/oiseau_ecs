//
// Created by Koiro on 06/03/2025.
//
#pragma once
#ifndef OISEAU_ECS_MUTENTITY_H
#define OISEAU_ECS_MUTENTITY_H

#include "World.h"

class MutEntity {
private:
    Entity id{};
    World *world = nullptr;

public:
    MutEntity(World *world1, Entity id1) : world(world1), id(id1) {}

    [[nodiscard]] Entity Id() const {
        return id;
    }

    template<typename ... T>
    MutEntity &AddComponents(T... component);
};

#endif //OISEAU_ECS_MUTENTITY_H
