// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <glm/vec3.hpp>

namespace phong {
    struct Material {
        const glm::vec3 ambient;
        const glm::vec3 diffuse;
        const glm::vec3 specular;
        const float shininess;
    };

    static constexpr auto EMERALD = Material{
        glm::vec3{ 0.0215f, 0.1745f, 0.0215f },
        glm::vec3{ 0.07568f, 0.61424f, 0.07568f },
        glm::vec3{ 0.633f, 0.727811f, 0.633f },
        0.6f * 128
    };

    static constexpr auto JADE = Material{
        glm::vec3{ 0.135f, 0.2225f, 0.1575f },
        glm::vec3{ 0.54f, 0.89f, 0.63f },
        glm::vec3{ 0.316228f, 0.316228f, 0.316228f },
        0.1f * 128
    };

    static constexpr auto OBSIDIAN = Material{
            glm::vec3{ 0.05375f, 0.05f, 0.06625f },
            glm::vec3{ 0.18275f, 0.17f, 0.22525f },
            glm::vec3{ 0.332741f, 0.328634f, 0.346435f },
            0.3f * 128
    };

    static constexpr auto PEARL = Material{
            glm::vec3{ 0.25f, 0.20725f, 0.20725f },
            glm::vec3{ 1.0f, 0.829f, 0.829f },
            glm::vec3{ 0.296648f, 0.296648f, 0.296648f },
            0.088f * 128
    };
}
