cmake_minimum_required(VERSION 3.14)
project(gaf_test_cv_lib)

find_package(OpenCV REQUIRED)

add_library(
    gaf_test_cv_lib
    SHARED
    cv_module.cpp
)

target_include_directories(
    gaf_test_cv_lib
    PUBLIC
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(
    gaf_test_cv_lib
    ${OpenCV_LIBS}
)
