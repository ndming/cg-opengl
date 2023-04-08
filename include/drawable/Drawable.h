// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <glm/vec3.hpp>
#include <memory>

#include "../EntityManager.h"
#include "../Shader.h"

#include "Material.h"

class Drawable {
public:
	~Drawable() = default;
	Drawable(const Drawable& other) = delete;
	Drawable& operator=(const Drawable& other) = delete;
	Drawable(Drawable&& other) noexcept = delete;
	Drawable& operator=(Drawable&& other) noexcept = delete;

	class Builder {
	public:
		virtual ~Builder() = default;
		Builder(const Builder&) = delete;
		Builder(Builder&&) noexcept = delete;
		Builder& operator=(const Builder&) = delete;
		Builder& operator=(Builder&&) noexcept = delete;

		Builder& shaderModel(Shader::Model model);

        Builder& phongAmbient(float r, float g, float b);
        Builder& phongAmbient(const glm::vec3& ambient);
        Builder& phongDiffuse(float r, float g, float b);
        Builder& phongDiffuse(const glm::vec3& diffuse);
        Builder& phongSpecular(float r, float g, float b);
        Builder& phongSpecular(const glm::vec3& specular);
        Builder& phongShininess(float shininess);
        Builder& phongMaterial(const phong::Material& material);

		virtual std::unique_ptr<Drawable> build(Engine& engine) = 0;

	protected:
		Builder() = default;

		Shader::Model _shaderModel{ Shader::Model::UNLIT };

        glm::vec3 _phongAmbient{1.0f, 0.5f, 0.31f };
        glm::vec3 _phongDiffuse{1.0f, 0.5f, 0.31f };
        glm::vec3 _phongSpecular{0.5f, 0.5f, 0.5f };
        float _phongShininess{ 32.0f };
	};

    [[nodiscard]] Entity getEntity() const;

    [[nodiscard]] Shader* getShader() const;

protected:
	explicit Drawable(const Entity entity, Shader* const shader)
	: _entity{ entity }, _shader{ shader } {}

private:
	const Entity _entity;

	Shader* const _shader;
};