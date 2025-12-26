cmake_minimum_required(VERSION 3.21)
project(orix-engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Steamworks path ----
set(STEAMWORKS_DIR ${CMAKE_SOURCE_DIR}/external/steamworks/sdk)

# ---- Executable ----
# Note: src/renderer/Shader.cpp is included here, fixing your "unresolved external" error
add_executable(orix-engine
    src/main.cpp
	src/core/Application.cpp
    src/core/Camera.cpp
    src/core/Input.cpp
    src/core/Chunk.cpp
    src/renderer/Shader.cpp
	src/game/World.cpp
    src/platform/Steam.cpp
    external/imgui/imgui_impl_sdl2.cpp
)

# ---- Find packages from vcpkg ----
find_package(SDL2 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# ---- Includes ----
target_include_directories(orix-engine PRIVATE
    ${STEAMWORKS_DIR}/public
    include
    external/imgui
    src
)

# ---- Link libraries ----
target_link_libraries(orix-engine PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
    glad::glad
    imgui::imgui
    glm::glm
    opengl32
    ${STEAMWORKS_DIR}/redistributable_bin/win64/steam_api64.lib
)

# ---- POST-BUILD: Copy Steam DLL ----
add_custom_command(TARGET orix-engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${STEAMWORKS_DIR}/redistributable_bin/win64/steam_api64.dll
    $<TARGET_FILE_DIR:orix-engine>
)

# ---- POST-BUILD: Copy Shaders Folder ----
# This ensures your .vert and .frag files are in the build directory
# every time you compile the project.
add_custom_command(TARGET orix-engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
    "$<TARGET_FILE_DIR:orix-engine>/shaders"
    COMMENT "Copying shaders to build directory..."
)