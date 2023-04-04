// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include "Scene.h"

void Scene::addEntity(const Entity entity) {
	const auto entityManager = EntityManager::get();
	if (entityManager->_entities.contains(entity)) {
		if (entityManager->_entities[entity] == EntityManager::Component::RENDERABLE) {
			_renderables.insert(entity);
		} else if (entityManager->_entities[entity] == EntityManager::Component::LIGHT) {
			_lights.insert(entity);
		}
	}
}

void Scene::removeEntity(const Entity entity) {
	_renderables.erase(entity);
	_lights.erase(entity);
}

bool Scene::hasEntity(const Entity entity) const {
	return _renderables.contains(entity) || _lights.contains(entity);
}
