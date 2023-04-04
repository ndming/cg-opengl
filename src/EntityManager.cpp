// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include "EntityManager.h"

EntityManager* EntityManager::get() {
	if (!_instance) {
		_instance = new EntityManager{};
	}
	return _instance;
}

Entity EntityManager::create() {
	auto entity = _next++;
	_entities.emplace(entity, Component::NONE);

	return entity;
}

bool EntityManager::isAlive(const Entity entity) const {
	return _entities.contains(entity);
}

void EntityManager::discard(const Entity entity) {
	_entities.erase(entity);
}
