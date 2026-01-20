cmake_minimum_required(VERSION 4.2.1)
project(Stb)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(stb STATIC
  include/stb/stb_image.h
  src/stb_image.c
)

target_include_directories(stb PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
)
