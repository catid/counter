cmake_minimum_required(VERSION 3.7)
project(counter)

set(CMAKE_CXX_STANDARD 11)

set(SRCFILES
    Counter.h
    tests/CounterUnitTest.cpp)

add_executable(counter_unit_test ${SRCFILES})
