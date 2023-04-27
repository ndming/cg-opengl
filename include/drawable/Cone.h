// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include "Drawable.h"

class Cone : public Drawable {
public:
	~Cone() = default;
	Cone(const Cone&) = delete;
	Cone(Cone&&) noexcept = delete;
	Cone& operator=(const Cone&) = delete;
	Cone& operator=(Cone&&) noexcept = delete;

	class Builder final : public Drawable::Builder {
	public:
		Builder& segments(int segments);

		std::unique_ptr<Drawable> build(Engine& engine) override;

	private:
		int _segments = 100;
	};

private:
	Cone(const Entity entity, Shader* const shader) : Drawable(entity, shader) {}
};