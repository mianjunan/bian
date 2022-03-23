cmake_minimum_required (VERSION 3.8)

project(glowworm)

set(
        SOURCES
        src/glowworm.c
        src/util.c include/util.h include/shared.h)

add_library(
        ${PROJECT_NAME}
        ${SOURCES}
)

add_library(
        sub::glowworm
        ALIAS
        ${PROJECT_NAME}
)

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)
