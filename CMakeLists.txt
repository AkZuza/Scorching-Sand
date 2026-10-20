cmake_minimum_required(VERSION 4.3)
project("Scorching Sand")

set(C_STANDARD 23)
set(C_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)
add_subdirectory(sandbox)

if(CMAKE_BUILD_TYPE EQUAL "Debug")
    target_compile_definitions(scorching_sands PUBLIC _DEBUG)
endif()
