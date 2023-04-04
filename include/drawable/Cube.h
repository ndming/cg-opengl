// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include "Drawable.h"

class Cube : public Drawable {
public:
	~Cube() = default;
	Cube(const Cube&) = delete;
	Cube(Cube&&) noexcept = delete;
	Cube& operator=(const Cube&) = delete;
	Cube& operator=(Cube&&) noexcept = delete;

	class Builder final : public Drawable::Builder {
	public:
		std::unique_ptr<Drawable> build(Engine& engine) override;
	};

private:
	explicit Cube(const Entity entity, Shader* const shader)
	: Drawable(entity, shader) {}
};