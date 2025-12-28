cmake_minimum_required(VERSION 3.21)
project(orix-engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Steamworks path ----
set(STEAMWORKS_DIR ${CMAKE_SOURCE_DIR}/external/steamworks/sdk)

# ---- Executable ----
add_executable(orix-engine
    src/main.cpp
    src/core/Application.cpp
    src/core/Camera.cpp
    src/core/Input.cpp
    src/game/Chunk.cpp
    src/game/Player.cpp
    src/renderer/Shader.cpp
    src/game/World.cpp
    src/platform/Steam.cpp
    src/core/StateManager.cpp
    src/ui/UIManager.cpp
    src/states/MainMenuState.cpp
    src/states/PlayState.cpp
    external/imgui/imgui_impl_sdl2.cpp
    # Adding RmlUi Backend Implementation files
    external/rmlui/RmlUi_Platform_SDL.cpp
    external/rmlui/RmlUi_Renderer_GL3.cpp
)

# ---- Find packages from vcpkg ----
find_package(SDL2 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(RmlUi CONFIG REQUIRED) # Added RmlUi package

# ---- Includes ----
target_include_directories(orix-engine PRIVATE
    ${STEAMWORKS_DIR}/public
    include
    external/imgui
    external/rmlui       # Added for RmlUi backend headers
    src
)

# ---- Link libraries ----
target_link_libraries(orix-engine PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
    glad::glad
    imgui::imgui
    glm::glm
    RmlUi::RmlUi         # Added RmlUi library link
    opengl32
    ${STEAMWORKS_DIR}/redistributable_bin/win64/steam_api64.lib
)

# ---- Definitions for RmlUi Backends ----
target_compile_definitions(orix-engine PRIVATE
    RMLUI_SDL_VERSION_MAJOR=2
    RMLUI_GL3_CUSTOM_LOADER=<glad/glad.h>
)

# ---- POST-BUILD: Copy Steam DLL ----
add_custom_command(TARGET orix-engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${STEAMWORKS_DIR}/redistributable_bin/win64/steam_api64.dll
    $<TARGET_FILE_DIR:orix-engine>
)

# ---- POST-BUILD: Copy Assets (Shaders & GUI) ----
add_custom_command(TARGET orix-engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
    "$<TARGET_FILE_DIR:orix-engine>/shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:orix-engine>/assets"
    COMMENT "Copying shaders and assets to build directory..."
)