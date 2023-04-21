// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <functional>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 getPlanetTransform(
    float revolveAngle, float rotateAngle, float tiltingAngle, float planetRadius,
    const std::function<float(float)>& orbitX, const std::function<float(float)>& orbitY,
    const glm::vec3& orbitOrientation = glm::vec3{ 0.0f, 0.0f, 1.0f },
    const glm::vec3& orbitCenter = glm::vec3{ 0.0f, 0.0f, 0.0f }
);

glm::mat4 getOrbitTransform(
    const glm::vec3& orbitOrientation = glm::vec3{ 0.0f, 0.0f, 1.0f },
    const glm::vec3& orbitCenter = glm::vec3{ 0.0f, 0.0f, 0.0f }
);
