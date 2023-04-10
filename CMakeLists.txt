# Setting the minimum version of CMake
cmake_minimum_required(VERSION 3.7...3.26)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(explorer VERSION 0.1
                DESCRIPTION "File Explorer using C++ and GTK"
                LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# option(MY_OPTION "This is settable from the command line" OFF)

file(GLOB_RECURSE SOURCES "sources/*.cpp")

add_executable(explorer ${SOURCES})

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-4.0)
add_subdirectory(${PROJECT_SOURCE_DIR}/submodules/fmt)

target_compile_options(explorer PRIVATE -Wall -Wextra -Wpedantic -Werror)

target_link_libraries(explorer PRIVATE fmt ${GTKMM_LIBRARIES})

target_include_directories(explorer PUBLIC
    ${GTKMM_INCLUDE_DIRS}
    "${PROJECT_SOURCE_DIR}/submodules/include"
    "${PROJECT_SOURCE_DIR}/sources"
)