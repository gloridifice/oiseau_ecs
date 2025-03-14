#ifndef OISEAU_ECS_LIBRARY_H
#define OISEAU_ECS_LIBRARY_H

#include "bitset"
#include "unordered_map"
#include <cstddef>
#include <map>
#include <set>
#include <tuple>
#include <vector>
#include "unordered_set"

#pragma region TypeAlias

using i32 = int;
using u32 = unsigned int;
using u64 = unsigned long int;
using f32 = float;
using f64 = double;
using usize = size_t;

template <typename K, typename V> using HashMap = std::unordered_map<K, V>;
template <typename V> using HashSet = std::unordered_set<V>;
template <typename V> using Optional = std::optional<V>;

template <typename T> using Vec = std::vector<T>;
template <typename T> using Shared = std::shared_ptr<T>;
template <typename T> using Unique = std::unique_ptr<T>;
template <typename T> using Weak = std::weak_ptr<T>;
template <typename T> using Set = std::set<T>;

#pragma endregion TypeAlias

using ComponentId = u64;
using ArchetypeId = u64;
using Entity = u64;
using Type = Vec<ComponentId>;
using ArchetypeSet = Set<ArchetypeId>;

#pragma region EntityManager

class EntityManager {
    class Generation {
    public:
        enum State {
            PopRecycled,
            Counting
        };

        State state = Counting;

        /// This index means [it, UINT32_MAX] is not used.
        u32 unused_entity_index = 0;
        Vec<u32> recycled_entities{};

        /// If returned value is `nullopt`, you should enter next generation.
        Optional<u32> pop_entity_index() {
            u32 ret = unused_entity_index;
            if (unused_entity_index < UINT32_MAX) {
                ret = unused_entity_index;
                unused_entity_index += 1;
            } else {
                if (!recycled_entities.empty()) {
                    ret = *recycled_entities.end();
                    recycled_entities.pop_back();
                } else {
                    return std::nullopt;
                }
            }
            return Optional<u32>{ret};
        }
    };

private:
    const u32 MAX_GENERATION = 16;
    u32 generation_index = 0;
    // vec[generation][entity]
    Vec<Generation> generations;

    /// Return [generation index, entity index]
    static std::tuple<u32, u32> parse_entity(Entity entity) {
        auto ei = (u32)entity;
        auto gi = (u32)(entity >> 32);

        return std::make_tuple(gi, ei);
    }

public:
    EntityManager() {
        generations = Vec<Generation>(MAX_GENERATION);
        for (auto& g : generations) {
            g = Generation();
        }
    }

    Entity entity() {
        while (true) {
            auto index = generations[generation_index].pop_entity_index();
            if (index.has_value()) {
                return index.value();
            } else {
                generation_index = (generation_index + 1) % MAX_GENERATION;
            }
        }
    }

    void recycle(Entity entity) {
        auto [gi, ei] = parse_entity(entity);
        auto& gen = generations[gi];
        gen.recycled_entities.push_back(ei);
    }
};


#pragma endregion


/// An Untyped Array
class Column {
private:
    void* elements; // buffer with component data
    usize element_size; // size of a single element
    usize count; // number of elements

public:
    void* operator[](const usize index) const {
        return (static_cast<char*>(elements) + element_size * index);
    }
};

class Archetype;

class ArchetypeEdge {
public:
    Archetype& add;
    Archetype& remove;
};

class Archetype {
public:
    ArchetypeId id;
    Type type;
    Vec<Column> columns;
    HashMap<ComponentId, ArchetypeEdge> edges;
};

class Record {
public:
    Archetype& archetype;
    usize row;
};

class ArchetypeRecord {
public:
    usize column;
};

using ArchetypeMap = HashMap<ArchetypeId, ArchetypeRecord>;

class World {
private:
    HashMap<Entity, Record> entity_index{};
    HashMap<ComponentId, ArchetypeMap> component_index{};
    HashMap<Type, Archetype> archetype_index{};

private:
    void move_entity(Archetype& archetype, usize row,
                     Archetype& next_archetype) {
        // todo
    }
    void move_entity_cause_add(Archetype& archetype, usize row, Archetype& next_archetype, void* component) {

    }

public:
    void* get_component(const Entity entity, const ComponentId cp) {
        const auto& [archetype, row] = entity_index[entity];

        auto& archetypes = component_index[cp];
        if (!archetypes.contains(archetype.id)) {
            return nullptr;
        }
        const auto& arche_record = archetypes[archetype.id];
        return archetype.columns[arche_record.column][row];
    }

    void add_component(const Entity entity, const ComponentId component) {
        const auto& [archetype, row] = entity_index[entity];
        auto& next_archetype = archetype.edges[component].add;
        move_entity(archetype, row, next_archetype);
    }
};

#endif // OISEAU_ECS_LIBRARY_H