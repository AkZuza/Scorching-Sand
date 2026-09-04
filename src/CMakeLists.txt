file(GLOB_RECURSE SCS_SOURCE_FILES "**.c")
file(GLOB_RECURSE SCS_HEADER_FILES "**.h")

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

add_library(
    scorching_sand
    SHARED
    ${SCS_SOURCE_FILES} ${SCS_HEADER_FILES}
)

target_include_directories(
    scorching_sand
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    scorching_sand
    PRIVATE
    glfw Vulkan::Vulkan
)
