// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <array>

class Engine;

class Skybox {
public:
    class Builder {
    public:
        Builder& color(float r, float g, float b, float a);

        Skybox* build(Engine& engine);

    private:
        std::array<float, 4> _color{ 0.0f, 0.0f, 0.0f, 1.0f };
    };

    [[nodiscard]] std::array<float, 4> getColor() const;

private:
    explicit Skybox(const std::array<float, 4>& color) : _color{ color } {}

    const std::array<float, 4> _color;

    friend class Engine;
};

