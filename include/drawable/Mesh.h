// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <functional>

#include "Drawable.h"

class Mesh : public Drawable {
public:
	~Mesh() = default;
	Mesh(const Mesh&) = delete;
	Mesh(Mesh&&) noexcept = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh& operator=(Mesh&&) noexcept = delete;

	class Builder : public Drawable::Builder {
	public:
		explicit Builder(std::function<float(float, float)> func) : _func{ std::move(func) } {}

		Builder& halfExtentX(float extent);
		Builder& halfExtentY(float extent);
		Builder& halfExtent(float extent);

		Builder& segmentsX(int segments);
		Builder& segmentsY(int segments);
		Builder& segments(int segments);

		std::unique_ptr<Drawable> build(Engine& engine) override;

    protected:
		const std::function<float(float, float)> _func;
		float _halfExtentX{ 1.0f };
		float _halfExtentY{ 1.0f };
		int _segmentsX{ 40 };
		int _segmentsY{ 40 };

		static constexpr auto MIN_SEGMENTS = 1;
		static constexpr auto MIN_EXTENT = 0.1f;
	};

private:
	Mesh(const Entity entity, Shader* const shader) : Drawable(entity, shader) {}
};