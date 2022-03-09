cmake_minimum_required(VERSION 3.21)
project(posix_programming)

add_compile_definitions(_POSIX_C_SOURCE=200809L _XOPEN_SOURCE=700)

set(CMAKE_CXX_STANDARD 14)

add_executable(posix_programming
        ipc/msg/main.c)

target_include_directories(posix_programming PRIVATE /usr/include)
target_include_directories(posix_programming PRIVATE /usr/local/include)
target_link_directories(posix_programming PRIVATE /usr/lib)
target_link_directories(posix_programming PRIVATE /usr/local/lib)

find_library(LIBDC_ERROR dc_error REQUIRED)
find_library(LIBDC_POSIX dc_posix REQUIRED)

target_link_libraries(posix_programming PRIVATE ${LIBDC_ERROR})
target_link_libraries(posix_programming PRIVATE ${LIBDC_POSIX})
