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

#include "OpenGl-Utility/SSBO.h"
#include "OpenGl-Utility/Shaders/ShaderProgram.h"

glm::ivec2 boardSize{ 20, 20 };

std::pair<glm::ivec2, glm::ivec2> visibleBoard = std::make_pair(glm::ivec2{ 0, 0 }, boardSize);

std::unique_ptr<Texture> boardA;
std::unique_ptr<Texture> boardB;

bool firstFrameOfBoard = true;
bool step = false;

std::unique_ptr<ShaderProgram> cellSetShader;
std::unique_ptr<ShaderProgram> cellClearShader;
std::unique_ptr<SSBO<glm::ivec2>> cellSSBO;

int maxComputeWorkGroupsX{ 0 };

void SetCells(const std::vector<glm::ivec2>& cells) {
    if (cells.size() >= (size_t)maxComputeWorkGroupsX) {
        size_t iterationCount = (size_t)std::ceil((float)cells.size() / (float)maxComputeWorkGroupsX);
        size_t highestIndex = 0;

        for (size_t i = 0; i < iterationCount; ++i) {
            std::vector<glm::ivec2> cellSubset{ };
            cellSubset.reserve(maxComputeWorkGroupsX - 1);
            for (size_t j = 0; j < (maxComputeWorkGroupsX - 1); ++j) {
                if (highestIndex >= cells.size()) {
                    break;
                }

                cellSubset.push_back(cells[highestIndex]);
                ++highestIndex;
            }

            SetCells(cellSubset);
        }

        return;
    }

    cellSetShader->Bind();

    glBindImageTexture(0, boardA->Get(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);

    glActiveTexture(GL_TEXTURE0);
    boardA->Bind();

    cellSSBO->SetData(cells);

    glDispatchCompute((int)cells.size(), 1, 1);

    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void ClearCells(const std::vector<glm::ivec2>& cells) {
    if (cells.size() >= (size_t)maxComputeWorkGroupsX) {
        size_t iterationCount = (size_t)std::ceil((float)cells.size() / (float)maxComputeWorkGroupsX);
        size_t highestIndex = 0;

        for (size_t i = 0; i < iterationCount; ++i) {
            std::vector<glm::ivec2> cellSubset{ };
            cellSubset.reserve(maxComputeWorkGroupsX - 1);
            for (size_t j = 0; j < (maxComputeWorkGroupsX - 1); ++j) {
                if (highestIndex >= cells.size()) {
                    break;
                }

                cellSubset.push_back(cells[highestIndex]);
                ++highestIndex;
            }

            ClearCells(cellSubset);
        }

        return;
    }

    cellClearShader->Bind();

    glBindImageTexture(0, boardA->Get(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);

    glActiveTexture(GL_TEXTURE0);
    boardA->Bind();

    cellSSBO->SetData(cells);

    glDispatchCompute((int)cells.size(), 1, 1);

    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

float chanceToFillCell = 0.3f;

void ApplyBoardType(int boardType) {
    // Clear the board
    std::vector<glm::ivec2> clearCells{ };
    clearCells.reserve((size_t)boardSize.x * (size_t)boardSize.y);
    for (int x = 0; x < boardSize.x; ++x) {
        for (int y = 0; y < boardSize.y; ++y) {
            clearCells.emplace_back(glm::ivec2{ x, y });
        }
    }

    ClearCells(clearCells);

    std::vector<glm::ivec2> cells{ };

    switch (boardType) {
        case 1: { // Random
            cells.reserve((size_t)((float)boardSize.x * (float)boardSize.y * chanceToFillCell));

            std::random_device dev;
            std::mt19937 rng(dev());

            for (int x = 0; x < boardSize.x; ++x) {
                for (int y = 0; y < boardSize.y; ++y) {
                    std::uniform_real dist(0.0f, 1.0f);
                    if (dist(rng) < chanceToFillCell) {
                        cells.emplace_back(glm::ivec2{ x, y });
                    }
                }
            }

            break;
        }
        case 2: { // Glider
            int baseY = boardSize.y / 2;
            int baseX = boardSize.x / 2;

            cells = std::vector<glm::ivec2>{
                { baseX + 0, baseY + 0 },
                { baseX + 1, baseY + 0 },
                { baseX + 2, baseY + 0 },
                { baseX + 1, baseY + 2 },
                { baseX + 2, baseY + 1 }
            };

            break;
        }
        case 3: { // Kok's galaxy
            int baseY = boardSize.y / 4;
            int baseX = boardSize.x / 4;

            cells = std::vector<glm::ivec2>{
                { baseX + 0, baseY + 0 },
                { baseX + 1, baseY + 0 },
                { baseX + 2, baseY + 0 },
                { baseX + 3, baseY + 0 },
                { baseX + 4, baseY + 0 },
                { baseX + 5, baseY + 0 },
                { baseX + 0, baseY + 1 },
                { baseX + 1, baseY + 1 },
                { baseX + 2, baseY + 1 },
                { baseX + 3, baseY + 1 },
                { baseX + 4, baseY + 1 },
                { baseX + 5, baseY + 1 },

                { baseX + 3, baseY + 7 },
                { baseX + 4, baseY + 7 },
                { baseX + 5, baseY + 7 },
                { baseX + 6, baseY + 7 },
                { baseX + 7, baseY + 7 },
                { baseX + 8, baseY + 7 },
                { baseX + 3, baseY + 8 },
                { baseX + 4, baseY + 8 },
                { baseX + 5, baseY + 8 },
                { baseX + 6, baseY + 8 },
                { baseX + 7, baseY + 8 },
                { baseX + 8, baseY + 8 },

                { baseX + 7, baseY + 0 },
                { baseX + 7, baseY + 1 },
                { baseX + 7, baseY + 2 },
                { baseX + 7, baseY + 3 },
                { baseX + 7, baseY + 4 },
                { baseX + 7, baseY + 5 },
                { baseX + 8, baseY + 0 },
                { baseX + 8, baseY + 1 },
                { baseX + 8, baseY + 2 },
                { baseX + 8, baseY + 3 },
                { baseX + 8, baseY + 4 },
                { baseX + 8, baseY + 5 },

                { baseX + 0, baseY + 3 },
                { baseX + 0, baseY + 4 },
                { baseX + 0, baseY + 5 },
                { baseX + 0, baseY + 6 },
                { baseX + 0, baseY + 7 },
                { baseX + 0, baseY + 8 },
                { baseX + 1, baseY + 3 },
                { baseX + 1, baseY + 4 },
                { baseX + 1, baseY + 5 },
                { baseX + 1, baseY + 6 },
                { baseX + 1, baseY + 7 },
                { baseX + 1, baseY + 8 },
            };

            break;
        }
        case 4: { // Glider Gun
            int baseY = 0;
            int baseX = 0;

            cells = std::vector<glm::ivec2>{
                { baseX + 0, baseY + 4 },
                { baseX + 1, baseY + 4 },
                { baseX + 0, baseY + 5 },
                { baseX + 1, baseY + 5 },

                { baseX + 10, baseY + 4 },
                { baseX + 10, baseY + 5 },
                { baseX + 10, baseY + 6 },

                { baseX + 11, baseY + 3 },
                { baseX + 11, baseY + 7 },

                { baseX + 12, baseY + 2 },
                { baseX + 12, baseY + 8 },

                { baseX + 13, baseY + 2 },
                { baseX + 13, baseY + 8 },

                { baseX + 14, baseY + 5 },

                { baseX + 15, baseY + 3 },
                { baseX + 15, baseY + 7 },

                { baseX + 16, baseY + 4 },
                { baseX + 16, baseY + 5 },
                { baseX + 16, baseY + 6 },

                { baseX + 17, baseY + 5 },

                { baseX + 20, baseY + 2 },
                { baseX + 20, baseY + 3 },
                { baseX + 20, baseY + 4 },

                { baseX + 21, baseY + 2 },
                { baseX + 21, baseY + 3 },
                { baseX + 21, baseY + 4 },

                { baseX + 22, baseY + 1 },
                { baseX + 22, baseY + 5 },

                { baseX + 24, baseY + 0 },
                { baseX + 24, baseY + 1 },
                { baseX + 24, baseY + 5 },
                { baseX + 24, baseY + 6 },

                { baseX + 34, baseY + 2 },
                { baseX + 35, baseY + 2 },
                { baseX + 34, baseY + 3 },
                { baseX + 35, baseY + 3 },
            };

            break;
        }
    }

    SetCells(cells);
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

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &maxComputeWorkGroupsX);

    ImGuiInstance imGui{ };
    imGui.Init(window);

    std::unique_ptr<ShaderProgram> computeShader = std::make_unique<ShaderProgram>();
    computeShader->AddShader("assets\\2x2ThreadGrid.glsl", ShaderProgram::ShaderType::COMPUTE);
    computeShader->Link();

    cellSetShader = std::make_unique<ShaderProgram>();
    cellSetShader->AddShader("assets\\SetCells.glsl", ShaderProgram::ShaderType::COMPUTE);
    cellSetShader->Link();

    cellClearShader = std::make_unique<ShaderProgram>();
    cellClearShader->AddShader("assets\\ClearCells.glsl", ShaderProgram::ShaderType::COMPUTE);
    cellClearShader->Link();

    Texture::Parameters textureParameters{ };
    textureParameters.minFilter = Texture::FilteringMode::NEAREST;
    textureParameters.magFilter = Texture::FilteringMode::NEAREST;

    boardA = std::make_unique<Texture>(boardSize, textureParameters);
    boardB = std::make_unique<Texture>(boardSize, textureParameters);

    cellSSBO = std::make_unique<SSBO<glm::ivec2>>(2);

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

    std::vector<glm::ivec2> setCells{ };
    std::vector<glm::ivec2> clearCells{ };

    int boardPreset = 0;

    bool boardSizeChange = false;

    while (!glfwWindowShouldClose(window)) {
        imGui.StartNewFrame();
        ImGui::DockSpaceOverViewport();

        ImGui::Begin("Game Of Life Pixel", nullptr, ImGuiWindowFlags_NoMove);
        {
            ImGui::Text("Compute time: %fms", (float)computeTime / 1'000'000.0);
            ImGui::Text("Current board size: (%d, %d)", boardSize.x, boardSize.y);

            ImGui::Separator();

            ImGui::Checkbox("Play/Pause", &running);

            if (!running) {
                ImGui::SameLine();

                if (ImGui::Button("Step")) {
                    step = true;
                }
            }

            ImGui::Text("Minimum Step Time (ms):");

            if (ImGui::DragInt("##minStepTime", (int*)&minStepTime, 0.05f, 0, 60 * 1000)) { // Max time is one minute
                millisCounted = 0;
            }

            ImGui::Separator();

            ImGui::Text("Modify Board Size:");

            if (ImGui::DragInt2("##BoardSize", glm::value_ptr(newBoardSize), 1, 1, 100000)) {
                boardSizeChange = true;
            }

            if (ImGui::Button("Pixel Perfect Board")) {
                newBoardSize = viewPortSize;
                boardSizeChange = true;
            }

            if (boardSizeChange) {
                if (ImGui::Button("Update Board Size")) {
                    newBoardSize = glm::clamp(newBoardSize, glm::ivec2{ 1, 1 }, viewPortSize);
                    boardSize = newBoardSize;

                    Texture::Parameters textureParameters{ };
                    textureParameters.minFilter = Texture::FilteringMode::NEAREST;
                    textureParameters.magFilter = Texture::FilteringMode::NEAREST;

                    boardA = std::make_unique<Texture>(boardSize, textureParameters);
                    boardB = std::make_unique<Texture>(boardSize, textureParameters);

                    ApplyBoardType(boardPreset);

                    boardSizeChange = false;
                }
            }

            ImGui::Separator();

            ImGui::Text("Select Board Preset");
            ImGui::RadioButton("Empty", &boardPreset, 0);

            ImGui::RadioButton("Random", &boardPreset, 1);
            if (boardPreset == 1) {
                ImGui::DragFloat("Chance to fill random cell: ", &chanceToFillCell, 0.001f, 0, 1);
            }

            ImGui::RadioButton("Glider", &boardPreset, 2);
            ImGui::RadioButton("Kok's Galaxy", &boardPreset, 3);
            ImGui::RadioButton("Glider Gun", &boardPreset, 4);

            if (ImGui::Button("Apply Board Preset")) {
                ApplyBoardType(boardPreset);
            }
        }
        ImGui::End();

        ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
        {
            ImVec2 size = ImGui::GetWindowSize();
            viewPortSize = glm::ivec2{ size.x, size.y - ImGui::GetFrameHeight() };

            ImGui::Image((ImTextureID)boardA->Get(), ImVec2{ (float)viewPortSize.x, (float)viewPortSize.y });

            glm::ivec2 mousePos{ ImGui::GetMousePos().x, ImGui::GetMousePos().y };
            glm::ivec2 windowPos{ ImGui::GetWindowPos().x, ImGui::GetWindowPos().y };

            glm::ivec2 viewportMousePos = mousePos - windowPos - glm::ivec2{ 0, ImGui::GetFrameHeight() };

            glm::vec2 cellPosF = (glm::vec2{ viewportMousePos.x, viewportMousePos.y } / glm::vec2{ viewPortSize.x, viewPortSize.y }) * glm::vec2{ boardSize.x, boardSize.y };

            glm::ivec2 cellPos{ cellPosF.x, cellPosF.y };

            if (ImGui::IsMouseDown(0)) {
                if (cellPos.x >= 0 && cellPos.y >= 0 && cellPos.x < boardSize.x && cellPos.y < boardSize.y) {
                    setCells.emplace_back(cellPos);
                }
            }

            if (ImGui::IsMouseDown(1)) {
                if (cellPos.x >= 0 && cellPos.y >= 0 && cellPos.x < boardSize.x && cellPos.y < boardSize.y) {
                    clearCells.emplace_back(cellPos);
                }
            }
        }
        ImGui::End();

        if (!setCells.empty()) {
            SetCells(setCells);

            setCells.clear();
        }

        if (!clearCells.empty()) {
            ClearCells(clearCells);

            clearCells.clear();
        }

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
