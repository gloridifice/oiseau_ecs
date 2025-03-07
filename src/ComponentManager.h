//
// Created by Koiro on 06/03/2025.
//

#ifndef OISEAU_ECS_COMPONENTMANAGER_H
#define OISEAU_ECS_COMPONENTMANAGER_H

#include "library.h"
#include "unordered_map"
#include "IComponentArray.h"

class ComponentManager {
private:
    std::unordered_map<const char *, ComponentType> componentTypes{};
    std::unordered_map<const char *, std::shared_ptr<IComponentArray>> componentArrays{};
    ComponentType nextComponentType{};

    template<typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray() {
        const char *typeName = typeid(T).name();
        assert(!componentTypes.contains(typeName) && "Component not exist.");
        return componentTypes[typeName];
    }

public:
    template<typename T>
    void RegisterComponent() {
        const char *typeName = typeid(T).name();
        assert(componentTypes.contains(typeName) && "Register component twice!");

        componentTypes.insert({typeName, nextComponentType});
        componentArrays.insert({typeName, std::make_shared<ComponentArray<T>>()});

        nextComponentType += 1;
    }

    template<class T>
    ComponentType GetComponentType() {
        auto typeName = typeid(T).name();
        assert(!componentTypes.contains(typeName) && "Component not registered before use!");

        return componentTypes[typeName];
    }

    template<class T>
    void AddComponent(Entity entity, T component) {
        GetComponentArray<T>()->Insert(entity, move(component));
    }

    template<class T>
    void RemoveComponent(Entity entity) {
        GetComponentArray<T>()->Remove(entity);
    }

    template<class T>
    T& GetComponent(Entity entity) {
        return GetComponentArray<T>()->GetData(entity);
    }

    void EntityDestroyed(Entity entity) {
        for (auto const& pair: componentArrays) {
            auto const& component = pair.second;
            component->EntityDestroyed(entity);
        }
    }
};


#endif //OISEAU_ECS_COMPONENTMANAGER_H
