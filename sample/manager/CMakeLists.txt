cmake_minimum_required(VERSION 3.14)
project(gaf_test_manager)

find_package(OpenCV REQUIRED)

add_executable(
    gaf_test_manager
    main.cpp
)

target_include_directories(
    gaf_test_manager
    PUBLIC
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(
    gaf_test_manager
    ${OpenCV_LIBS}
    gaf
)
