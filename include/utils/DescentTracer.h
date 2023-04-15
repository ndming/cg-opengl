// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <glm/vec3.hpp>
#include <functional>
#include <memory>

#include "../Engine.h"
#include "../Scene.h"
#include "Drawable.h"

class DescentTracer {
public:
    class Builder {
    public:
        Builder& objective(std::function<float(float, float)> function) noexcept;
        Builder& gradientX(std::function<float(float, float)> gradient) noexcept;
        Builder& gradientY(std::function<float(float, float)> gradient) noexcept;

        Builder& traceSize(float size);
        Builder& heightPadding(float padding);
        Builder& traceColor(float r, float g, float b);
        Builder& markColor(float r, float g, float b);

        std::unique_ptr<DescentTracer> build();

    private:
        std::function<float(float, float)> _objective{ nullptr };
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
    DescentTracer(
        std::function<float(float, float)>&& objective,
        std::function<float(float, float)>&& gradientX,
        std::function<float(float, float)>&& gradientY,
        const float traceSize, const float heightPadding, const glm::vec3& traceColor, const glm::vec3& markColor
    ) noexcept : _objective{ objective }, _gradientX{ gradientX }, _gradientY{ gradientY },
    _traceSize{ traceSize }, _heightPadding{ heightPadding }, _traceColor{ traceColor }, _markColor{ markColor } {}

    const std::function<float(float, float)> _objective;
    const std::function<float(float, float)> _gradientX;
    const std::function<float(float, float)> _gradientY;

    const float _traceSize;
    const float _heightPadding;
    const glm::vec3 _traceColor;
    const glm::vec3 _markColor;

    std::vector<std::unique_ptr<Drawable>> _currentTraces{};

    float _currentX{ 0.0f };
    float _currentY{ 0.0f };

    [[nodiscard]] glm::vec3 getNormalAt(float x, float y) const;
};
