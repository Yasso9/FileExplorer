# Setting the minimum version of CMake
cmake_minimum_required(VERSION 3.7...3.26)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(explorer DESCRIPTION "File Explorer using C++"
                LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(SUBMODULES_DIR "${PROJECT_SOURCE_DIR}/submodules")

add_subdirectory(${SUBMODULES_DIR}/fmt)
add_library(glad STATIC
    ${SUBMODULES_DIR}/glad/glad.c
)
add_subdirectory(${SUBMODULES_DIR}/SDL)
add_library(imgui STATIC
	${SUBMODULES_DIR}/imgui/imgui.cpp
	${SUBMODULES_DIR}/imgui/imgui.h
	${SUBMODULES_DIR}/imgui/imstb_rectpack.h
	${SUBMODULES_DIR}/imgui/imstb_textedit.h
	${SUBMODULES_DIR}/imgui/imstb_truetype.h
    ${SUBMODULES_DIR}/imgui/imgui_demo.cpp
    ${SUBMODULES_DIR}/imgui/imgui_draw.cpp
    ${SUBMODULES_DIR}/imgui/imgui_internal.h
    ${SUBMODULES_DIR}/imgui/imgui_tables.cpp
    ${SUBMODULES_DIR}/imgui/imgui_widgets.cpp
    ${SUBMODULES_DIR}/imgui/backends/imgui_impl_sdl2.cpp
    ${SUBMODULES_DIR}/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(glad PUBLIC
    ${SUBMODULES_DIR}/include/glad
)
target_include_directories(imgui PUBLIC
    ${SUBMODULES_DIR}/include/imgui
    ${SUBMODULES_DIR}/include/SDL
)

file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/sources/*.cpp")
add_executable(explorer ${SOURCES})

target_compile_options(explorer PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(explorer PRIVATE fmt glad SDL2 imgui)

target_include_directories(explorer PUBLIC
    ${SUBMODULES_DIR}/include
    ${PROJECT_SOURCE_DIR}/sources
)