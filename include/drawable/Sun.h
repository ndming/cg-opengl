// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include "Drawable.h"
#include "Sphere.h"

class Sun : public Drawable {
public:
	~Sun() = default;
	Sun(const Sun&) = delete;
	Sun(Sun&&) noexcept = delete;
	Sun& operator=(const Sun&) = delete;
	Sun& operator=(Sun&&) noexcept = delete;

	class Builder final : protected Sphere::SubdivisionBuilder {
	public:
		explicit Builder() { color(1.0f, 1.0f, 1.0f); }

		Builder& color(float r, float g, float b);

		std::unique_ptr<Drawable> build(Engine& engine) override;

	private:
		static constexpr auto RECURSIVE_DEPTH = 2;

		static constexpr auto SUN_RADIUS = 0.25f;
	};

private:
	explicit Sun(const Entity entity, Shader* const shader) : Drawable(entity, shader) {}
};