cmake_minimum_required(VERSION 3.30)
project(oiseau_ecs)

set(CMAKE_CXX_STANDARD 20)

add_library(oiseau_ecs STATIC src/library.cpp
        src/EntityManager.h
        src/IComponentArray.h
        src/ComponentManager.h
        src/World.h
        src/MutEntity.h
        src/World.cpp
        src/MutEntity.cpp
        src/Query.cpp
        src/Query.h
        src/foo.cpp
        src/Table.cpp
        src/Table.h)
