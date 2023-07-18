// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include "Drawable.h"
#include "Sphere.h"

class Aura : public Drawable {
public:
	~Aura() = default;
	Aura(const Aura&) = delete;
	Aura(Aura&&) noexcept = delete;
	Aura& operator=(const Aura&) = delete;
	Aura& operator=(Aura&&) noexcept = delete;

	class Builder final : public Sphere::SubdivisionBuilder {
	public:
		explicit Builder() { color(1.0f, 1.0f, 1.0f); }

		Builder& color(float r, float g, float b);

        Builder& coreRadius(float radius);

		std::unique_ptr<Drawable> build(Engine& engine) override;
	};

private:
	Aura(const Entity entity, Shader* const shader) : Drawable(entity, shader) {}
};