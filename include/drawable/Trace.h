// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include "Drawable.h"

class Trace : public Drawable {
public:
    ~Trace() = default;
    Trace(const Trace&) = delete;
    Trace(Trace&&) noexcept = delete;
    Trace& operator=(const Trace&) = delete;
    Trace& operator=(Trace&&) noexcept = delete;

    class Builder final : protected Drawable::Builder {
    public:
        Builder& position(float x, float y, float z);
        Builder& direction(const glm::vec3& direction);
        Builder& normal(const glm::vec3& normal);
        Builder& color(float r, float g, float b);
        Builder& size(float size);

        std::unique_ptr<Drawable> build(Engine& engine) override;

    private:
        glm::vec3 _position{ 0.0f, 0.0f, 0.0f };
        glm::vec3 _direction{ 0.0f, 1.0f, 0.0f };
        glm::vec3 _normal{ 0.0f, 0.0f, 1.0f };
        glm::vec3 _color{ 1.0f, 1.0f, 1.0f };
        float _size{ 0.15f };
    };

private:
    explicit Trace(const Entity entity, Shader* const shader) : Drawable(entity, shader) {}
};