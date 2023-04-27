// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <functional>

#include "Drawable.h"

class Orbit : public Drawable {
public:
    ~Orbit() = default;
    Orbit(const Orbit&) = delete;
    Orbit(Orbit&&) noexcept = delete;
    Orbit& operator=(const Orbit&) = delete;
    Orbit& operator=(Orbit&&) noexcept = delete;

    class Builder final : protected Drawable::Builder {
    public:
        Builder(const std::function<float(float)>& orbitX, const std::function<float(float)>& orbitY)
        : _orbitX{ orbitX }, _orbitY{ orbitY } {
        }

        Builder& color(const float r, const float g, const float b) {
            _color.r = r; _color.g = g; _color.b = b;
            return *this;
        }

        Builder& segments(const int segments) {
            _segments = segments;
            return *this;
        }

        std::unique_ptr<Drawable> build(Engine& engine) override;

    private:
        const std::function<float(float)> _orbitX;
        const std::function<float(float)> _orbitY;

        glm::vec3 _color{ 1.0f, 1.0f, 1.0f };
        int _segments{ 1000 };
    };

private:
    Orbit(const Entity entity, Shader* const shader) : Drawable(entity, shader) {}
};
