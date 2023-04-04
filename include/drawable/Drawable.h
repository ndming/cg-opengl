// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <glm/vec3.hpp>
#include <memory>

#include "../EntityManager.h"
#include "../Shader.h"

class Drawable {
public:
	~Drawable() = default;
	Drawable(const Drawable& other) = delete;
	Drawable& operator=(const Drawable& other) = delete;
	Drawable(Drawable&& other) noexcept = delete;
	Drawable& operator=(Drawable&& other) noexcept = delete;

	[[nodiscard]] Entity getEntity() const;

	[[nodiscard]] Shader* getShader() const;

	class Builder {
	public:
		virtual ~Builder() = default;
		Builder(const Builder&) = delete;
		Builder(Builder&&) noexcept = delete;
		Builder& operator=(const Builder&) = delete;
		Builder& operator=(Builder&&) noexcept = delete;

		Builder& shaderModel(Shader::Model model);

		virtual std::unique_ptr<Drawable> build(Engine& engine) = 0;

	protected:
		Builder() = default;

		Shader::Model _shaderModel{ Shader::Model::UNLIT };
	};

protected:
	explicit Drawable(const Entity entity, Shader* const shader)
	: _entity{ entity }, _shader{ shader } {}

	struct Material {
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;
	};

	inline static const Material MATERIAL{
		glm::vec3{ 1.0f, 0.5f, 0.31f },
		glm::vec3{ 1.0f, 0.5f, 0.31f },
		glm::vec3{ 0.5f, 0.5f, 0.5f },
		32.0f
	};

private:
	const Entity _entity;

	Shader* const _shader;
};