cmake_minimum_required(VERSION 3.27)
project(waterboard)

set(CMAKE_CXX_STANDARD 17)

add_executable(waterboard main.cpp
        simulation.cpp
        annealing.cpp
        simulation.h
        annealing.h)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")