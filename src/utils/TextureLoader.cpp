// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <iostream>
#include <stb_image.h>
#include <stdexcept>

#include "utils/TextureLoader.h"

GLuint loadTexture(const std::string_view path) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, channels;
    // OpenGL expects the 0.0 coordinate on the y-axis to be on the bottom side of the image,
    // but images usually have 0.0 at the top of the y-axis.
    stbi_set_flip_vertically_on_load(true);
    const auto data = stbi_load(path.data(), &width, &height, &channels, 0);
    if (data) {
        GLint format;
        switch (channels) {
            case 1:
                format = GL_RED;
                break;
            case 3:
                format = GL_RGB;
                break;
            default:
                format = GL_RGBA;
        }
        glBindTexture(GL_TEXTURE_2D, textureID);
        // Start generating a texture using the previously loaded image data.
        // 1st param: this operation will generate a texture on the currently bound texture object at the same target.
        // 2nd param: specifies the mipmap level for which we want to create a texture for.
        // 3rd param: what kind of format we want to store the texture.
        // 4th, 5th param: the width and height of the resulting texture.
        // 6th param: should always be 0 (some legacy stuff).
        // 7th, 8th param: the format and data type of the source image
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        // This will automatically generate all the required mipmaps for the currently bound texture.
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set the texture wrapping/filtering options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        std::cerr << "Texture failed to load at: " << path << '\n';
        stbi_image_free(data);
        throw std::invalid_argument("Failed to load texture.");
    }
    stbi_image_free(data);
    return textureID;
}
