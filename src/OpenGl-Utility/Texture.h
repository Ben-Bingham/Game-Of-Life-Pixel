#pragma once
#include <glm/vec2.hpp>

class Texture {
public:
    enum class Format {
        RGB,
        RGBA
    };

	Texture(Format, glm::ivec2 size);
    Texture(const Texture& other) = delete;
    Texture(Texture&& other) noexcept = default;
    Texture& operator=(const Texture& other) = delete;
    Texture& operator=(Texture&& other) noexcept = default;

private:
    unsigned int m_TextureHandle;
};