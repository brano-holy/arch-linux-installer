cmake_minimum_required(VERSION 2.8)

project(archlinux-installer)

add_definitions("-std=c++11")

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.53 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

file(GLOB ALI_SRC_CPP src/*.cpp src/configuration/*.cpp src/utils/*.cpp)
add_executable(archlinux-installer ${ALI_SRC_CPP})

add_subdirectory(src)

target_link_libraries(archlinux-installer ${Boost_LIBRARIES})
