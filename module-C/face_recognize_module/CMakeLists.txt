cmake_minimum_required (VERSION 3.8)

project(face_recognize_module)

add_executable (
        ${PROJECT_NAME}
        src/main.c include/main.h
        )

target_link_libraries(
        ${PROJECT_NAME}
        sub::glowworm
)

target_include_directories(
        face_recognize_module
        PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)