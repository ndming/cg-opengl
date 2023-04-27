// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <glm/vec3.hpp>

namespace phong {
    struct Material {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;
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

    static constexpr auto RUBY = Material{
        glm::vec3{ 0.1745f, 0.01175f, 0.01175f },
        glm::vec3{ 0.61424f, 0.04136f, 0.04136f },
        glm::vec3{ 0.727811f, 0.626959f, 0.626959f },
        0.6f * 128
    };

    static constexpr auto TURQUOISE = Material{
        glm::vec3{ 0.1f, 0.18725f, 0.1745f },
        glm::vec3{ 0.396f, 0.74151f, 0.69102f },
        glm::vec3{ 0.297254f, 0.30829f, 0.306678f },
        0.1f * 128
    };

    static constexpr auto COPPER = Material{
        glm::vec3{ 0.19125f, 0.0735f, 0.0225f },
        glm::vec3{ 0.7038f, 0.27048f, 0.0828f },
        glm::vec3{ 0.256777f, 0.137622f, 0.086014f },
        0.1f * 128
    };

    static constexpr auto SILVER = Material{
        glm::vec3{ 0.19225f, 0.19225f, 0.19225f },
        glm::vec3{ 0.50754f, 0.50754f, 0.50754f },
        glm::vec3{ 0.508273f,	0.508273f, 0.508273f },
        0.4f * 128
    };

    static constexpr auto GOLD = Material{
        glm::vec3{ 0.24725f, 0.1995f, 0.0745f },
        glm::vec3{ 0.75164f, 0.60648f, 0.22648f },
        glm::vec3{ 0.628281f, 0.555802f,	0.366065f },
        0.4f * 128
    };

    static constexpr auto CHROME = Material{
        glm::vec3{ 0.25f, 0.25f, 0.25f },
        glm::vec3{ 0.4f, 0.4f, 0.4f },
        glm::vec3{ 0.774597f, 0.774597f, 0.774597f },
        0.6f * 128
    };

    static constexpr auto WHITE_PLASTIC = Material{
        glm::vec3{ 0.0f, 0.0f, 0.0f },
        glm::vec3{ 0.55f, 0.55f, 0.55f },
        glm::vec3{ 0.70f, 0.70f, 0.70f },
        0.25f * 128
    };

    static constexpr auto CYAN_PLASTIC = Material{
        glm::vec3{ 0.0f, 0.1f, 0.06f },
        glm::vec3{ 0.0f, 0.50980392f, 0.50980392f },
        glm::vec3{ 0.50196078f, 0.50196078f, 0.50196078f },
        0.25f * 128
    };
}
