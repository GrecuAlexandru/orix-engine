cmake_minimum_required(VERSION 3.21)
project(BlockGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Steamworks path ----
set(STEAMWORKS_DIR ${CMAKE_SOURCE_DIR}/external/steamworks/sdk)

# ---- Executable ----
add_executable(BlockGame
    src/main.cpp
    src/platform/Steam.cpp
)

# ---- Find packages from vcpkg ----
find_package(SDL2 CONFIG REQUIRED)

# ---- Includes ----
target_include_directories(BlockGame PRIVATE
    ${STEAMWORKS_DIR}/public
)

# ---- Link libraries ----
target_link_libraries(BlockGame PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
    opengl32
    ${STEAMWORKS_DIR}/redistributable_bin/win64/steam_api64.lib
)

# ---- Copy Steam DLL ----
add_custom_command(TARGET BlockGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${STEAMWORKS_DIR}/redistributable_bin/win64/steam_api64.dll
    $<TARGET_FILE_DIR:BlockGame>
)
