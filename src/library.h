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
    static std::tuple<u32, u32> parse_entity(const Entity entity) {
        auto ei = static_cast<u32>(entity);
        auto gi = static_cast<u32>(entity >> 32);

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
            if (auto index = generations[generation_index].pop_entity_index();
                index.has_value()) {
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

class UntypedPtr {
private:
    void* ptr;
    usize size;

public:
    UntypedPtr(void* ptr, const usize size) : ptr(ptr), size(size) {
    }

    void copy_value_to(void* dst) const {
        memcpy(dst, ptr, size);
    }

    template <typename T> static UntypedPtr from_typed_ptr(T* value_ptr) {
        return UntypedPtr(static_cast<void*>(value_ptr), sizeof(T));
    }

    template <typename T> T* into_typed() const {
        if (sizeof(T) != size) {
            throw "Type's size is not matched!";
        }
        return static_cast<T*>(ptr);
    }

    bool is_not_null() const {
        return ptr != nullptr && size > 0;
    }

    static UntypedPtr null() {
        return UntypedPtr(nullptr, 0);
    }
};

class UntypedArray {
private:
    void* start_ptr = nullptr;
    /// Capacity of elements;
    usize capacity{};
    /// Count of elements
    Optional<usize> end_index = std::nullopt;
    /// Size of an element
    usize element_byte_size;
    HashSet<usize> removed_indices;

    void* get_ptr_at(const usize index) {
        return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(start_ptr) +
                                       element_byte_size * index);
    }

    usize byte_capacity_in_memory() const {
        return capacity * element_byte_size;
    }

    void increase() {
        capacity *= 2;
        realloc_memory();
    }

    void realloc_memory() {
        start_ptr = realloc(start_ptr, byte_capacity_in_memory());
    }

    void end_index_increase() {
        if (end_index) {
            *end_index += 1;
        } else {
            end_index = 0;
        }
    }

    void end_index_decrease() {
        if (!end_index)
            throw "Array is empty! You can't access it by index.";
        if (*end_index == 0) {
            end_index = std::nullopt;
        } else {
            *end_index -= 1;
        }
    }

public:
    explicit UntypedArray(const usize element_size_in_byte,
                          const usize capacity = 4)
        : capacity(capacity), element_byte_size(element_size_in_byte) {

        realloc_memory();
    }

    usize size() const {
        if (end_index) {
            return *end_index + 1 - removed_indices.size();
        }
        return 0;
    }

    void push(const UntypedPtr&& value) {
        usize index = 0;
        if (!removed_indices.empty()) {
            index = (*removed_indices.begin());
            removed_indices.extract(index);
        } else {
            end_index_increase();
            index = *end_index;
            if (end_index >= capacity)
                increase();
        }
        void* p = get_ptr_at(index);
        value.copy_value_to(p);
    }

    template <typename T>
    void push(T* value) {
        this->push(UntypedPtr::from_typed_ptr(value));
    }

    template <typename T>
    T pop_back() {
        return remove<T>(end_index);
    }

    template <typename T>
    T remove(const usize index) {
        if (end_index && index == *end_index) {
            end_index_decrease();
            while (end_index && removed_indices.contains(*end_index)) {
                removed_indices.extract(*end_index);
                end_index_decrease();
            }
        } else {
            removed_indices.insert(index);
        }
        return *get_typed<T>(index);
    }

    UntypedPtr get(const size_t index) {
        return {get_ptr_at(index), element_byte_size};
    }

    template <typename T> T* get_typed(const usize index) {
        return static_cast<T*>(get_ptr_at(index));
    }

    UntypedPtr operator[](const usize index) {
        return get(index);
    }

    Optional<usize> get_end_index() const {
        return end_index;
    }
};

class ComponentInfo {
public:
    usize size;
    ComponentId id;
};

class Archetype;

class ArchetypeEdge {
public:
    Weak<Archetype> add;
    Weak<Archetype> remove;
};

class Archetype {
public:
    ArchetypeId id;
    Type type;
    Vec<ComponentInfo> component_infos;
    Vec<UntypedArray> columns;
    HashMap<ComponentId, ArchetypeEdge> edges;
};

class Record {
public:
    Shared<Archetype> archetype;
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
    HashMap<ComponentId, ComponentInfo> component_info_index{};

public:
    template <typename T>
    static ComponentId component_type_of() {
        return reinterpret_cast<u64>(typeid(T).name());
    }

    void move_entity(Archetype& archetype, usize row,
                     Archetype& next_archetype) {
        // todo
    }

    /// [A, B, C]
    /// [A, C, B, D]
    /// [B, C]
    template <typename T>
    void move_entity_caused_by_adding(Archetype& archetype, usize row,
                                      Archetype& next_archetype,
                                      T&& component) {
        auto& type = archetype.type;
        auto& new_type = archetype.type;
        if (const ComponentId id = component_type_of<T>(); component_info_index.
            contains(id)) {
            for (ComponentInfo target_info : next_archetype.component_infos) {

            }
        }
    }

    void move_entity_caused_by_removing(Archetype& archetype, usize row,
                                        Archetype& next_archetype,
                                        void* component) {

    }

public:
    UntypedPtr get_component(const Entity entity, const ComponentId cp) {
        const auto& [archetype, row] = entity_index[entity];

        auto& archetypes = component_index[cp];
        if (!archetypes.contains(archetype->id)) {
            return UntypedPtr::null();
        }
        const auto& arche_record = archetypes[archetype->id];
        return archetype->columns[arche_record.column][row];
    }

    void add_component(const Entity entity, const ComponentId component) {
        const auto& [archetype, row] = entity_index[entity];
        if (const auto& next_archetype = archetype->edges[component].add;
            next_archetype.expired()) {
            //todo create new archetype
        } else {
            move_entity(*archetype, row, *next_archetype.lock());
        }
    }
};

#endif // OISEAU_ECS_LIBRARY_H