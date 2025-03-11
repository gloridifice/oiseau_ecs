//
// Created by Koiro on 07/03/2025.
//

#ifndef OISEAU_ECS_TABLE_H
#define OISEAU_ECS_TABLE_H

#include "IComponentArray.h"
#include "library.h"
#include "unordered_map"
#include "vector"


class Table {
  private:
    std::unordered_map<ComponentType, void*> componentArrayMap;
    std::unordered_map<ComponentType, std::shared_ptr<IComponentArray>>
        componentArrays{};
    Signature signature;

  public:
    template <class T> ComponentArray<T>* get_component_array() {
        auto type = oiseau::component_type<T>();
        return componentArrays[type];
    }

    Table(std::vector<ComponentType> types) { this->types = types; };

    void insert(Entity entity) {}
};

#endif // OISEAU_ECS_TABLE_H
