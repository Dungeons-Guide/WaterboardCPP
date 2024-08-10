
message(JAVA_HOME="${JAVA_HOME}")

cmake_minimum_required(VERSION 3.27)
project(waterboard)

set(CMAKE_CXX_STANDARD 17)

find_package(JNI REQUIRED)
include_directories(${JNI_INCLUDE_DIRS})
message(JNI_INCLUDE_DIRs="${JNI_INCLUDE_DIRS}")

add_executable(waterboard_cli main.cpp
        simulation.cpp
        annealing.cpp
        simulation.h
        annealing.h)

add_library(waterboard
        SHARED
        simulation.cpp
        annealing.cpp
        simulation.h
        annealing.h
        kr_syeyoung_dungeonsguide_mod_dungeon_roomprocessor_waterpuzzle_Waterboard.h
        waterpuzzle.cpp)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")