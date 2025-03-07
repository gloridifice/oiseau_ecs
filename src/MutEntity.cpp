//
// Created by Koiro on 06/03/2025.
//
#include "MutEntity.h"

template<typename... T>
MutEntity &MutEntity::AddComponents(T... component) {
    assert(world != nullptr && "World should not be null.");
    (world->AddComponent(id, std::move(component)), ...);
    return *this;
}
