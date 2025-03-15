//
// Created by Koiro on 14/03/2025.
//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "library.h"


TEST_CASE("Test untyped array") {
    auto array = UntypedArray(sizeof(int));
    auto& a = array;
    int v1 = 1;
    int v2 = 2;
    int v3 = 3;
    CHECK(a.size() == 0);
    a.push(UntypedPtr::from_typed_ptr(&v1));
    CHECK(a.size() == 1);
    a.push(&v2);
    CHECK(a.size() == 2);
    a.push(&v3);
    CHECK(a.size() == 3);

    CHECK(*a[0].into_typed<int>() == v1);
    CHECK(*a.get(1).into_typed<int>() == v2);
    CHECK(*a.get_typed<int>(2) == v3);

    a.remove<int>(1);
    CHECK(a.size() == 2);
    CHECK(a.get_end_index() == 2);
    a.remove<int>(2);
    CHECK(a.size() == 1);
    CHECK(*a.get_end_index() == 0);
}
