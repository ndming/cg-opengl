// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include "TransformManager.h"

TransformManager* TransformManager::getInstance() {
	if (!_instance) {
		_instance = new TransformManager();
	}
	return _instance;
}

void TransformManager::setTransform(const Entity entity, const glm::mat4& transform) {
	_transforms[entity] = transform;
}

glm::mat4 TransformManager::getTransform(const Entity entity) const {
	if (_transforms.contains(entity)) {
		return _transforms.at(entity);
	}
	return glm::mat4(1.0f);
}