// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <functional>

#include "drawable/Drawable.h"
#include "drawable/Mesh.h"

class Contour : public Drawable {
public:
    ~Contour() = default;
    Contour(const Contour&) = delete;
    Contour(Contour&&) noexcept = delete;
    Contour& operator=(const Contour&) = delete;
    Contour& operator=(Contour&&) noexcept = delete;

    class Builder final : public Mesh::Builder {
    public:
        explicit Builder(std::function<float(float, float)> func) : Mesh::Builder(std::move(func)) {}

        Builder& low(float lo);

        Builder& high(float hi);

        std::unique_ptr<Drawable> build(Engine& engine) override;

    private:
        float _lo{ -1.0f };
        float _hi{  1.0f };

        [[nodiscard]] float mapHeat(float z) const;
    };

private:
    explicit Contour(const Entity entity, Shader* const shader) : Drawable(entity, shader) {}
};