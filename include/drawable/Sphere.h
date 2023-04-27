// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <vector>

#include "Drawable.h"

class Sphere : public Drawable {
public:
	~Sphere() = default;
	Sphere(const Sphere&) = delete;
	Sphere(Sphere&&) noexcept = delete;
	Sphere& operator=(const Sphere&) = delete;
	Sphere& operator=(Sphere&&) noexcept = delete;

	class GeographicBuilder final : public Builder {
	public:
		GeographicBuilder& longitudes(int amount);

		GeographicBuilder& latitudes(int amount);

		std::unique_ptr<Drawable> build(Engine& engine) override;

	private:
		int _longitudes = 50;
		int _latitudes  = 20;
	};

	class SubdivisionBuilder : public Builder {
	public:
		enum class Polyhedron {
			TETRAHEDRON,
			ICOSAHEDRON
		};

		SubdivisionBuilder& recursiveDepth(int depth);

		SubdivisionBuilder& initialPolygon(Polyhedron polyhedron);

        SubdivisionBuilder& uniformColor(float r, float g, float b);

		std::unique_ptr<Drawable> build(Engine& engine) override;

	protected:
		SubdivisionBuilder& radius(float radius);

	private:
		float _radius{ 1.0f };

		int _depth{ 4 };

		Polyhedron _polyhedron{ Polyhedron::TETRAHEDRON };

		float* _uniformColor{ nullptr };

		[[nodiscard]] std::vector<glm::vec3> getFaces() const;

		[[nodiscard]] std::vector<float> subdivide(
			const glm::vec3& p0, 
			const glm::vec3& p1, 
			const glm::vec3& p2,
			int depth
		) const;
	};

private:
	Sphere(const Entity entity, Shader* const shader) : Drawable(entity, shader) {}
};