// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <chrono>

#include "utils/SgdIterator.h"

SgdIterator::Builder& SgdIterator::Builder::convergenceRate(const float rate) {
    _convergenceRate = rate;
    return *this;
}

SgdIterator::Builder &SgdIterator::Builder::gradientX(std::function<float(float, float)> gradient) {
    _gradientX = std::move(gradient);
    return *this;
}

SgdIterator::Builder &SgdIterator::Builder::gradientY(std::function<float(float, float)> gradient) {
    _gradientY = std::move(gradient);
    return *this;
}

std::unique_ptr<SgdIterator> SgdIterator::Builder::build() {
    return std::unique_ptr<SgdIterator>{
        new SgdIterator{std::move(_gradientX),std::move(_gradientY),_convergenceRate
        }
    };
}

SgdIterator::SgdIterator(
    std::function<float(float, float)> &&gradientX,
    std::function<float(float, float)> &&gradientY,
    const float convergenceRate
) : _gradientX{ gradientX }, _gradientY{ gradientY }, _convergenceRate{ convergenceRate } {}

std::mt19937 SgdIterator::initGenerator() {
    auto rd = std::random_device{};
    auto ss = std::seed_seq{
            static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count()),
            rd(), rd(), rd(), rd(), rd(), rd(), rd()
    };
    return std::mt19937{ ss };
}

void SgdIterator::resetState(const float x, const float y) {
    _x = x;
    _y = y;
}

void SgdIterator::randomState(float halfExtentX, float halfExtentY) {
    auto distX = std::uniform_real_distribution{ -halfExtentX, halfExtentX };
    _x = distX(_generator);
    auto distY = std::uniform_real_distribution{ -halfExtentY, halfExtentY };
    _y = distY(_generator);
}

void SgdIterator::iterate() {
    const auto tmpX = _x;
    const auto tmpY = _y;
    _x = tmpX - _convergenceRate * _gradientX(tmpX, tmpY);
    _y = tmpY - _convergenceRate * _gradientY(tmpX, tmpY);
}

std::pair<float, float> SgdIterator::getState() const {
    return std::make_pair(_x, _y);
}
