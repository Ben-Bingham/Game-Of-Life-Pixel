#pragma once
#include <vector>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <glm/vec2.hpp>

class Texture {
public:
    enum class Format {
        R = GL_RED,
        RGB = GL_RGB,
        RGBA = GL_RGBA,
        RGBA16F = GL_RGBA16F
    };

    enum class StorageType {
        UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
        FLOAT = GL_FLOAT
    };

    Texture(glm::ivec2 size, Format format = Format::RGB, StorageType storageType = StorageType::UNSIGNED_BYTE, std::vector<unsigned char> data = std::vector<unsigned char>{ });
    Texture(const Texture& other) = delete;
    Texture(Texture&& other) noexcept = default;
    Texture& operator=(const Texture& other) = delete;
    Texture& operator=(Texture&& other) noexcept = default;
    ~Texture();

    void Bind();

    unsigned int Get();

private:
    unsigned int m_TextureHandle;
};