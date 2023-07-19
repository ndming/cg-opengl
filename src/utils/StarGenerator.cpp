// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <chrono>
#include <cmath>

#include "utils/StarGenerator.h"

using namespace std;

StarGenerator::Builder &StarGenerator::Builder::regionRadiusMean(const float mean) {
    _regionRadiusMean = mean;
    return *this;
}

StarGenerator::Builder &StarGenerator::Builder::regionRadiusDeviation(const float deviation) {
    _regionRadiusDeviation = deviation;
    return *this;
}

StarGenerator::Builder &StarGenerator::Builder::minCoreRadius(const float radius) {
    _minCoreRadius = radius;
    return *this;
}

StarGenerator::Builder &StarGenerator::Builder::maxCoreRadius(const float radius) {
    _maxCoreRadius = radius;
    return *this;
}

StarGenerator::Builder &StarGenerator::Builder::minRecursiveDepth(const int depth) {
    _minRecursiveDepth = depth;
    return *this;
}

StarGenerator::Builder &StarGenerator::Builder::maxRecursiveDepth(const int depth) {
    _maxRecursiveDepth = depth;
    return *this;
}

StarGenerator::Builder &StarGenerator::Builder::billetPolyhedron(Sphere::SubdivisionBuilder::Polyhedron polyhedron) {
    _billetPolyhedron = polyhedron;
    return *this;
}

std::unique_ptr<StarGenerator> StarGenerator::Builder::build() const {
    return std::unique_ptr<StarGenerator>(
         new StarGenerator(
             _regionRadiusMean, _regionRadiusDeviation, _minCoreRadius,
             _maxCoreRadius, _minRecursiveDepth, _maxRecursiveDepth, _billetPolyhedron
         )
    );
}

StarGenerator::StarGenerator(
    float regionRadiusMean, float regionRadiusDeviation, float minCoreRadius,
    float maxCoreRadius, int minRecursiveDepth, int maxRecursiveDepth, Sphere::SubdivisionBuilder::Polyhedron billetPolyhedron
): _regionRadiusDist{ regionRadiusMean, regionRadiusDeviation }, _coreRadiusDist{ minCoreRadius, maxCoreRadius },
_recursiveDepthDist{ minRecursiveDepth, maxRecursiveDepth }, _billetPolyhedron{ billetPolyhedron } {
}

std::seed_seq StarGenerator::_ss{ getSeedSequence() };

std::seed_seq StarGenerator::getSeedSequence() {
    auto rd = random_device{};
    return seed_seq{ rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd() };
}

Star StarGenerator::generate(Engine& engine) {
    const auto star = Aura::Builder()
            .color(1.0f, 1.0f, 1.0f)
            .coreRadius(_coreRadiusDist(_generator))
            .initialPolygon(_billetPolyhedron)
            .recursiveDepth(_recursiveDepthDist(_generator))
            .shaderModel(Shader::Model::UNLIT)
            .build(engine);

    const auto phi = _phiDist(_generator);
    const auto theta = _thetaDist(_generator);
    const auto x = _regionRadiusDist(_generator) * sin(theta) * cos(phi);
    const auto y = _regionRadiusDist(_generator) * sin(theta) * sin(phi);
    const auto z = _regionRadiusDist(_generator) * cos(theta);

    auto transform = glm::translate(glm::mat4(1.0f), glm::vec3{ x, y, z });
    const auto size = _sizeDist(_generator);
    transform = glm::scale(transform, glm::vec3{ size, size, size });

    const auto light = EntityManager::get()->create();
    LightManager::Builder(LightManager::Type::POINT)
        .position(x, y, z)
        .distance(LightManager::LightDistance::SUPER_MASSIVE)
        .build(light);


    return Star{ star->getEntity(), transform, light };
}
