// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <utility>

#include "LightManager.h"
#include "EntityManager.h"

LightManager::Builder& LightManager::Builder::color(
	const float r, 
	const float g, 
	const float b
) {
	_rgb[0] = r;
	_rgb[1] = g;
	_rgb[2] = b;
	return *this;
}

LightManager::Builder& LightManager::Builder::direction(
	const float x,
	const float y,
	const float z
) {
	_dir[0] = x;
	_dir[1] = y;
	_dir[2] = z;
	return *this;
}

LightManager::Builder& LightManager::Builder::position(
	const float x,
	const float y,
	const float z
) {
	_pos[0] = x;
	_pos[1] = y;
	_pos[2] = z;
	return *this;
}

LightManager::Builder& LightManager::Builder::ambient(
	const float r,
	const float g,
	const float b
) {
	_ambient[0] = r;
	_ambient[1] = g;
	_ambient[2] = b;
	return *this;
}

LightManager::Builder& LightManager::Builder::diffuse(
	const float r,
	const float g,
	const float b
) {
	_diffuse[0] = r;
	_diffuse[1] = g;
	_diffuse[2] = b;
	return *this;
}

LightManager::Builder& LightManager::Builder::specular(
	const float r,
	const float g,
	const float b
) {
	_specular[0] = r;
	_specular[1] = g;
	_specular[2] = b;
	return *this;
}

LightManager::Builder& LightManager::Builder::distance(const LightDistance distance) {
	_distance = distance;
	return *this;
}

void LightManager::Builder::build(const Entity entity) const {
	switch (_type) {
	case Type::DIRECTIONAL:
		buildDirectionalLight(entity);
		break;
	case Type::POINT:
		buildPointLight(entity);
		break;
	}
	// Record this entity as light component
	const auto entityManager = EntityManager::get();
	entityManager->_entities[entity] = EntityManager::Component::LIGHT;
}

void LightManager::Builder::buildDirectionalLight(const Entity entity) const {
	const auto lightManager = getInstance();
	lightManager->_directionalLights[entity] = std::make_unique<DirectionalLight>(
		glm::vec3{ _dir[0], _dir[1], _dir[2] },
		glm::vec3{ _ambient[0], _ambient[1], _ambient[2] },
		glm::vec3{ _diffuse[0], _diffuse[1], _diffuse[2] },
		glm::vec3{ _specular[0], _specular[1], _specular[2] }
	);
}

void LightManager::Builder::buildPointLight(const Entity entity) const {
	const auto lightManager = getInstance();
	const auto [constant, linear, quadratic] = resolveLightDistance();
	lightManager->_pointLights[entity] = std::make_unique<PointLight>(
		glm::vec3{ _pos[0], _pos[1], _pos[2] },
		glm::vec3{ _ambient[0], _ambient[1], _ambient[2] },
		glm::vec3{ _diffuse[0], _diffuse[1], _diffuse[2] },
		glm::vec3{ _specular[0], _specular[1], _specular[2] },
		constant, linear, quadratic
	);
}

std::array<float, 3> LightManager::Builder::resolveLightDistance() const {
	switch (_distance) {
	case LightDistance::SHORT:
		// Distance of 50
		return std::array{ 1.0f, 0.09f, 0.032f };
	case LightDistance::LARGE:
		// Distance of 600
		return std::array{ 1.0f, 0.007f, 0.0002f };
	case LightDistance::AVERAGE:
		// Distance of 200
		return std::array{ 1.0f, 0.022f, 0.0019f };
	}
	return { 1.0f, 0.045f, 0.0075f };
}

LightManager* LightManager::getInstance() {
	if (!_instance) {
		_instance = new LightManager();
	}
	return _instance;
}

bool LightManager::hasComponent(const Entity entity) const {
	return _directionalLights.contains(entity)
		|| _pointLights.contains(entity);
}

void LightManager::setPosition(
	const Entity entity, 
	const float x, 
	const float y, 
	const float z
) {
	if (_pointLights.contains(entity)) {
		_pointLights[entity]->position = glm::vec3{ x, y, z };
	}
}
