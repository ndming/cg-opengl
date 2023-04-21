// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <array>
#include <glm/vec3.hpp>
#include <memory>

#include "EntityManager.h"

class LightManager {
public:
	enum class Type {
		DIRECTIONAL,
		POINT
	};

	enum class LightDistance {
		SHORT,
		AVERAGE,
		LARGE,
        MASSIVE
	};

	class Builder {
	public:
		explicit Builder(const Type lightType) : _type{ lightType } {}

		Builder& color(float r, float g, float b);
		Builder& direction(float x, float y, float z);
		Builder& position(float x, float y, float z);

		Builder& ambient(float r, float g, float b);
		Builder& diffuse(float r, float g, float b);
		Builder& specular(float r, float g, float b);

		Builder& distance(LightDistance distance);

		void build(Entity entity) const;

	private:
		const Type _type;

		float _rgb[3]{ 1.0f, 1.0f,  1.0f };
		float _dir[3]{ 1.0f, 1.0f, -1.0f };
		float _pos[3]{ 0.0f, 0.0f,  0.0f };

		float _ambient[3] { 0.02f, 0.02f, 0.02f };
		float _diffuse[3] { 0.5f, 0.5f, 0.5f };
		float _specular[3]{ 1.0f, 1.0f, 1.0f };

		LightDistance _distance{ LightDistance::AVERAGE };

		void buildDirectionalLight(Entity entity) const;
		void buildPointLight(Entity entity) const;

		[[nodiscard]] std::array<float, 3> resolveLightDistance() const;
	};

	[[nodiscard]] bool hasComponent(Entity entity) const;

	void setPosition(Entity light, float x, float y, float z);

private:
	struct DirectionalLight {
		glm::vec3 direction;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};

	struct PointLight {
		glm::vec3 position;
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float constant;
		float linear;
		float quadratic;
	};

	LightManager() = default;

	inline static LightManager* _instance{ nullptr };

	static LightManager* getInstance();

	std::unordered_map<Entity, std::unique_ptr<DirectionalLight>> _directionalLights{};

	std::unordered_map<Entity, std::unique_ptr<PointLight>> _pointLights{};

	friend class Engine;
	friend class Renderer;
};