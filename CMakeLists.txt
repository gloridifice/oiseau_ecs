cmake_minimum_required(VERSION 3.30)
project(oiseau_ecs)

set(CMAKE_CXX_STANDARD 20)

add_library(oiseau_ecs STATIC src/library.cpp
        src/library.h)
