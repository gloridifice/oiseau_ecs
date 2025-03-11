#ifndef OISEAU_ECS_LIBRARY_H
#define OISEAU_ECS_LIBRARY_H

#include "bitset"
#include "unordered_map"
#include <alloca.h>
#include <cassert>
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <memory>
#include <tuple>
#include <vector>

using i32 = int;
using u32 = unsigned int;
using u64 = unsigned long int;
using f32 = float;
using f64 = double;
using usize = size_t;

template <typename K, typename V> using HashMap = std::unordered_map<K, V>;

template <typename T> using Vec = std::vector<T>;
template <typename T> using Shared = std::shared_ptr<T>;
template <typename T> using Unique = std::unique_ptr<T>;
template <typename T> using Weak = std::weak_ptr<T>;

using Entity = uint64_t;
const Entity MAX_ENTITIES = 114514;

using ComponentType = uintptr_t;

class UntypedPtr {
  private:
    void* ptr;
    usize size;

  public:
    UntypedPtr(void* ptr, usize size) : ptr(ptr), size(size) {}
    template <typename T> static UntypedPtr from_variable(T* ptr) {
        auto size = sizeof(T);
        UntypedPtr{ptr, size};
    }
    void copy_value_to(void* dst) { memcpy(dst, ptr, size); }
};

class UntypedArray {
  private:
    void* start_ptr;
    /// Capacity of elements;
    usize capacity;
    /// Count of elements
    usize size;
    /// Size of an element
    usize element_byte_size;

    void* get_ptr_at(size_t index) {
        return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(start_ptr) +
                                       element_byte_size * index);
    }

    usize byte_size_in_memory() { return size * element_byte_size; }

    usize byte_capacity_in_memory() { return capacity * element_byte_size; }

    void increase() { realloca(capacity * 2); }

    void realloca(usize capacity) {
        this->capacity = capacity;
        start_ptr = alloca(byte_capacity_in_memory());
    }

  public:
    UntypedArray(usize element_size_in_byte, usize capacity = 4)
        : element_byte_size(element_size_in_byte) {
        realloca(capacity);
    }

    void push(UntypedPtr& value) {
        if (size == capacity) {
            increase();
        }
        void* p = get_ptr_at(size);
        value.copy_value_to(p);
        size += 1;
    }

    UntypedPtr get(size_t index) {
        return {get_ptr_at(index), element_byte_size};
    }

    template <typename T> T* get_typed(usize index) {
        return static_cast<T*>(get_ptr_at(index));
    }
};

class Signature {
  private:
    Vec<ComponentType> types{};

  public:
    bool operator==(const Signature& rhs) const;
    bool operator!=(const Signature& rhs) const;
    Vec<ComponentType>& get_types() { return types; }
};

class ComponentInfo {
  public:
    usize size;
};

class CompnentManager {
  private:
    HashMap<ComponentType, ComponentInfo> infoMap;

  public:
    ComponentInfo get_component_info_by_id(ComponentType type) {
        return infoMap[type];
    }
};

class Archetype {
  private:
    std::map<ComponentType, Unique<UntypedArray>> arrays{};

  public:
    Archetype(Signature& signature, std::shared_ptr<CompnentManager> manager) {
        for (auto type : signature.get_types()) {
            auto info = manager->get_component_info_by_id(type);
            arrays.insert_or_assign(
                type, std::make_shared<UntypedArray>(UntypedArray(info.size)));
        }
    }

    void add(Entity entity, UntypedPtr ptr) {
        for (auto& [type, array] : arrays) {
            array->push();
        }
    }

    void remove(Entity entity) {}
};

void foo() {}

namespace oiseau {

template <typename T> extern ComponentType component_type() {
    return reinterpret_cast<uintptr_t>(typeid(T).name());
}

} // namespace oiseau

#endif // OISEAU_ECS_LIBRARY_H
