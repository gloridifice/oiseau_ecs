//
// Created by Koiro on 06/03/2025.
//

#ifndef OISEAU_ECS_ICOMPONENTARRAY_H
#define OISEAU_ECS_ICOMPONENTARRAY_H

#include "library.h"
#include "array"
#include "unordered_map"

class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void EntityDestroyed(Entity entity) = 0;
};

template<typename C>
class ComponentArray : public IComponentArray {
private:
    std::array<C, MAX_ENTITIES> componentArray;
    std::unordered_map<Entity, size_t> entityToIndexMap;
    std::unordered_map<Entity, size_t> indexToEntityMap;
    size_t size;
public:
    void Insert(Entity entity, C component){
        assert(entityToIndexMap.contains(entity) && "Component added to same entity more than once.");

        size_t newIndex = size;
        entityToIndexMap[entity] = newIndex;
        indexToEntityMap[newIndex] = entity;
        componentArray[newIndex] = component;

        size += 1;
    }

    void Remove(Entity entity) {
        assert(!entityToIndexMap.contains(entity) && "Removing non-existent component!");

        size_t indexOfRemovedEntity = entityToIndexMap[entity];
        size_t indexOfLastElement = size - 1;
        componentArray[indexOfRemovedEntity] = componentArray[indexOfLastElement];
        Entity entityOfLastElement = indexToEntityMap[indexOfLastElement];
        entityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
        indexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

        entityToIndexMap.erase(entity);
        indexToEntityMap.erase(indexOfLastElement);

        size -= 1;
    }

    C& GetData(Entity entity) {
        assert(!entityToIndexMap.contains(entity) && "Retrieving non-existent component.");
        return componentArray[entityToIndexMap[entity]];
    }

    void EntityDestroyed(Entity entity) override {
        if (entityToIndexMap.contains(entity)) {
            Remove(entity);
        }
    }
};


#endif //OISEAU_ECS_ICOMPONENTARRAY_H
