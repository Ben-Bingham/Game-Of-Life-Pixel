#pragma once

#include <string>

#include <glm/glm.hpp>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

class ShaderProgram {
public:
    ShaderProgram();
    ShaderProgram(const ShaderProgram& other) = delete;
    ShaderProgram(ShaderProgram&& other) noexcept = default;
    ShaderProgram& operator=(const ShaderProgram& other) = delete;
    ShaderProgram& operator=(ShaderProgram&& other) noexcept = default;
    ~ShaderProgram();

    void Bind();

    unsigned int Get();

    enum class ShaderType {
        VERTEX = GL_VERTEX_SHADER,
        FRAGMENT = GL_FRAGMENT_SHADER,
        GEOMETRY = GL_GEOMETRY_SHADER,
        COMPUTE = GL_COMPUTE_SHADER
    };

    void AddShader(const std::string& shaderPath, ShaderType shaderType);

    void Link();

    void SetFloat(const std::string& name, const float& value);
    void SetInt(const std::string& name, const int& value);

    void SetVec2(const std::string& name, const glm::vec2& value);
    void SetVec3(const std::string& name, const glm::vec3& value);
    void SetVec4(const std::string& name, const glm::vec4& value);
    void SetMat4(const std::string& name, const glm::mat4& value);

    void SetVec2(const std::string& name, const glm::ivec2& value);

    void SetBool(const std::string& name, const bool& value);

protected:
    unsigned int m_ShaderProgramHandle;

    std::vector<unsigned int> m_Shaders;
};
