#include "s3e/Core/FrameBuffer.hpp"

#include "s3e/Support/log.hpp"

namespace Tom::s3e {
    FrameBufferTexture::FrameBufferTexture()
        : FrameBufferTexture(GL_COLOR_ATTACHMENT0, {0, 0}) {}

    FrameBufferTexture::FrameBufferTexture(GLuint attachment,
                                           sf::Vector2u size,
                                           GLint internal,
                                           GLenum format,
                                           GLenum type,
                                           GLint magFilter,
                                           GLint minFilter,
                                           GLint wrap,
                                           GLsizei samples)
        : Texture(
            size, internal, format, type, magFilter, minFilter, wrap, false, samples),
          attachment(attachment) {}

    FrameBufferTexture::~FrameBufferTexture() {
        Texture::~Texture();
    }

    GLuint FrameBufferTexture::getAttachment() const {
        return attachment;
    }

    void FrameBufferTexture::setAttachment(GLuint attachment) {
        this->attachment = attachment;
    }
}

namespace Tom::s3e {
    FrameBuffer::FrameBuffer(sf::Vector2u size,
                             std::vector<FrameBufferAttachment> attachments,
                             bool depthBuffer,
                             GLsizei samples)
        : size(size), rboDepth(0), samples(samples), resolved(nullptr) {
        glGenFramebuffers(1, &fboId);

        bind();

        for (auto & a : attachments) {
            auto texture = std::make_shared<FrameBufferTexture>(
                a.attachment, size, a.internal, a.format, a.type, GL_NEAREST,
                GL_NEAREST, GL_CLAMP, samples);
            GLenum textarget =
                (samples > 0 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D);
            glFramebufferTexture2D(GL_FRAMEBUFFER, a.attachment, textarget,
                                   texture->getTextureId(), 0);
            textures.push_back(texture);
        }

        if (depthBuffer) {
            glGenRenderbuffers(1, &rboDepth);
            glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);

            if (samples > 0)
                glRenderbufferStorageMultisample(
                    GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT, size.x, size.y);
            else
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                                      size.x, size.y);

            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                      GL_RENDERBUFFER, rboDepth);
        }

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            Logging::Core->error("FrameBuffer is not complete");

        std::vector<GLuint> buffers;
        for (int i = 0; i < count(); i++) {
            auto a = textures[i]->getAttachment();
            if (a != GL_DEPTH_ATTACHMENT && a != GL_STENCIL_ATTACHMENT)
                buffers.push_back(a);
        }
        glDrawBuffers(buffers.size(), &buffers[0]);

        unbind();

        if (samples > 0)
            resolved =
                std::make_shared<FrameBuffer>(size, attachments, depthBuffer, 0);
    }

    FrameBuffer::~FrameBuffer() {
        glDeleteFramebuffers(1, &fboId);
        if (rboDepth > 0)
            glDeleteRenderbuffers(1, &rboDepth);
    }

    GLuint FrameBuffer::getId() const {
        return fboId;
    }

    const sf::Vector2u & FrameBuffer::getSize() const {
        return size;
    }

    void FrameBuffer::setSize(sf::Vector2u size) {
        this->size = size;
        for (auto & texture : textures) {
            texture->setSize(size);
        }
        if (rboDepth > 0) {
            glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
            if (samples > 0)
                glRenderbufferStorageMultisample(
                    GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT, size.x, size.y);
            else
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                                      size.x, size.y);
        }

        if (resolved)
            resolved->setSize(size);
    }

    const std::vector<FrameBufferTexture::Ptr> & FrameBuffer::getTextures() const {
        return textures;
    }

    bool FrameBuffer::isMultisampled() {
        return samples > 0;
    }

    size_t FrameBuffer::count() const {
        return textures.size();
    }

    void FrameBuffer::blit(GLint dest, GLbitfield bitfield) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, fboId);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dest);
        if (count() > 1) {
            for (int i = 0; i < count(); i++) {
                auto a = textures[i]->getAttachment();
                if (a != GL_DEPTH_ATTACHMENT && a != GL_STENCIL_ATTACHMENT) {
                    glReadBuffer(a);
                    glDrawBuffer(a);
                    glBlitFramebuffer(0, 0, size.x, size.y, 0, 0, size.x,
                                      size.y, bitfield, GL_NEAREST);
                }
            }
        }
        else
            glBlitFramebuffer(0, 0, size.x, size.y, 0, 0, size.x, size.y,
                              bitfield, GL_NEAREST);
    }

    FrameBuffer::Ptr & FrameBuffer::getResovled() {
        if (resolved)
            blit(resolved->getId(), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT
                                        | GL_STENCIL_BUFFER_BIT);
        return resolved;
    }

    void FrameBuffer::bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    }

    void FrameBuffer::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
