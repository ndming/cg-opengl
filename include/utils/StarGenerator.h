// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <memory>
#include <numbers>
#include <random>

#include "Context.h"
#include "Engine.h"
#include "EntityManager.h"
#include "TransformManager.h"

#include "drawable/Aura.h"

class StarGenerator {
public:
    class Builder {
    public:
        Builder& regionRadiusMean(float mean);
        Builder& regionRadiusDeviation(float deviation);

        Builder& minCoreRadius(float radius);
        Builder& maxCoreRadius(float radius);

        Builder& minRecursiveDepth(int depth);
        Builder& maxRecursiveDepth(int depth);

        Builder& billetPolyhedron(Sphere::SubdivisionBuilder::Polyhedron polyhedron);

        [[nodiscard]] std::unique_ptr<StarGenerator> build() const;

    private:
        float _regionRadiusMean{ 8000.0f };
        float _regionRadiusDeviation{ 10.0f };

        float _minCoreRadius{ 0.15f };
        float _maxCoreRadius{ 0.25f };

        int _minRecursiveDepth{ 1 };
        int _maxRecursiveDepth{ 3 };

        Sphere::SubdivisionBuilder::Polyhedron _billetPolyhedron{ Sphere::SubdivisionBuilder::Polyhedron::ICOSAHEDRON };
    };

    Entity generate(TransformManager* tm, Engine& engine);

private:
    StarGenerator(
        float regionRadiusMean, float regionRadiusDeviation, float minCoreRadius, float maxCoreRadius,
        int minRecursiveDepth, int maxRecursiveDepth, Sphere::SubdivisionBuilder::Polyhedron billetPolyhedron
    );

    std::mt19937 _generator{ _ss };

    std::normal_distribution<float> _regionRadiusDist;
    std::uniform_real_distribution<float> _coreRadiusDist;
    std::uniform_int_distribution<int> _recursiveDepthDist;
    Sphere::SubdivisionBuilder::Polyhedron _billetPolyhedron;

    std::uniform_real_distribution<float> _phiDist{ 0.0f, 2 * std::numbers::pi_v<float> };
    std::uniform_real_distribution<float> _thetaDist{ 0.0f, std::numbers::pi_v<float> };
    std::uniform_real_distribution<float> _sizeDist{ 50.0f, 80.0f };

    static std::seed_seq _ss;
    static std::seed_seq getSeedSequence();
};
