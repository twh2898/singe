#include "s3e/Graphics/Texture.hpp"

#include <SFML/Graphics/Image.hpp>

#include "s3e/Support/log.hpp"

namespace Tom::s3e {
    Texture::Texture(const sf::Image & image,
                     GLint magFilter,
                     GLint minFilter,
                     GLint wrap,
                     bool mipmaps)
        : textureId(0),
          size(image.getSize()),
          internal(GL_RGBA),
          format(GL_RGBA),
          type(GL_UNSIGNED_BYTE),
          samples(0),
          target(GL_TEXTURE_2D),
          minFilter(minFilter),
          magFilter(magFilter),
          wrap(wrap),
          mipmaps(mipmaps) {

        glGenTextures(1, &textureId);
        Logging::Graphics->debug("generated opengl texture: {} samples = {}",
                                 textureId, samples);

        loadFrom(image);
    }

    Texture::Texture(const sf::Vector2u & size,
                     GLint internal,
                     GLenum format,
                     GLenum type,
                     GLsizei samples,
                     GLint magFilter,
                     GLint minFilter,
                     GLint wrap,
                     bool mipmaps)
        : textureId(0),
          size(size),
          internal(internal),
          format(format),
          type(type),
          samples(samples),
          target(samples > 0 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D),
          minFilter(minFilter),
          magFilter(magFilter),
          wrap(wrap),
          mipmaps(mipmaps) {

        glGenTextures(1, &textureId);
        Logging::Graphics->debug("generated opengl texture: {} samples = {}",
                                 textureId, samples);

        resize(size);
    }

    Texture::~Texture() {
        if (textureId > 0) {
            Logging::Graphics->debug("deleting opengl texture: {}", textureId);
            glDeleteTextures(1, &textureId);
        }
    }

    void Texture::loadFrom(const sf::Image & image) {
        bind();
        glTexImage2D(target, 0, internal, size.x, size.y, 0, format, type,
                     image.getPixelsPtr());

        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);

        glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);

        if (mipmaps)
            glGenerateMipmap(target);
        unbind();
    }

    GLuint Texture::getTextureId() const {
        return textureId;
    }

    const sf::Vector2u & Texture::getSize() const {
        return size;
    }

    void Texture::resize(const sf::Vector2u & size) {
        this->size = size;
        if (size.x > 0 && size.y > 0) {
            bind();
            if (samples > 0) {
                glTexImage2DMultisample(target, samples, internal, size.x,
                                        size.y, GL_TRUE);
            }
            else {
                glTexImage2D(target, 0, internal, size.x, size.y, 0, format,
                             type, NULL);

                glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
                glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);

                glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
                glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
            }
            unbind();
        }
    }

    void Texture::bind() {
        glBindTexture(target, textureId);
    }

    void Texture::unbind() {
        glBindTexture(target, 0);
    }
}
