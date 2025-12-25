#include <SDL.h>
#include <SDL_video.h>
#include <glad/glad.h>
#include <iostream>

// ImGui
#include "imgui_impl_sdl2.h"
#include <imgui.h>
#include <imgui_impl_opengl3.h>

#include "Camera.hpp"
#include "Input.hpp"
#include "platform/Steam.hpp"
#include "renderer/Mesh.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

const char *vertexShaderSource = R"(
	#version 450 core
	layout (location = 0) in vec3 aPos;
	uniform mat4 u_MVP;
	void main()
	{
		gl_Position = u_MVP * vec4(aPos, 1.0);
	}
)";

const char *fragmentShaderSource = R"(
	#version 450 core
	out vec4 FragColor;
	void main()
	{
		FragColor = vec4(0.1f, 0.8f, 0.2f, 1.0f);
	}
)";

int main(int argc, char **argv) {
    if (!Steam::Init())
        return -1;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL_Init failed\n";
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window =
        SDL_CreateWindow("orix-engine", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);

    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    // Initialize IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 450");

    // Enable Depth Testing
    glEnable(GL_DEPTH_TEST);

    // Compile Shaders (Basic Boilerplate)
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Setup VAO and VBO
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    Camera camera;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    bool isMouseLocked = true;
    Input::SetCursorLock(isMouseLocked);

    // Timing variables
    uint32_t lastTime = SDL_GetTicks();
    int frameCount = 0;
    float fps = 0;

    bool running = true;
    while (running) {
        Input::Update();

        // Calculate DeltaTime
        float currentFrame = SDL_GetTicks() / 1000.0f;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            ImGui_ImplSDL2_ProcessEvent(&e);
            if (e.type == SDL_QUIT)
                running = false;
        }

        if (Input::IsKeyPressed(SDL_SCANCODE_ESCAPE)) {
            if (isMouseLocked) {
                isMouseLocked = false;
                Input::SetCursorLock(isMouseLocked);
            } else {
                running = false;
            }
        }

        // Input Logic
        if (Input::IsMouseButtonPressed(SDL_BUTTON_LEFT) && !isMouseLocked) {
            isMouseLocked = true;
            Input::SetCursorLock(isMouseLocked);
        }

        if (isMouseLocked) {
            camera.Update(deltaTime);
        }

        // ---- RENDER START ----
        glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = camera.GetProjectionMatrix(1280.0f, 720.0f);
        glm::mat4 mvp = projection * view * model;

        // Send matrix to shader
        int mvpLoc = glGetUniformLocation(shaderProgram, "u_MVP");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36); // Draw 36 vertices (the cube)

        // ImGui Rendering
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(10, 10));
        ImGui::Begin("Debug", nullptr,
                     ImGuiWindowFlags_AlwaysAutoResize |
                         ImGuiWindowFlags_NoBackground);
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text("Pos: %.1f, %.1f, %.1f", camera.Position.x,
                    camera.Position.y, camera.Position.z);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

    // Cleanup IMGUI
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    Steam::Shutdown();
    return 0;
}
