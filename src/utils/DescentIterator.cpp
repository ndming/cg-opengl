// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <chrono>
#include <stdexcept>

#include "utils/DescentIterator.h"

DescentIterator::Builder& DescentIterator::Builder::convergenceRate(const float rate) {
    _convergenceRate = rate;
    return *this;
}

DescentIterator::Builder &DescentIterator::Builder::gradientX(std::function<float(float, float)> gradient) noexcept {
    _gradientX = std::move(gradient);
    return *this;
}

DescentIterator::Builder &DescentIterator::Builder::gradientY(std::function<float(float, float)> gradient) noexcept {
    _gradientY = std::move(gradient);
    return *this;
}

std::unique_ptr<DescentIterator> DescentIterator::Builder::build() {
    if (!_gradientX || !_gradientY) {
        throw std::runtime_error("DescentIterator: Both gradientX and gradient Y must be set.");
    }
    return std::unique_ptr<DescentIterator>{
        new DescentIterator{
            std::move(_gradientX), std::move(_gradientY), _convergenceRate
        }
    };
}

DescentIterator::DescentIterator(
    std::function<float(float, float)> &&gradientX,
    std::function<float(float, float)> &&gradientY,
    const float convergenceRate
) : _gradientX{ gradientX }, _gradientY{ gradientY }, _convergenceRate{ convergenceRate } {}

std::mt19937 DescentIterator::initGenerator() {
    auto rd = std::random_device{};
    auto ss = std::seed_seq{
            static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count()),
            rd(), rd(), rd(), rd(), rd(), rd(), rd()
    };
    return std::mt19937{ ss };
}

void DescentIterator::resetState(const float x, const float y) {
    _x = x;
    _y = y;
}

void DescentIterator::randomState(const float halfExtentX, const float halfExtentY) {
    auto distX = std::uniform_real_distribution{ -halfExtentX, halfExtentX };
    _x = distX(_generator);
    auto distY = std::uniform_real_distribution{ -halfExtentY, halfExtentY };
    _y = distY(_generator);
}

void DescentIterator::randomState(const float halfExtent) {
    randomState(halfExtent, halfExtent);
}

void DescentIterator::iterate(const float halfExtentX, const float halfExtentY) {
    const auto tmpX = _x;
    const auto tmpY = _y;
    _x = tmpX - _convergenceRate * _gradientX(tmpX, tmpY);
    if (std::abs(_x) > halfExtentX) {
        _x = (_x < 0) ? -halfExtentX : halfExtentX;
    }
    _y = tmpY - _convergenceRate * _gradientY(tmpX, tmpY);
    if (std::abs(_y) > halfExtentY) {
        _y = (_y < 0) ? -halfExtentY : halfExtentY;
    }
}

void DescentIterator::iterate(float halfExtent) {
    iterate(halfExtent, halfExtent);
}

std::pair<float, float> DescentIterator::getState() const {
    return std::make_pair(_x, _y);
}
