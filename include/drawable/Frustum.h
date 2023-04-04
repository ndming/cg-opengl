// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include "Drawable.h"

class Frustum : public Drawable {
public:
	~Frustum() = default;
	Frustum(const Frustum&) = delete;
	Frustum(Frustum&&) noexcept = delete;
	Frustum& operator=(const Frustum&) = delete;
	Frustum& operator=(Frustum&&) noexcept = delete;

	class Builder final : public Drawable::Builder {
	public:
		std::unique_ptr<Drawable> build(Engine& engine) override;
	};

private:
	explicit Frustum(const Entity entity, Shader* const shader) : Drawable(entity, shader) {}
};