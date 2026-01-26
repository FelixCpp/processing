cmake_minimum_required(VERSION 4.2.1)
project(Example)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(
  ${CMAKE_CURRENT_SOURCE_DIR}/../processing/
  ${CMAKE_CURRENT_BINARY_DIR}/processing/
)

add_executable(example src/renderbuffer_test.cpp)

target_link_libraries(example PRIVATE processing)
