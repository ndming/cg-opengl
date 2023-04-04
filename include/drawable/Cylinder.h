// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include "Drawable.h"

class Cylinder : public Drawable {
public:
	~Cylinder() = default;
	Cylinder(const Cylinder&) = delete;
	Cylinder(Cylinder&&) noexcept = delete;
	Cylinder& operator=(const Cylinder&) = delete;
	Cylinder& operator=(Cylinder&&) noexcept = delete;

	class Builder final : public Drawable::Builder {
	public:
		Builder& segments(int segments);

		std::unique_ptr<Drawable> build(Engine& engine) override;

	private:
		int _segments = 100;
	};

private:
	explicit Cylinder(const Entity entity, Shader* const shader) : Drawable(entity, shader) {}
};