#ifndef OISEAU_ECS_LIBRARY_H
#define OISEAU_ECS_LIBRARY_H

#include <cstdlib>
#include <cassert>
#include "bitset"

using Entity = uint64_t;
const Entity MAX_ENTITIES = 114514;

using ComponentType = uintptr_t;

template<typename T>
ComponentType component_type() {
    return reinterpret_cast<uintptr_t>(typeid(T).name());
}

#endif //OISEAU_ECS_LIBRARY_H
