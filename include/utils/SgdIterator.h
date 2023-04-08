// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <functional>
#include <memory>
#include <random>

class SgdIterator {
public:
    class Builder {
    public:
        Builder& convergenceRate(float rate);

        Builder& gradientX(std::function<float(float, float)> gradient);
        Builder& gradientY(std::function<float(float, float)> gradient);

        std::unique_ptr<SgdIterator> build();

    private:
        std::function<float(float, float)> _gradientX{ [](const auto x, const auto y){ return 2 * x; }};
        std::function<float(float, float)> _gradientY{ [](const auto x, const auto y){ return 2 * y; }};

        float _convergenceRate{ 0.1f };
    };

    [[nodiscard]] std::pair<float, float> getState() const;

    void resetState(float x, float y);

    void randomState(float halfExtentX, float halfExtentY);

    void iterate();

private:
    SgdIterator(
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