// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <functional>
#include <glm/gtc/matrix_transform.hpp>

// Sun
constexpr auto SUN_RADIUS = 46.5046f; // using AU, multiplied by (just) 10000

// Mercury
constexpr auto MERCURY_SEMI_MAJOR = 77.42f; // using AU, multiplied by 100
constexpr auto MERCURY_SEMI_MINOR = 76.56f; // using AU, multiplied by 100
constexpr auto MERCURY_RADIUS = 0.8154f;    // using AU, multiplied by 50000
constexpr auto MERCURY_TILTING = 0.01f;     // degree
constexpr auto MERCURY_REVOLVING_SPEED = 0.1277f; // km/s, scaled by 375
constexpr auto MERCURY_ROTATING_SPEED = 0.0289f;  // km/h, scaled by 375

// Venus
constexpr auto VENUS_SEMI_MAJOR = 144.66f; // using AU, multiplied by 100
constexpr auto VENUS_SEMI_MINOR = 143.68f; // using AU, multiplied by 100
constexpr auto VENUS_RADIUS = 2.0227f;    // using AU, multiplied by 50000
constexpr auto VENUS_TILTING = 177.36f;     // degree
constexpr auto VENUS_REVOLVING_SPEED = 0.0934f; // km/s, scaled by 375
constexpr auto VENUS_ROTATING_SPEED = 0.0174f;  // km/h, scaled by 375

// Earth
constexpr auto EARTH_SEMI_MAJOR = 200.0f; // using AU, multiplied by 100
constexpr auto EARTH_SEMI_MINOR = 198.0f;  // using AU, multiplied by 100
constexpr auto EARTH_RADIUS = 2.1294f;    // using AU, multiplied by 50000
constexpr auto EARTH_TILTING = 23.5f;     // degree
constexpr auto EARTH_REVOLVING_SPEED = 0.0793f; // km/s, scaled by 375
constexpr auto EARTH_ROTATING_SPEED = 4.4651f;  // km/h, scaled by 375

// Mars
constexpr auto MARS_SEMI_MAJOR = 304.74f; // using AU, multiplied by 100
constexpr auto MARS_SEMI_MINOR = 303.4f; // using AU, multiplied by 100
constexpr auto MARS_RADIUS = 1.1329f;     // using AU, multiplied by 50000
constexpr auto MARS_TILTING = 25.19f;      // degree
constexpr auto MARS_REVOLVING_SPEED = 0.0642f; // km/s, scaled by 375
constexpr auto MARS_ROTATING_SPEED = 2.3153f;  // km/h, scaled by 375

// Jupiter
constexpr auto JUPITER_SEMI_MAJOR = 1040.56f; // using AU, multiplied by 100
constexpr auto JUPITER_SEMI_MINOR = 1039.16f; // using AU, multiplied by 100
constexpr auto JUPITER_RADIUS = 23.3663f;    // using AU, multiplied by 50000
constexpr auto JUPITER_TILTING = 3.13f;      // degree
constexpr auto JUPITER_REVOLVING_SPEED = 0.0349f;  // km/s, scaled by 375
constexpr auto JUPITER_ROTATING_SPEED = 125.3387f; // km/h, scaled by 375

// Saturn
constexpr auto SATURN_SEMI_MAJOR = 1904.76f; // using AU, multiplied by 100
constexpr auto SATURN_SEMI_MINOR = 1902.22f; // using AU, multiplied by 100
constexpr auto SATURN_RADIUS = 19.4628f;    // using AU, multiplied by 50000
constexpr auto SATURN_RING_RADIUS = 22.35f; // using AU, multiplied by 50000
constexpr auto SATURN_RING_THICKNESS = 25.0f;
constexpr auto SATURN_TILTING = 26.73f;     // degree
constexpr auto SATURN_RING_TILTING = 27.0f; // degree
constexpr auto SATURN_REVOLVING_SPEED = 0.0258f; // km/s, scaled by 375
constexpr auto SATURN_ROTATING_SPEED = 92.856f;  // km/h, scaled by 375

// Uranus
constexpr auto URANUS_SEMI_MAJOR = 3838.28f; // using AU, multiplied by 100
constexpr auto URANUS_SEMI_MINOR = 3833.00f; // using AU, multiplied by 100
constexpr auto URANUS_RADIUS = 8.4767f;      // using AU, multiplied by 50000
constexpr auto URANUS_TILTING = 97.77f;      // degree
constexpr auto URANUS_REVOLVING_SPEED = 0.0182f; // km/s, scaled by 375
constexpr auto URANUS_ROTATING_SPEED = 25.008f;  // km/h, scaled by 375

// Neptune
constexpr auto NEPTUNE_SEMI_MAJOR = 6012.22f; // using AU, multiplied by 100
constexpr auto NEPTUNE_SEMI_MINOR = 6006.68f; // using AU, multiplied by 100
constexpr auto NEPTUNE_RADIUS = 8.2294f;      // using AU, multiplied by 50000
constexpr auto NEPTUNE_TILTING = 28.32f;      // degree
constexpr auto NEPTUNE_REVOLVING_SPEED = 0.0145f; // km/s, scaled by 375
constexpr auto NEPTUNE_ROTATING_SPEED = 25.917f;  // km/h, scaled by 375

// Moon
constexpr auto MOON_SEMI_MAJOR = 0.512 + EARTH_RADIUS * 2; // padded by the Earth's diameter since scales are different
constexpr auto MOON_SEMI_MINOR = 0.512 + EARTH_RADIUS * 2; // padded by the Earth's diameter since scales are different
constexpr auto MOON_RADIUS = 0.5806f; // using AU, multiplied by 50000
constexpr auto MOON_TILTING = 6.68f;  // degree
constexpr auto MOON_REVOLVING_SPEED = EARTH_ROTATING_SPEED;
constexpr auto MOON_ROTATING_SPEED = EARTH_ROTATING_SPEED / 27.3;

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

glm::mat4 getRingTransform(
    float revolveAngle, float tiltingAngle, const std::function<float(float)>& orbitX,
    const std::function<float(float)>& orbitY, const glm::vec3& orbitOrientation = glm::vec3{ 1.0f, 1.0f, 1.0f }
);