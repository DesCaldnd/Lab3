cmake_minimum_required(VERSION 3.22)
project(Lab3 C)

set(CMAKE_C_STANDARD 23)
if (NOT MSVC)
set(CMAKE_C_FLAGS  "-Wall -Werror -Wextra")
endif ()

add_executable(Task2 T2/main.c)
add_executable(Task9 T9/main.c)
