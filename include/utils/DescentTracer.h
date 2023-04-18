// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <glm/vec3.hpp>
#include <functional>
#include <memory>

#include "../Engine.h"
#include "../Scene.h"
#include "drawable/Drawable.h"
#include "drawable/Material.h"

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

        Builder& usePhong(bool enable);
        Builder& traceMaterial(const phong::Material& material);
        Builder& markMaterial(const phong::Material& material);

        std::unique_ptr<DescentTracer> build();

    private:
        std::function<float(float, float)> _objective{ nullptr };
        std::function<float(float, float)> _gradientX{ nullptr };
        std::function<float(float, float)> _gradientY{ nullptr };

        float _traceSize{ 0.1f };
        float _heightPadding{ 0.05f };
        glm::vec3 _traceColor{ 1.0f, 1.0f, 1.0f };
        glm::vec3 _markColor{ 1.0f, 0.0f, 0.0f };

        bool _usePhong{ false };
        phong::Material _traceMaterial{ phong::OBSIDIAN };
        phong::Material _markMaterial{ phong::GOLD };
    };

    void resetTo(float x, float y, Scene& scene, Engine& engine);

    void traceTo(float x, float y, Scene& scene, Engine& engine);

private:
    DescentTracer(
        std::function<float(float, float)>&& objective,
        std::function<float(float, float)>&& gradientX,
        std::function<float(float, float)>&& gradientY,
        const float traceSize, const float heightPadding, const glm::vec3& traceColor, const glm::vec3& markColor,
        const bool usePhong, const phong::Material& traceMaterial, const phong::Material& markMaterial
    ) noexcept : _objective{ objective }, _gradientX{ gradientX }, _gradientY{ gradientY },
    _traceSize{ traceSize }, _heightPadding{ heightPadding }, _traceColor{ traceColor }, _markColor{ markColor },
    _usePhong{ usePhong }, _traceMaterial{ traceMaterial }, _markMaterial{ markMaterial } {}

    const std::function<float(float, float)> _objective;
    const std::function<float(float, float)> _gradientX;
    const std::function<float(float, float)> _gradientY;

    const float _traceSize;
    const float _heightPadding;
    const glm::vec3 _traceColor;
    const glm::vec3 _markColor;

    const bool _usePhong;
    const phong::Material _traceMaterial;
    const phong::Material _markMaterial;

    std::vector<std::unique_ptr<Drawable>> _currentTraces{};

    float _currentX{ 0.0f };
    float _currentY{ 0.0f };

    [[nodiscard]] glm::vec3 getNormalAt(float x, float y) const;
};
