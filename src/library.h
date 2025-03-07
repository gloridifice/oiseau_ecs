#ifndef OISEAU_ECS_LIBRARY_H
#define OISEAU_ECS_LIBRARY_H

#include "bitset"
#include "unordered_map"
#include <cassert>
#include <cstddef>

using i32 = int;
using u32 = unsigned int;
using u64 = unsigned long int;
using f32 = float;
using f64 = double;
using usize = size_t;

template <typename K, typename V> using HashMap = std::unordered_map<K, V>;

/// Sp
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
    usize size;
    usize element_size;

    void* get_ptr(size_t index) {
        return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(start_ptr) +
                                       element_size * index);
    }

  public:
    UntypedPtr get(size_t index) { return {get_ptr(index), element_size}; }

    template <typename T> T* get_typed(usize index) {
        return static_cast<T*>(get_ptr(index));
    }
};

using Entity = uint64_t;
const Entity MAX_ENTITIES = 114514;

using ComponentType = uintptr_t;

namespace oiseau {
template <typename T> extern ComponentType component_type() {
    return reinterpret_cast<uintptr_t>(typeid(T).name());
}
} // namespace oiseau

#endif // OISEAU_ECS_LIBRARY_H
