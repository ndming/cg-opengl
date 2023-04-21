// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"

Entity Camera::getEntity() const {
	return _entity;
}

void Camera::relativeDrag(const float offsetX, const float offsetY) {
	_phi -= offsetX * DRAG_SENSITIVE;
	_theta -= offsetY * DRAG_SENSITIVE;
	_theta = glm::clamp(_theta, MIN_THETA, MAX_THETA);
}

void Camera::relativeZoom(const float amount) {
	_radius -= amount * ZOOM_SENSITIVE;
	_radius = glm::clamp(_radius, MIN_RADIUS, MAX_RADIUS);
}

void Camera::setProjection(const float fov, const float ratio, const float near, const float far) {
	_projection = glm::perspective(fov, ratio, near, far);
}

void Camera::setProjection(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar) {
	_projection = glm::ortho(left, right, bottom, top, zNear, zFar);
}

glm::mat4 Camera::getProjection() const {
	return _projection;
}

glm::mat4 Camera::getViewMatrix() const {
	const auto pos = glm::vec3{
		_radius * glm::sin(glm::radians(_theta)) * glm::cos(glm::radians(_phi)),
		_radius * glm::sin(glm::radians(_theta)) * glm::sin(glm::radians(_phi)),
		_radius * glm::cos(glm::radians(_theta))
	};
	return lookAt(pos, glm::vec3{ 0.0f, 0.0f, 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f });
}

void Camera::setRadius(const float radius) {
	_radius = glm::clamp(radius, MIN_RADIUS, MAX_RADIUS);
}





