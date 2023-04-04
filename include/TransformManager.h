// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>

#include "EntityManager.h"

class TransformManager {
public:
	void setTransform(Entity entity, const glm::mat4& transform);

	[[nodiscard]] glm::mat4 getTransform(Entity entity) const;

private:
	TransformManager() = default;

	inline static TransformManager* _instance{ nullptr };

	static TransformManager* getInstance();

	std::unordered_map<Entity, glm::mat4> _transforms{};

	friend class Engine;
	friend class Renderer;
};