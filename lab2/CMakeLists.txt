cmake_minimum_required(VERSION 3.10) 
project(demo)
add_executable(he ckks_test.cpp)
add_compile_options(-std=c++17)

find_package(SEAL)
target_link_libraries(he SEAL::seal)

