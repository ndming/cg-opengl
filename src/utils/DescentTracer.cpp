// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <stdexcept>

#include "../EntityManager.h"
#include "Trace.h"

#include "utils/DescentTracer.h"

DescentTracer::Builder &DescentTracer::Builder::objective(std::function<float(float, float)> function) noexcept {
    _objective = std::move(function);
    return *this;
}

DescentTracer::Builder &DescentTracer::Builder::gradientX(std::function<float(float, float)> gradient) noexcept {
    _gradientX = std::move(gradient);
    return *this;
}

DescentTracer::Builder &DescentTracer::Builder::gradientY(std::function<float(float, float)> gradient) noexcept {
    _gradientY = std::move(gradient);
    return *this;
}

DescentTracer::Builder &DescentTracer::Builder::traceSize(const float size) {
    _traceSize = size;
    return *this;
}

DescentTracer::Builder &DescentTracer::Builder::heightPadding(const float padding) {
    _heightPadding = padding;
    return *this;
}

DescentTracer::Builder &DescentTracer::Builder::traceColor(const float r, const float g, const float b) {
    _traceColor.r = r; _traceColor.g = g; _traceColor.b = b;
    return *this;
}

DescentTracer::Builder &DescentTracer::Builder::markColor(const float r, const float g, const float b) {
    _markColor.r = r; _markColor.g = g; _markColor.b = b;
    return *this;
}

std::unique_ptr<DescentTracer> DescentTracer::Builder::build() {
    if (!_objective || !_gradientX || !_gradientY) {
        throw std::runtime_error("DescentTracer: objective, gradientX, and gradientY must be set.");
    }
    return std::unique_ptr<DescentTracer>(new DescentTracer(
        std::move(_objective), std::move(_gradientX), std::move(_gradientY),
        _traceSize, _heightPadding, _traceColor, _markColor
    ));
}

void DescentTracer::resetTo(const float x, const float y, Scene &scene, Engine &engine) {
    // Remove and clean up all current traces
    const auto entityManager = EntityManager::get();
    for (const auto& trace : _currentTraces) {
        scene.removeEntity(trace->getEntity());
        engine.destroyEntity(trace->getEntity());
        engine.destroyShader(trace->getShader());
        entityManager->discard(trace->getEntity());
    }
    _currentTraces.clear();

    // Create a mark at this position
    const auto norm = getNormalAt(x, y);
    auto mark = Trace::Builder()
            .position(x, y, _objective(x, y))
            .normal(norm)
            .direction(glm::vec3{ -_gradientX(x, y), -_gradientY(x, y), 0.0f })
            .size(_traceSize)
            .color(_markColor.r, _markColor.g, _markColor.b)
            .build(engine);

    // Pad the mark and add to the scene
    const auto tm = engine.getTransformManager();
    tm->setTransform(mark->getEntity(), glm::translate(glm::mat4(1.0f), norm * _heightPadding));
    scene.addEntity(mark->getEntity());

    _currentTraces.push_back(std::move(mark));
    _currentX = x;
    _currentY = y;
}

void DescentTracer::traceTo(const float x, const float y, Scene &scene, Engine &engine) {
    const auto currPos = glm::vec3{ _currentX, _currentY, _objective(_currentX, _currentY) };
    const auto lastPos = glm::vec3{ x, y, _objective(x, y) };
    auto distance = glm::distance(currPos, lastPos);
    // Because there's already a trace at the current (x, y), we only draw a new trace when the distance is far enough.
    while (distance - 2.0f * _traceSize >= 0.0f) {
        // We use Intercept theorem to estimate the step size of x and y
        const auto ratio = (2.0f * _traceSize) / distance;
        const auto tmpX = _currentX;
        const auto tmpY = _currentY;
        _currentX -= (tmpX - x) * ratio;
        _currentY -= (tmpY - y) * ratio;

        // Draw a trace at the current position
        const auto gradX = _gradientX(_currentX, _currentY);
        const auto gradY = _gradientY(_currentX, _currentY);
        const auto norm = -glm::normalize(glm::vec3{ gradX, gradY, -1.0f });
        auto trace = Trace::Builder()
                .position(_currentX, _currentY, _objective(_currentX, _currentY))
                .normal(norm)
                .direction(glm::vec3{ -gradX, -gradY, 0.0f })
                .size(_traceSize)
                .color(_traceColor.r, _traceColor.g, _traceColor.b)
                .build(engine);

        // Pad the mark and add to the scene
        const auto tm = engine.getTransformManager();
        tm->setTransform(trace->getEntity(), glm::translate(glm::mat4(1.0f), norm * _heightPadding));
        scene.addEntity(trace->getEntity());
        _currentTraces.push_back(std::move(trace));

        distance -= 2 * _traceSize;
    }
}

glm::vec3 DescentTracer::getNormalAt(const float x, const float y) const {
    // Get the normal vector at this position
    const auto dirX = _gradientX(x, y);
    const auto dirY = _gradientY(x, y);
    return -glm::normalize(glm::vec3{ dirX, dirY, -1.0f });
}
