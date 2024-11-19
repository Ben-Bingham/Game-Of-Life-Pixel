#include "imgui.h"
#include <fstream>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>

#include "OpenGl-Utility/Shader.h"
#include "OpenGl-Utility/GLDebug.h"
#include "OpenGl-Utility/Texture.h"

#include "ImGuiInstance.h"
#include <random>

#include <glm/gtc/type_ptr.hpp>

// From LearnOpenGL.com
// https://learnopengl.com/code_viewer_gh.php?code=src/1.getting_started/2.2.hello_triangle_indexed/hello_triangle_indexed.cpp

void processInput(GLFWwindow* window);

// settings
glm::ivec2 viewPortSize{ };
glm::ivec2 boardSize{ 5000, 5000 };

unsigned int minStepTime{ 16 };

std::unique_ptr<Texture> boardA;
std::unique_ptr<Texture> boardB;

unsigned int computerShader{ 0 };

ImGuiInstance imGui{ };

static std::string ReadFile(const std::string& path) {
    std::string out;
    std::ifstream file;

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(path);

        std::stringstream shaderStream;
        shaderStream << file.rdbuf();

        file.close();
        out = shaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cout << "ERROR: Could not successfully read file with path: " << path << "And error: " << e.what() << std::endl;
    }

    return out;
}

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(1600, 1000, "Game Of Life Pixel", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    imGui.Init(window);

    std::vector<unsigned char> startingData;
    startingData.resize(boardSize.x * boardSize.y * 4);

    startingData[10 * boardSize.x + 10] = 255;
    startingData[10 * boardSize.x + 11] = 255;
    startingData[10 * boardSize.x + 12] = 255;
    startingData[12 * boardSize.x + 11] = 255;
    startingData[11 * boardSize.x + 12] = 255;

    std::random_device dev;
    std::mt19937 rng(dev());

    for (int x = 0; x < boardSize.x * 4; x += 4) {
        for (int y = 0; y < boardSize.y * 4; y += 4) {

            std::uniform_int_distribution<std::mt19937::result_type> dist6(0, 10);

            if (dist6(rng) > 8) {
                startingData[y * boardSize.x + x] = 255;
            }
        }
    }

    boardA = std::make_unique<Texture>(boardSize, Texture::Format::RGBA, Texture::StorageType::UNSIGNED_BYTE, startingData);
    boardB = std::make_unique<Texture>(boardSize, Texture::Format::RGBA, Texture::StorageType::UNSIGNED_BYTE);

    glBindImageTexture(0, boardA->Get(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);
    glBindImageTexture(1, boardB->Get(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);

    std::string computerShaderSource = ReadFile("assets\\compute.glsl");
    const char* computerSource = computerShaderSource.c_str();

    unsigned int cShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(cShader, 1, &computerSource, nullptr);
    glCompileShader(cShader);

    int success;
    char infoLog[512];
    glGetShaderiv(cShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(cShader, 512, nullptr, infoLog);
        std::cout << "ERROR: Compute shader failed to compile:" << std::endl;
        std::cout << infoLog << std::endl;
    }

    computerShader = glCreateProgram();
    glAttachShader(computerShader, cShader);

    glLinkProgram(computerShader);

    glGetProgramiv(computerShader, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(computerShader, 512, nullptr, infoLog);
        std::cout << "ERROR: Compute Shader program failed to link:" << std::endl;
        std::cout << infoLog << std::endl;
    }

    glDeleteShader(cShader);

    std::chrono::time_point<std::chrono::steady_clock> lastStepTime = std::chrono::steady_clock::now();

    std::chrono::duration<double> frameTime{ };
    std::chrono::duration<double> computeTime{ };

    glfwSwapInterval(0);

    bool killTimerThread = false;
    std::atomic<size_t> millisCounted = 0;

    std::thread timerThread{ [&]() {
        while (true) {
            if (killTimerThread) {
                break;
            }

            std::this_thread::sleep_for(std::chrono::duration<double>(1.0f / 1000.0f)); // Sleep for 1 millisecond
            ++millisCounted;
        }
    }};

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {

        std::chrono::time_point<std::chrono::steady_clock> frameStart = std::chrono::steady_clock::now();

        //std::this_thread::sleep_for(std::chrono::duration<double>(1.0));

        imGui.StartNewFrame();
        ImGui::DockSpaceOverViewport();

        // input
        // -----
        processInput(window);

        ImGui::Begin("Game Of Life Pixel", nullptr, ImGuiWindowFlags_NoMove);
        {
            ImGui::Text(("Compute Time: " + std::to_string((double)computeTime.count() * 1000.0) + "ms").c_str());

            ImGui::Text("x: %d, y: %d", viewPortSize.x, viewPortSize.y);

            if (ImGui::DragInt("Minimum step time", (int*)&minStepTime, 0.05f, 0, 60 * 1000)) { // Max time is one minute
                millisCounted = 0;
            }
        }
        ImGui::End();

        ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
        {
            ImVec2 size = ImGui::GetWindowSize();
            viewPortSize = glm::ivec2{ size.x, size.y };

            ImGui::Image((ImTextureID)boardB->Get(), ImVec2{ (float)viewPortSize.x, (float)viewPortSize.y });
        }
        ImGui::End();

        //ImGui::ShowDemoWindow();

        if (millisCounted >= minStepTime) {
            millisCounted = 0;

            std::chrono::time_point<std::chrono::steady_clock> computeStart = std::chrono::steady_clock::now();

            glUseProgram(computerShader);
            glBindImageTexture(0, boardA->Get(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);
            glBindImageTexture(1, boardB->Get(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);

            glUniform2iv(glGetUniformLocation(computerShader, "boardSize"), 1, glm::value_ptr(boardSize));

            glActiveTexture(GL_TEXTURE0);
            boardA->Bind();

            glActiveTexture(GL_TEXTURE1);
            boardB->Bind();

            glDispatchCompute(boardSize.x, boardSize.y, 1);
            glMemoryBarrier(GL_ALL_BARRIER_BITS);
            computeTime = std::chrono::steady_clock::now() - computeStart;

            std::swap(boardA, boardB);
        }

        imGui.FinishFrame();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();

        frameTime = std::chrono::steady_clock::now() - frameStart;
    }

    imGui.Cleanup();

    glDeleteProgram(computerShader);

    killTimerThread = true;
    timerThread.join();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
