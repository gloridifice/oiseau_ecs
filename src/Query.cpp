//
// Created by Koiro on 06/03/2025.
//

#include "Query.h"
#include "World.h"

template <typename... T>
std::tuple<T&...> Query<T...>::Iterator::operator*() const {
    auto id = *iter;
    return std::tie(world->GetComponentManager().GetComponent<T>(id)...);
}
