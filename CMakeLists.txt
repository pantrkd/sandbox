cmake_minimum_required(VERSION 3.28.0)
set(PROJECT_NAME sandbox)
project(${PROJECT_NAME})
add_executable(${PROJECT_NAME} main.cpp)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)