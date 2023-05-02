// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include "EntityManager.h"

class Camera {
public:
	~Camera() = default;
	Camera(const Camera&) = delete;
	Camera(Camera&&) noexcept = delete;
	Camera& operator=(const Camera&) = delete;
	Camera& operator=(Camera&&) noexcept = delete;

	[[nodiscard]] Entity getEntity() const;

	void setProjection(float left, float right, float bottom, float top, float zNear, float zFar);

	void setProjection(float fov, float ratio, float near, float far);

	[[nodiscard]] glm::mat4 getProjection() const;

	[[nodiscard]] glm::mat4 getViewMatrix() const;

	void relativeDrag(float offsetX, float offsetY);

	void relativeZoom(float amount);

    void setRadius(float radius);

    void setLongitudeAngle(float degree);

    void setLatitudeAngle(float degree);

    void setZoomSensitive(float sensitive);

    void setDragSensitive(float sensitive);

private:
	explicit Camera(const Entity entity) : _entity{ entity } {}

	const Entity _entity;

	float _radius{ 4.0f };	// radius of the looking sphere [MIN_RADIUS; MAX_RADIUS]
	float _phi{ -90.0f };	// XY-plane rotation [0; 360]
	float _theta{ 80.0f };	// Z-axis rotation [1; 179]

    float _zoomSensitive{ 5.0f };
    float _dragSensitive{ 0.5f };

	glm::mat4 _projection{ glm::perspective(glm::radians(DEFAULT_FOV), 1.0f, DEFAULT_NEAR, DEFAULT_FAR) };

	static constexpr auto MIN_RADIUS = 1.0f;
	static constexpr auto MAX_RADIUS = 5000.0f;

	static constexpr auto MIN_THETA = 1.0f;
	static constexpr auto MAX_THETA = 179.0f;

	static constexpr auto DEFAULT_FOV  = 45.0f;
	static constexpr auto DEFAULT_NEAR = 0.1f;
	static constexpr auto DEFAULT_FAR  = 100.0f;

	friend class Engine;
};