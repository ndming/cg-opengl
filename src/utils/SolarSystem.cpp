// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "utils/SolarSystem.h"

glm::mat4 getPlanetTransform(
    const float revolveAngle, const float rotateAngle, const float tiltingAngle, const float planetRadius,
    const std::function<float(float)>& orbitX, const std::function<float(float)>& orbitY,
    const glm::vec3& orbitOrientation, const glm::vec3& orbitCenter
) {
    // Center
    auto transform = glm::translate(glm::mat4(1.0f), orbitCenter);
    // Orientation
    if (orbitOrientation != glm::vec3{ 0.0f, 0.0f, 1.0f }) {
        const auto orientingAxis = glm::normalize(glm::cross(glm::vec3{ 0.0f, 0.0f, 1.0f }, orbitOrientation));
        const auto orientingAngle = glm::acos(glm::normalize(orbitOrientation).z);
        const auto orientingQuat = glm::angleAxis(orientingAngle, orientingAxis);
        const auto orientingMat = glm::mat4_cast(orientingQuat);
        transform *= orientingMat;
    }
    // Revolve
    const auto revolveVec = glm::vec3{ orbitX(revolveAngle), orbitY(revolveAngle), 0.0f };
    transform = glm::translate(transform, revolveVec);
    // Tilt
    const auto tiltingAxis = glm::vec3{ 0.0f, 1.0f, 0.0f };
    const auto tiltingQuat = glm::angleAxis(tiltingAngle, tiltingAxis);
    const auto tiltingMat = glm::mat4_cast(tiltingQuat);
    transform *= tiltingMat;
    // Rotate
    const auto rotateAxis = glm::vec3{ 0.0f, 0.0f, 1.0f };
    const auto rotateQuat = glm::angleAxis(rotateAngle, rotateAxis);
    const auto rotateMat = glm::mat4_cast(rotateQuat);
    transform *= rotateMat;
    // Scale
    transform = glm::scale(transform, glm::vec3{ planetRadius, planetRadius, planetRadius });
    return transform;
}

glm::mat4 getOrbitTransform(const glm::vec3& orbitOrientation, const glm::vec3& orbitCenter) {
    // Center
    auto transform = glm::translate(glm::mat4(1.0f), orbitCenter);
    // Orientation
    const auto orientingAxis = glm::normalize(glm::cross(glm::vec3{ 0.0f, 0.0f, 1.0f }, orbitOrientation));
    const auto orientingAngle = glm::acos(glm::normalize(orbitOrientation).z);
    const auto orientingQuat = glm::angleAxis(orientingAngle, orientingAxis);
    const auto orientingMat = glm::mat4_cast(orientingQuat);
    transform *= orientingMat;
    return transform;
}

glm::mat4 getRingTransform(
    const float revolveAngle, const float tiltingAngle, const std::function<float(float)>& orbitX,
    const std::function<float(float)>& orbitY, const glm::vec3& orbitOrientation
) {
    auto transform = glm::mat4(1.0f);

    // Orientation
    const auto orientingAngle = glm::acos(glm::normalize(orbitOrientation).z);
    if (orbitOrientation != glm::vec3{ 0.0f, 0.0f, 1.0f }) {
        const auto orientingAxis = glm::normalize(glm::cross(glm::vec3{ 0.0f, 0.0f, 1.0f }, orbitOrientation));
        const auto orientingQuat = glm::angleAxis(orientingAngle, orientingAxis);
        const auto orientingMat = glm::mat4_cast(orientingQuat);
        transform *= orientingMat;
    }

    // Revolve
    const auto revolveVec = glm::vec3{ orbitX(revolveAngle), orbitY(revolveAngle), 0.0f };
    transform = glm::translate(transform, revolveVec);

    // Tilt
    const auto tiltingAxis = glm::vec3{ 0.0f, 1.0f, 0.0f };
    const auto tiltingQuat = glm::angleAxis(tiltingAngle - orientingAngle, tiltingAxis);
    const auto tiltingMat = glm::mat4_cast(tiltingQuat);
    transform *= tiltingMat;

    return transform;
}
