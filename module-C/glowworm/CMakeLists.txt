cmake_minimum_required (VERSION 3.8)

project(glowworm)

set(
        SOURCES
        src/glowworm.c
)

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
