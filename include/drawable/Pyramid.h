// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include "Drawable.h"

class Pyramid : public Drawable {
public:
	~Pyramid() = default;
	Pyramid(const Pyramid&) = delete;
	Pyramid(Pyramid&&) noexcept = delete;
	Pyramid& operator=(const Pyramid&) = delete;
	Pyramid& operator=(Pyramid&&) noexcept = delete;

	class Builder final : public Drawable::Builder {
	public:
		std::unique_ptr<Drawable> build(Engine& engine) override;
	};

private:
	explicit Pyramid(const Entity entity, Shader* const shader) : Drawable(entity, shader) {}
};