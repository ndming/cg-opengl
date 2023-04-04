// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <set>

#include "EntityManager.h"

class Scene {
public:
	~Scene() = default;
	Scene(const Scene&) = delete;
	Scene(Scene&&) noexcept = delete;
	Scene& operator=(const Scene&) = delete;
	Scene& operator=(Scene&&) noexcept = delete;

	void addEntity(Entity entity);

	void removeEntity(Entity entity);

	[[nodiscard]] bool hasEntity(Entity entity) const;

private:
	Scene() = default;

	std::set<Entity> _renderables{};

	std::set<Entity> _lights{};

	friend class Engine;
	friend class Renderer;
};
