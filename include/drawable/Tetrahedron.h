// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include "Drawable.h"

class Tetrahedron : public Drawable {
public:
	~Tetrahedron() = default;
	Tetrahedron(const Tetrahedron&) = delete;
	Tetrahedron(Tetrahedron&&) noexcept = delete;
	Tetrahedron& operator=(const Tetrahedron&) = delete;
	Tetrahedron& operator=(Tetrahedron&&) noexcept = delete;

	class Builder final : public Drawable::Builder {
	public:
		std::unique_ptr<Drawable> build(Engine& engine) override;
	};

private:
	Tetrahedron(const Entity entity, Shader* const shader) : Drawable(entity, shader) {}
};