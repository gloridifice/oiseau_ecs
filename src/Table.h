//
// Created by Koiro on 07/03/2025.
//

#ifndef OISEAU_ECS_TABLE_H
#define OISEAU_ECS_TABLE_H

#include "unordered_map"
#include "IComponentArray.h"
#include "library.h"

class Table {
private:
    std::unordered_map<ComponentType, void*> componentArrayMap;
    std::unordered_map<ComponentType, std::shared_ptr<IComponentArray>> componentArrays{};

public:
    template<class T>
    ComponentArray<T>* GetComponentArray() {
        const char *typeName = typeid(T).name();
        return componentArrays[typeName];
    }
};


#endif //OISEAU_ECS_TABLE_H
