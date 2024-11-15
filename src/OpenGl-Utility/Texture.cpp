#include "Texture.h"
#include <vector>

Texture::Texture(glm::ivec2 size, Format format, StorageType storageType, std::vector<unsigned char> data) {
    glGenTextures(1, &m_TextureHandle);
    glBindTexture(GL_TEXTURE_2D, m_TextureHandle);

    // Set default parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (data.empty()) {
        glTexImage2D(GL_TEXTURE_2D, 0, (int)format, size.x, size.y, 0, (int)format, (int)storageType, nullptr);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, (int)format, size.x, size.y, 0, (int)format, (int)storageType, data.data());
    }
}

Texture::~Texture() {
    glDeleteTextures(1, &m_TextureHandle);
}

void Texture::Bind() {
    glBindTexture(GL_TEXTURE_2D, m_TextureHandle);
}

unsigned Texture::Get() {
    return m_TextureHandle;
}
