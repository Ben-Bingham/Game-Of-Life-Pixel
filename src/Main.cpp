#include "imgui.h"
#include <fstream>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>

#include "OpenGl-Utility/GLDebug.h"
#include "OpenGl-Utility/Texture.h"

#include "ImGuiInstance.h"
#include <random>

#include <glm/gtc/type_ptr.hpp>

#include "OpenGl-Utility/Shaders/ShaderProgram.h"

// From LearnOpenGL.com
// https://learnopengl.com/code_viewer_gh.php?code=src/1.getting_started/2.2.hello_triangle_indexed/hello_triangle_indexed.cpp

void processInput(GLFWwindow* window);

glm::ivec2 boardSize{ 100, 100 };

std::unique_ptr<Texture> boardA;
std::unique_ptr<Texture> boardB;

bool firstFrameOfBoard = true;
bool step = false;

void ResetBoard() {
    std::vector<unsigned char> startingData;
    startingData.resize(boardSize.x * boardSize.y * 4);

    //startingData[4 * (boardSize.x * 4) + (4 * 4)] = 255;
    //startingData[4 * (boardSize.x * 4) + (5 * 4)] = 255;
    //startingData[4 * (boardSize.x * 4) + (6 * 4)] = 255;
    //startingData[6 * (boardSize.x * 4) + (5 * 4)] = 255;
    //startingData[5 * (boardSize.x * 4) + (6 * 4)] = 255;

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

    //startingData[0] = 255;

    Texture::Parameters textureParameters{ };
    textureParameters.minFilter = Texture::FilteringMode::NEAREST;
    textureParameters.magFilter = Texture::FilteringMode::NEAREST;

    boardA = std::make_unique<Texture>(boardSize, textureParameters, startingData);
    boardB = std::make_unique<Texture>(boardSize, textureParameters);

    glBindImageTexture(0, boardA->Get(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);
    glBindImageTexture(1, boardB->Get(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);

    firstFrameOfBoard = true;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    GLFWwindow* window = glfwCreateWindow(1600, 1000, "Game Of Life Pixel", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

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

    ImGuiInstance imGui{ };
    imGui.Init(window);

    ResetBoard();

    std::unique_ptr<ShaderProgram> computeShader = std::make_unique<ShaderProgram>();
    //computeShader->AddShader("assets\\OnePixel.glsl", ShaderProgram::ShaderType::COMPUTE);
    //computeShader->AddShader("assets\\TwoByTwoPixel.glsl", ShaderProgram::ShaderType::COMPUTE);
    computeShader->AddShader("assets\\Parallel2x2.glsl", ShaderProgram::ShaderType::COMPUTE);

    computeShader->Link();

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

    glm::ivec2 viewPortSize{ };
    glm::ivec2 newBoardSize{ boardSize };

    unsigned int minStepTime{ 16 };

    bool running = false;

    unsigned int timeQueryA{ };
    unsigned int timeQueryB{ };

    glGenQueries(1, &timeQueryA);
    glGenQueries(1, &timeQueryB);

    uint64_t computeTime{ };

    bool firstFrame = true;

    while (!glfwWindowShouldClose(window)) {
        imGui.StartNewFrame();
        ImGui::DockSpaceOverViewport();

        processInput(window);

        ImGui::Begin("Game Of Life Pixel", nullptr, ImGuiWindowFlags_NoMove);
        {
            ImGui::Text("Compute time: %fms", (float)computeTime / 1'000'000.0);
            ImGui::Text("Current board size: (%d, %d)", boardSize.x, boardSize.y);

            if (ImGui::DragInt("Minimum step time", (int*)&minStepTime, 0.05f, 0, 60 * 1000)) { // Max time is one minute
                millisCounted = 0;
            }

            ImGui::DragInt2("Board size", glm::value_ptr(newBoardSize), 1, 1, 100000);

            if (ImGui::Button("Reset")) {
                boardSize = newBoardSize;
                ResetBoard();
            }

            if (ImGui::Button("Pixel Perfect Board")) {
                boardSize = viewPortSize;
                newBoardSize = boardSize;
                ResetBoard();
            }

            ImGui::Checkbox("Play/Pause", &running);

            if (!running) {
                if (ImGui::Button("Step")) {
                    step = true;
                }
            }
        }
        ImGui::End();

        ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
        {
            ImVec2 size = ImGui::GetWindowSize();
            viewPortSize = glm::ivec2{ size.x, size.y - ImGui::GetFrameHeight() };

            ImGui::Image((ImTextureID)boardA->Get(), ImVec2{ (float)viewPortSize.x, (float)viewPortSize.y });

            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                //std::cout << "Left dragging" << std::endl;
            }

            glm::ivec2 mousePos{ ImGui::GetMousePos().x, ImGui::GetMousePos().y };
            glm::ivec2 windowPos{ ImGui::GetWindowPos().x, ImGui::GetWindowPos().y };

            glm::ivec2 viewportMousePos = mousePos - windowPos - glm::ivec2{ 0, ImGui::GetFrameHeight() };

            //std::cout << "Mouse pos: (" << viewportMousePos.x << ", " << viewportMousePos.y << ")" << std::endl;
        }
        ImGui::End();

        //ImGui::ShowDemoWindow();

        if (firstFrameOfBoard || (running && millisCounted >= minStepTime) || (!running && step)) {
            // Get the last compute time
            if (!firstFrame) {
                glGetQueryObjectui64v(timeQueryB, GL_QUERY_RESULT, &computeTime);
            }

            firstFrame = false;
            firstFrameOfBoard = false;
            step = false;

            millisCounted = 0;

            computeShader->Bind();
            glBindImageTexture(0, boardA->Get(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);
            glBindImageTexture(1, boardB->Get(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);

            computeShader->SetVec2("boardSize", boardSize);

            glActiveTexture(GL_TEXTURE0);
            boardA->Bind();

            glActiveTexture(GL_TEXTURE1);
            boardB->Bind();

            glBeginQuery(GL_TIME_ELAPSED, timeQueryA);

            glDispatchCompute(boardSize.x / 2, boardSize.y / 2, 1);
            //glDispatchCompute(boardSize.x, boardSize.y, 1);

            glMemoryBarrier(GL_ALL_BARRIER_BITS);

            glEndQuery(GL_TIME_ELAPSED);

            std::swap(timeQueryA, timeQueryB);
            std::swap(boardA, boardB);
        }

        imGui.FinishFrame();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    imGui.Cleanup();

    killTimerThread = true;
    timerThread.join();

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
