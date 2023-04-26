# Setting the minimum version of CMake
cmake_minimum_required(VERSION 3.7...3.26)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()



##################### Project Setup #####################

project(explorer DESCRIPTION "File Explorer using C++"
                LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(SUBMODULES_DIR "${PROJECT_SOURCE_DIR}/submodules")

set(SRC_DIR "${PROJECT_SOURCE_DIR}/sources")
file(GLOB_RECURSE TPP_FILES "${SRC_DIR}/*.tpp")
file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.hpp")
# message(STATUS "TPP_FILES: ${TPP_FILES}")
# message(STATUS "Sources: ${SOURCES}")

# Associate all .tpp files with C++
foreach(TPP_FILE ${TPP_FILES})
    set_source_files_properties(${TPP_FILE} PROPERTIES LANGUAGE CXX)
endforeach()



##################### Submodules #####################

add_subdirectory(${SUBMODULES_DIR}/fmt)

add_library(glad STATIC
    ${SUBMODULES_DIR}/glad/glad.c
)
target_include_directories(glad PUBLIC
    ${SUBMODULES_DIR}/include/glad
)

set(GLFW_USE_WAYLAND OFF)
set(GLFW_BUILD_WAYLAND OFF)
set(GLFW_BUILD_X11 ON)
add_subdirectory(${SUBMODULES_DIR}/glfw)

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
    ${SUBMODULES_DIR}/imgui/backends/imgui_impl_glfw.cpp
    ${SUBMODULES_DIR}/imgui/backends/imgui_impl_opengl3.cpp
    ${SUBMODULES_DIR}/imgui/misc/cpp/imgui_stdlib.h
    ${SUBMODULES_DIR}/imgui/misc/cpp/imgui_stdlib.cpp
)
target_include_directories(imgui PUBLIC
    ${SUBMODULES_DIR}/include/imgui/
    ${SUBMODULES_DIR}/glfw/include/
)



##################### target #####################

add_executable(explorer ${SOURCES} ${TPP_FILES})

target_compile_options(explorer PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(explorer PRIVATE fmt glad glfw imgui)

target_include_directories(explorer PUBLIC
    ${SUBMODULES_DIR}/include
    ${PROJECT_SOURCE_DIR}/sources
)