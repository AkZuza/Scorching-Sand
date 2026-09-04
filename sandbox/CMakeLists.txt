file(GLOB_RECURSE SANDBOX_SOURCE_FILES "**.c")
file(GLOB_RECURSE SANDBOX_HEADER_FILES "**.h")

add_executable(
    sandbox
    ${SANDBOX_SOURCE_FILES} ${SANDBOX_HEADER_FILES}
)

target_link_libraries(
    sandbox
    PRIVATE
    scorching_sand
)
