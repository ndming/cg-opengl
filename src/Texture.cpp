// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <algorithm>
#include <cmath>

#include "Texture.h"
#include "Engine.h"

Texture::Builder &Texture::Builder::width(const int width) {
    _width = width;
    return *this;
}

Texture::Builder &Texture::Builder::height(const int height) {
    _height = height;
    return *this;
}

Texture::Builder &Texture::Builder::format(const Texture::InternalFormat format) {
    _format = format;
    return *this;
}

Texture::Builder &Texture::Builder::sampler(const Texture::Sampler sampler) {
    _sampler = sampler;
    return *this;
}

Texture *Texture::Builder::build(Engine &engine) const {
    GLuint textureID;
    glGenTextures(1, &textureID);

    const auto target = static_cast<GLenum>(_sampler);
    const auto internalFormat = static_cast<GLint>(_format);
    const auto width = static_cast<GLsizei>(_width);
    const auto height = static_cast<GLsizei>(_height);

    // Set the texture wrapping/filtering options
    glBindTexture(target, textureID);
    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(target, 0);

    const auto texture = new Texture(textureID, target, internalFormat, width, height);
    engine._textures.insert(texture);

    return texture;
}

Texture::Texture(
    const GLuint textureID, const GLenum target, const GLint internalFormat, const GLsizei width, const GLsizei height
) : _textureID{ textureID }, _target{ target }, _internalFormat{ internalFormat }, _width{ width }, _height{ height } {}

void Texture::setImage(const int level, const Texture::PixelBufferDescriptor &descriptor) const {
    glBindTexture(_target, _textureID);
    // 1st param: this operation will generate a texture on the currently bound texture object at the same target.
    // 2nd param: specifies the mipmap level for which we want to create a texture for.
    // 3rd param: what kind of format we want to store the texture.
    // 4th, 5th param: the width and height of the resulting texture.
    // 6th param: should always be 0 (some legacy stuff).
    // 7th, 8th param: the format and data type of the source image
    const auto format = static_cast<GLenum>(descriptor.format);
    const auto type = static_cast<GLenum>(descriptor.type);
    glTexImage2D(_target, level, _internalFormat, _width, _height, 0, format, type, descriptor.data);

    // glBindTexture(_target, 0);
}

void Texture::generateMipmaps() const {
    glBindTexture(_target, _textureID);
    // This will automatically generate all the required mipmaps for the currently bound texture.
    glGenerateMipmap(_target);
    // glBindTexture(_target, 0);
}

GLuint Texture::getNativeObject() const {
    return _textureID;
}

GLenum Texture::getTarget() const {
    return _target;
}
