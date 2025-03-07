//
// Created by Koiro on 06/03/2025.
//

#ifndef OISEAU_ECS_QUERY_H
#define OISEAU_ECS_QUERY_H

#include <utility>

#include "unordered_set"
#include "vector"
#include "tuple"
#include "ComponentManager.h"

class World;

template<typename... T>
class Query {
private:
    World *world = nullptr;
    std::unordered_set<Entity> entities{};

public:
    Query(World *world1, std::unordered_set<Entity> entities) : world(world1), entities(std::move(entities)) {}

    class Iterator {
    private:
        World* world = nullptr;
        std::unordered_set<Entity>::iterator iter;
    public:
        Iterator(World* world, std::unordered_set<Entity>::iterator iter): world(world), iter(iter) {}

        std::tuple<T &...> operator*() const;

        Iterator& operator++() {
            ++iter;
            return *this;
        }

        bool operator==(const Iterator& other) const {
            return iter == other.iter;
        }

        bool operator!=(const Iterator& other) const {
            return *this != other;
        }
    };

    Iterator begin() {
        return Iterator(world, entities.begin());
    }

    Iterator end() {
        return Iterator(world, entities.end());
    }
};


#endif //OISEAU_ECS_QUERY_H
