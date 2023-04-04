// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <unordered_map>

using Entity = unsigned int;

class EntityManager {
public:
	~EntityManager() = default;
	EntityManager(const EntityManager&) = delete;
	EntityManager(EntityManager&&) noexcept = delete;
	EntityManager& operator=(const EntityManager&) = delete;
	EntityManager& operator=(EntityManager&&) = delete;

	static EntityManager* get();

	[[nodiscard]] Entity create();

	[[nodiscard]] bool isAlive(Entity entity) const;

	void discard(Entity entity);

private:
	enum class Component {
		RENDERABLE,
		LIGHT,
		NONE
	};

	EntityManager() = default;

	Entity _next{ 0 };

	std::unordered_map<Entity, Component> _entities{};

	inline static EntityManager* _instance{ nullptr };

	friend class Engine;
	friend class LightManager;
	friend class RenderableManager;
	friend class Scene;
};