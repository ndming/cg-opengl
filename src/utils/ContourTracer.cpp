// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include "EntityManager.h"
#include "drawable/Trace.h"

#include "utils/ContourTracer.h"

ContourTracer::Builder &ContourTracer::Builder::gradientX(std::function<float(float, float)> gradient) noexcept {
    _gradientX = std::move(gradient);
    return *this;
}

ContourTracer::Builder &ContourTracer::Builder::gradientY(std::function<float(float, float)> gradient) noexcept {
    _gradientY = std::move(gradient);
    return *this;
}

ContourTracer::Builder &ContourTracer::Builder::traceSize(const float size) {
    _traceSize = size;
    return *this;
}

ContourTracer::Builder &ContourTracer::Builder::heightPadding(const float padding) {
    _heightPadding = padding;
    return *this;
}

ContourTracer::Builder &ContourTracer::Builder::traceColor(float r, float g, float b) {
    _traceColor.r = r; _traceColor.g = g; _traceColor.b = b;
    return *this;
}

ContourTracer::Builder &ContourTracer::Builder::markColor(float r, float g, float b) {
    _markColor.r = r; _markColor.g = g; _markColor.b = b;
    return *this;
}

std::unique_ptr<ContourTracer> ContourTracer::Builder::build() {
    if (!_gradientX || !_gradientY) {
        throw std::runtime_error("ContourTracer: gradientX and gradientY must be set.");
    }
    return std::unique_ptr<ContourTracer>(new ContourTracer(
            std::move(_gradientX), std::move(_gradientY),
            _traceSize, _heightPadding, _traceColor, _markColor
    ));
}

void ContourTracer::resetTo(const float x, const float y, Scene &scene, Engine &engine) {
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
    const auto norm = glm::vec3{ 0.0f, 0.0f, 1.0f };
    auto mark = Trace::Builder()
            .position(x, y, 0.0f)
            .normal(norm)
            .direction(glm::vec3{ -_gradientX(x, y), -_gradientY(x, y), 0.0f })
            .color(_markColor.r, _markColor.g, _markColor.b)
            .size(_traceSize)
            .shaderModel(Shader::Model::UNLIT)
            .build(engine);

    // Pad the mark and add to the scene
    const auto tm = engine.getTransformManager();
    tm->setTransform(mark->getEntity(), glm::translate(glm::mat4(1.0f), norm * _heightPadding));
    scene.addEntity(mark->getEntity());

    _currentTraces.push_back(std::move(mark));
    _currentX = x;
    _currentY = y;
}

void ContourTracer::traceTo(const float x, const float y, Scene &scene, Engine &engine) {
    const auto currPos = glm::vec3{ _currentX, _currentY, 0.0f };
    const auto lastPos = glm::vec3{ x, y, 0.0f };
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
        const auto norm = glm::vec3{ 0.0f, 0.0f, 1.0f };

        auto trace = Trace::Builder()
                .position(_currentX, _currentY, 0.0f)
                .normal(norm)
                .direction(glm::vec3{ -gradX, -gradY, 0.0f })
                .color(_traceColor.r, _traceColor.g, _traceColor.b)
                .size(_traceSize)
                .shaderModel(Shader::Model::UNLIT)
                .build(engine);

        // Pad the mark and add to the scene
        const auto tm = engine.getTransformManager();
        tm->setTransform(trace->getEntity(), glm::translate(glm::mat4(1.0f), norm * _heightPadding));
        scene.addEntity(trace->getEntity());
        _currentTraces.push_back(std::move(trace));

        distance -= 2 * _traceSize;
    }
}
