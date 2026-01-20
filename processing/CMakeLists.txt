cmake_minimum_required(VERSION 4.2.1)
project(Processing)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(../external/glfw ${CMAKE_BINARY_DIR}/glfw)
add_subdirectory(../external/glad ${CMAKE_BINARY_DIR}/glad)
add_subdirectory(../external/stb/ ${CMAKE_BINARY_DIR}/stb)

add_library(processing STATIC
    src/processing/glfw_graphics_context.cpp
    src/processing/glfw_window.cpp
    src/processing/graphics.cpp
    src/processing/graphics_context.cpp
    src/processing/processing.cpp
    src/processing/render_style.cpp
    src/processing/render_style_stack.cpp
    src/processing/render_targets.cpp
    src/processing/batch_renderer.cpp
    src/processing/shape_builder.cpp
    src/processing/texture.cpp
    src/processing/window.cpp
)

target_include_directories(processing PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

target_include_directories(processing PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
)

target_link_libraries(processing PRIVATE glfw)
target_link_libraries(processing PRIVATE glad)
target_link_libraries(processing PRIVATE stb)

target_compile_definitions(processing PRIVATE GLFW_INCLUDE_NONE)
