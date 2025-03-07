//
// Created by Koiro on 06/03/2025.
//
#include "World.h"
#include "MutEntity.h"

void World::Despawn(Entity entity) {
    GetEntityManager().DestroyEntity(entity);
    GetComponentManager().EntityDestroyed(entity);
}

ComponentManager &World::GetComponentManager() {
    return componentManager;
}

EntityManager &World::GetEntityManager() {
    return entityManager;
}

template<typename... T>
MutEntity World::Spawn(T... components) {
    auto id = GetEntityManager().CreateEntity();
    auto entity = MutEntity(this, id).AddComponents(components...);
    return entity;
}

template<typename... T>
Query<T...> World::Query() {

    return;//todo;
}

template<class T>
void World::AddComponent(Entity entity, T component) {
    GetComponentManager().AddComponent(entity, move(component));
}

template<class T>
void World::RemoveComponent(Entity entity) {
    GetComponentManager().RemoveComponent<T>(entity);
}

