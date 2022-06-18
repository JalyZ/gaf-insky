cmake_minimum_required(VERSION 3.14)
project(robot_sdk)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 17)

option(BUILD_SAMPLE "SHOULD BUILD SAMPLE" OFF)

find_package(Boost REQUIRED COMPONENTS filesystem)

include_directories(
    include
)

add_library(
    gaf
    STATIC
    src/AlgManager.cpp
)

target_link_libraries(
    gaf
    ${Boost_LIBRARIES}
    Boost::filesystem
)

target_include_directories(
    gaf
    PRIVATE
    ${Boost_INCLUDE_DIRS}
)

if(BUILD_SAMPLE)
    add_subdirectory(
        sample
    )
endif()

install(
    DIRECTORY include/gaf
    DESTINATION include
)

install(
    TARGETS gaf
    DESTINATION lib
)