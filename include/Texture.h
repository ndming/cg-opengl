// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <glad/glad.h>

class Engine;

class Texture {
public:
    ~Texture() = default;
    Texture(const Texture&) = delete;
    Texture(Texture&&) noexcept = delete;
    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) = delete;

    enum class InternalFormat {
        R8 = GL_RED,
        RG8 = GL_RG,
        RGB8 = GL_RGB,
        RGBA8 = GL_RGBA
    };

    enum class Sampler {
        SAMPLER_2D = GL_TEXTURE_2D,
    };

    class Builder {
    public:
        Builder& width(int width);
        Builder& height(int height);
        Builder& format(InternalFormat format);
        Builder& sampler(Sampler sampler);

        Texture* build(Engine& engine) const;

    private:
        int _width{ 600 };
        int _height{ 600 };
        InternalFormat _format{ InternalFormat::RGBA8 };
        Sampler _sampler{ Sampler::SAMPLER_2D };
    };

    enum class Format {
        R = GL_RED,
        RGB = GL_RGB,
        RGBA = GL_RGBA,
    };

    enum class Type {
        USHORT = GL_UNSIGNED_SHORT,
        UBYTE = GL_UNSIGNED_BYTE
    };

    struct PixelBufferDescriptor {
        void* data;
        Format format;
        Type type;
    };

    [[nodiscard]] GLuint getNativeObject() const;
    [[nodiscard]] GLenum getTarget() const;

    void setImage(int level, const PixelBufferDescriptor& descriptor) const;

    void generateMipmaps() const;

private:
    Texture(GLuint textureID, GLenum target, GLint internalFormat, GLsizei width, GLsizei height);

    const GLuint _textureID;

    const GLenum _target;
    const GLint _internalFormat;
    const GLsizei _width;
    const GLsizei _height;
};
