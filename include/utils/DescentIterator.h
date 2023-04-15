// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <functional>
#include <memory>
#include <random>

class DescentIterator {
public:
    class Builder {
    public:
        Builder& convergenceRate(float rate);

        Builder& gradientX(std::function<float(float, float)> gradient) noexcept;
        Builder& gradientY(std::function<float(float, float)> gradient) noexcept;

        std::unique_ptr<DescentIterator> build();

    private:
        std::function<float(float, float)> _gradientX{ nullptr };
        std::function<float(float, float)> _gradientY{ nullptr };

        float _convergenceRate{ 0.1f };
    };

    [[nodiscard]] std::pair<float, float> getState() const;

    void resetState(float x, float y);

    void randomState(float halfExtentX, float halfExtentY);

    void iterate();

private:
    DescentIterator(
        std::function<float(float, float)>&& gradientX,
        std::function<float(float, float)>&& gradientY,
        float convergenceRate
    );

    const std::function<float(float, float)> _gradientX;
    const std::function<float(float, float)> _gradientY;

    const float _convergenceRate{ 0.1f };

    float _x{ 0.0f };
    float _y{ 0.0f };

    std::mt19937 _generator{ initGenerator() };

private:
    static std::mt19937 initGenerator();
};