//
// Created by Koiro on 06/03/2025.
//

#pragma once
#ifndef OISEAU_ECS_WORLD_H
#define OISEAU_ECS_WORLD_H

#include "ComponentManager.h"
#include "EntityManager.h"
#include "Query.h"

class MutEntity;

class World {
private:
    ComponentManager componentManager {};
    EntityManager entityManager {};

public:
    World() = default;

    template<typename... T>
    MutEntity Spawn(T... components);
    void Despawn(Entity entity);

    template<typename... T>
    Query<T...> Query();

    template<class T>
    void AddComponent(Entity entity, T component);

    template<class T>
    void RemoveComponent(Entity entity);

    ComponentManager& GetComponentManager();
    EntityManager& GetEntityManager();
};


#endif //OISEAU_ECS_WORLD_H
