cmake_minimum_required(VERSION 2.8)

project(BIGINT)

include_directories(${BIGINT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE} -flto -O3 -pthread -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")

add_executable(big_integer_testing
               big_integer_testing.cpp
               big_integer.hpp
               my_vector.hpp
               shared_ptr.hpp
               gtest/gtest-all.cc
               gtest/gtest.h
               gtest/gtest_main.cc)

target_link_libraries(big_integer_testing -lpthread)
