// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <functional>

#include "Drawable.h"

class Ring : public Drawable {
public:
    ~Ring() = default;
    Ring(const Ring&) = delete;
    Ring(Ring&&) noexcept = delete;
    Ring& operator=(const Ring&) = delete;
    Ring& operator=(Ring&&) noexcept = delete;

    class Builder final : public Drawable::Builder {
    public:
        Builder& radius(const float radius) {
            _radius = radius;
            return *this;
        }

        Builder& thickness(const float thickness) {
            _thickness = thickness;
            return *this;
        }

        Builder& segments(const int segments) {
            _segments = segments;
            return *this;
        }

        std::unique_ptr<Drawable> build(Engine& engine) override;

    private:
        float _radius{ 1.0f };
        float _thickness{ 1.0f };
        int _segments{ 100 };
    };

private:
    Ring(const Entity entity, Shader* const shader) : Drawable(entity, shader) {}
};
