cmake_minimum_required(VERSION 2.8)

# Set project name
project("archlinux-installer")

# Set build type and definitions
if($ENV{DEBUG})
    # Debug
    set(CMAKE_BUILD_TYPE "Debug")
else($ENV{DEBUG})
    # Release
    set(CMAKE_BUILD_TYPE "Release")
    add_definitions("-O3")
endif($ENV{DEBUG})

add_definitions("-std=c++14")
add_definitions("-Wall -Wextra -pedantic")

find_package(yaml-cpp)
include_directories(${YAML_CPP_INCLUDE_DIR})

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS system filesystem iostreams REQUIRED)

file(GLOB_RECURSE CPPS "src/*.cpp")
file(GLOB_RECURSE HPPS "include/*.hpp")

include_directories("include")

add_executable(archlinux-installer ${CPPS} ${HPPS})

target_link_libraries(${PROJECT_NAME} ${YAML_CPP_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

