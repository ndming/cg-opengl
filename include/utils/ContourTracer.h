// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <functional>
#include <glm/vec3.hpp>
#include <memory>

#include "Engine.h"
#include "Scene.h"
#include "drawable/Drawable.h"
#include "drawable/Material.h"

class ContourTracer {
public:
    class Builder {
    public:
        Builder& gradientX(std::function<float(float, float)> gradient) noexcept;
        Builder& gradientY(std::function<float(float, float)> gradient) noexcept;

        Builder& traceSize(float size);
        Builder& heightPadding(float padding);

        Builder& traceColor(float r, float g, float b);
        Builder& markColor(float r, float g, float b);

        std::unique_ptr<ContourTracer> build();

    private:
        std::function<float(float, float)> _gradientX{ nullptr };
        std::function<float(float, float)> _gradientY{ nullptr };

        float _traceSize{ 0.1f };
        float _heightPadding{ 0.05f };
        glm::vec3 _traceColor{ 1.0f, 1.0f, 1.0f };
        glm::vec3 _markColor{ 1.0f, 0.0f, 0.0f };
    };

    void resetTo(float x, float y, Scene& scene, Engine& engine);

    void traceTo(float x, float y, Scene& scene, Engine& engine);

private:
    ContourTracer(
            std::function<float(float, float)>&& gradientX,
            std::function<float(float, float)>&& gradientY,
            const float traceSize, const float heightPadding, const glm::vec3& traceColor, const glm::vec3& markColor
    ) noexcept : _gradientX{ gradientX }, _gradientY{ gradientY },
    _traceSize{ traceSize }, _heightPadding{ heightPadding }, _traceColor{ traceColor }, _markColor{ markColor } {}

    const std::function<float(float, float)> _gradientX;
    const std::function<float(float, float)> _gradientY;

    const float _traceSize;
    const float _heightPadding;
    const glm::vec3 _traceColor;
    const glm::vec3 _markColor;

    std::vector<std::unique_ptr<Drawable>> _currentTraces{};

    float _currentX{ 0.0f };
    float _currentY{ 0.0f };
};
