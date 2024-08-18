cmake_minimum_required(VERSION 3.20)
project(TerrariumPolyOctave VERSION 1.0.0)

set(FIRMWARE_NAME TerrariumPolyOctave)
set(FIRMWARE_SOURCES
    main.cpp
    syscalls.c
    util/Led.h
    util/Led.cpp
    util/LinearRamp.h
    util/Mapping.h
    util/Multirate.h
    util/Terrarium.h
    util/Terrarium.cpp
)
set(LIBDAISY_DIR ${CMAKE_SOURCE_DIR}/lib/libDaisy)
include(${LIBDAISY_DIR}/cmake/default_build.cmake)

option(Q_BUILD_EXAMPLES "build Q library examples" OFF)
option(Q_BUILD_TEST "build Q library tests" OFF)
option(Q_BUILD_IO "build Q IO library" OFF)
add_subdirectory(lib/q)

add_subdirectory(lib/gcem)

target_include_directories(${FIRMWARE_NAME} PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(${FIRMWARE_NAME} PUBLIC libq gcem)

set_target_properties(${FIRMWARE_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
)

target_link_options(${FIRMWARE_NAME} PRIVATE
    -flto=auto
)

if(NOT PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    # Git auto-ignore out-of-source build directory
    file(GENERATE OUTPUT .gitignore CONTENT "*")
endif()
