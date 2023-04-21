// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <iostream>
#include <stb_image.h>
#include <stdexcept>

#include "utils/TextureLoader.h"

Texture::InternalFormat internalFormat(int channels);
Texture::Format format(int channels);

Texture* loadTexture(const std::string_view name, Engine& engine) {
    const auto path = std::string{ "res/textures/" } + name.data();
    int width, height, channels;
    stbi_set_flip_vertically_on_load(false);
    const auto data = stbi_load(path.data(), &width, &height, &channels, 0);
    if (data) {
        std::cout << name << ": ";
        std::cout << "width=" << width << " | height=" << height << " | channels=" << channels << '\n';
        const auto texture = Texture::Builder()
                .width(width)
                .height(height)
                .sampler(Texture::Sampler::SAMPLER_2D)
                .format(internalFormat(channels))
                .build(engine);

        const auto descriptor = Texture::PixelBufferDescriptor{ data, format(channels), Texture::Type::UBYTE };
        texture->setImage(0, descriptor);

        texture->generateMipmaps();

        stbi_image_free(data);
        return texture;
    } else {
        std::cerr << "Texture failed to load at: " << path << '\n';
        stbi_image_free(data);
        throw std::invalid_argument("Failed to load texture.");
    }
}

Texture::InternalFormat internalFormat(const int channels) {
    switch (channels) {
        case 1:
            return Texture::InternalFormat::R8;
        case 2:
            return Texture::InternalFormat::RG8;
        case 3:
            return Texture::InternalFormat::RGB8;
        default:
            return Texture::InternalFormat::RGBA8;
    }
}

Texture::Format format(const int channels) {
    switch (channels) {
        case 1:
            return Texture::Format::R;
        case 3:
            return Texture::Format::RGB;
        default:
            return Texture::Format::RGBA;
    }
}