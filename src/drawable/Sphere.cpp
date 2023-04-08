// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <cmath>
#include <glm/geometric.hpp>
#include <numbers>
#include <ranges>
#include <vector>

#include "../VertexBuffer.h"
#include "../IndexBuffer.h"
#include "../RenderableManager.h"

#include "Sphere.h"
#include "Color.h"

Sphere::GeographicBuilder& Sphere::GeographicBuilder::longitudes(const int amount) {
	_longitudes = amount;
	return *this;
}

Sphere::GeographicBuilder& Sphere::GeographicBuilder::latitudes(const int amount) {
	_latitudes = amount;
	return *this;
}

std::unique_ptr<Drawable> Sphere::GeographicBuilder::build(Engine& engine) {
	auto positions = std::vector<float>{};
	auto colors = std::vector<float>{};
	auto normals = std::vector<float>{};

	// top vertex
	positions.push_back(0.0f); positions.push_back(0.0f); positions.push_back(1.0f);
	colors.push_back(srgb::RED[0]); colors.push_back(srgb::RED[1]);
	colors.push_back(srgb::RED[2]); colors.push_back(1.0f);
	normals.push_back(0.0f); normals.push_back(0.0f); normals.push_back(1.0f);

	// bottom vertex
	positions.push_back(0.0f); positions.push_back(0.0f); positions.push_back(-1.0f);
	colors.push_back(srgb::BLUE[0]); colors.push_back(srgb::BLUE[1]);
	colors.push_back(srgb::BLUE[2]); colors.push_back(1.0f);
	normals.push_back(0.0f); normals.push_back(0.0f); normals.push_back(-1.0f);

	for (auto i = 1; i < _latitudes; ++i) {
		const auto theta = static_cast<float>(i) * 
			std::numbers::pi_v<float> / static_cast<float>(_latitudes);
		for (auto j = 0; j < _longitudes; ++j) {
			const auto phi = static_cast<float>(j) * 2.0f * 
				std::numbers::pi_v<float> / static_cast<float>(_longitudes);

			const auto diX = std::sin(theta) * std::cos(phi);
			const auto diY = std::sin(theta) * std::sin(phi);
			const auto diZ = std::cos(theta);

			const auto dir = normalize(glm::vec3{ diX, diY, diZ });

			positions.push_back(dir.x); positions.push_back(dir.y); positions.push_back(dir.z);

			const auto rgb = srgb::heatColorAt(dir.z);
			colors.insert(colors.end(), rgb.begin(), rgb.end());
			colors.push_back(1.0f);

			normals.push_back(dir.x); normals.push_back(dir.y); normals.push_back(dir.z);
		}
	}

	constexpr auto floatSize = 4;
	const auto vertexBuffer = VertexBuffer::Builder(3)
		.vertexCount(static_cast<int>(positions.size()) / 3)
		.attribute(
			0, VertexBuffer::VertexAttribute::POSITION, 
			VertexBuffer::AttributeType::FLOAT3, 0, floatSize * 3
		)
		.attribute(
			1, VertexBuffer::VertexAttribute::NORMAL,
			VertexBuffer::AttributeType::FLOAT3, 0, floatSize * 3
		)
		.attribute(
			2, VertexBuffer::VertexAttribute::COLOR, 
			VertexBuffer::AttributeType::FLOAT4, 0, floatSize * 4
		)
		.build(engine);
	vertexBuffer->setBufferAt(0, positions.data());
	vertexBuffer->setBufferAt(1, normals.data());
	vertexBuffer->setBufferAt(2, colors.data());
	

	auto stripIndices = std::vector<unsigned>{};
	for (auto i = 0; i < _latitudes - 2; ++i) {
		// Connection to the previous strip
		if (i > 0) {
			stripIndices.push_back(i * _longitudes + 2);
		}
		for (auto j = 0; j < _longitudes; ++j) {
			stripIndices.push_back(i * _longitudes + j + 2);
			stripIndices.push_back((i + 1) * _longitudes + j + 2);
		}
		stripIndices.push_back(i * _longitudes + 2);
		stripIndices.push_back((i + 1) * _longitudes + 2);
		// Connection to the next strip
		if (i < _latitudes - 3) {
			stripIndices.push_back((i + 1) * _longitudes + 2);
		}
	}
	const auto stripBuffer = IndexBuffer::Builder()
		.indexCount(static_cast<int>(stripIndices.size()))
		.indexType(IndexBuffer::Builder::IndexType::UINT)
		.build(engine);
	stripBuffer->setBuffer(stripIndices.data());

	auto topIndices = std::vector{ 0u };
	for (auto i = 0; i < _longitudes; ++i) {
		topIndices.push_back(i + 2);
	}
	topIndices.push_back(2u);
	const auto topBuffer = IndexBuffer::Builder()
		.indexCount(static_cast<int>(topIndices.size()))
		.indexType(IndexBuffer::Builder::IndexType::UINT)
		.build(engine);
	topBuffer->setBuffer(topIndices.data());

	auto botIndices = std::vector{ 1u };
	const auto lastDiv = _latitudes - 2;
	for (auto i = 0; i < _longitudes; ++i) {
		botIndices.push_back(lastDiv * _longitudes + i + 2);
	}
	botIndices.push_back(lastDiv * _longitudes + 2);
	const auto botBuffer = IndexBuffer::Builder()
		.indexCount(static_cast<int>(botIndices.size()))
		.indexType(IndexBuffer::Builder::IndexType::UINT)
		.build(engine);
	botBuffer->setBuffer(botIndices.data());

	const auto shader = Shader::Builder(_shaderModel).build(engine);
	if (_shaderModel == Shader::Model::PHONG) {
        shader->use();
        shader->setUniform(Shader::Uniform::MATERIAL_AMBIENT, _phongAmbient.r, _phongAmbient.g, _phongAmbient.b);
        shader->setUniform(Shader::Uniform::MATERIAL_DIFFUSE, _phongDiffuse.r, _phongDiffuse.g, _phongDiffuse.b);
        shader->setUniform(Shader::Uniform::MATERIAL_SPECULAR, _phongSpecular.r, _phongSpecular.g, _phongSpecular.b);
        shader->setUniform(Shader::Uniform::MATERIAL_SHININESS, _phongShininess);
	}

	const auto entity = EntityManager::get()->create();
	RenderableManager::Builder(3)
		.geometry(
			0, RenderableManager::PrimitiveType::TRIANGLE_STRIP, *vertexBuffer, *stripBuffer,
			static_cast<int>(stripIndices.size()), 0
		)
		.shader(0, shader)
		.geometry(
			1, RenderableManager::PrimitiveType::TRIANGLE_FAN, *vertexBuffer, *topBuffer,
			static_cast<int>(topIndices.size()), 0
		)
		.shader(1, shader)
		.geometry(
			2, RenderableManager::PrimitiveType::TRIANGLE_FAN, *vertexBuffer, *botBuffer,
			static_cast<int>(botIndices.size()), 0
		)
		.shader(2, shader)
		.build(entity);

	return std::unique_ptr<Drawable>(new Sphere(entity, shader));
}

Sphere::SubdivisionBuilder& Sphere::SubdivisionBuilder::radius(const float radius) {
	_radius = radius;
	return *this;
}

Sphere::SubdivisionBuilder& Sphere::SubdivisionBuilder::recursiveDepth(const int depth) {
	_depth = depth;
	return *this;
}

Sphere::SubdivisionBuilder& Sphere::SubdivisionBuilder::initialPolygon(const Polyhedron polyhedron) {
	_polyhedron = polyhedron;
	return *this;
}

Sphere::SubdivisionBuilder& Sphere::SubdivisionBuilder::uniformColor(const float r, const float g, const float b) {
	if (!_uniformColor) {
		_uniformColor = new float[3] { r, g, b };
	} else {
		_uniformColor[0] = r;
		_uniformColor[1] = g;
		_uniformColor[2] = b;
	}
	return *this;
}

std::unique_ptr<Drawable> Sphere::SubdivisionBuilder::build(Engine& engine) {
	const auto faces = getFaces();
	const auto faceCount = static_cast<int>(faces.size() / 3);

	auto positions = std::vector<float>{};
	auto colors = std::vector<float>{};
	auto normals = std::vector<float>{};

	for (auto i = 0; i < faceCount; ++i) {
		const auto p0 = faces[i * 3 + 0];
		const auto p1 = faces[i * 3 + 1];
		const auto p2 = faces[i * 3 + 2];

		const auto data = subdivide(p0, p1, p2, _depth);
		positions.insert(positions.end(), data.begin(), data.end());
		normals.insert(normals.end(), data.begin(), data.end());

		const auto count = static_cast<int>(data.size() / 3);
		if (!_uniformColor) {
			const auto color = srgb::hueAt(i);
			for (auto j = 0; j < count; ++j) {
				colors.insert(colors.end(), color.begin(), color.end());
				colors.push_back(1.0f);
			}
		} else {
			for (auto j = 0; j < count; ++j) {
				colors.insert(colors.end(), _uniformColor, _uniformColor + 3);
				colors.push_back(1.0f);
			}
		}
	}

	delete _uniformColor;

	const auto vertexCount = static_cast<int>(positions.size() / 3);
	auto indices = std::vector<unsigned>{};
	for (auto i = 0; i < vertexCount; ++i) {
		indices.push_back(static_cast<unsigned>(i));
	}

	constexpr auto floatSize = 4;
	const auto vertexBuffer = VertexBuffer::Builder(3)
		.vertexCount(vertexCount)
		.attribute(
			0, VertexBuffer::VertexAttribute::POSITION,
			VertexBuffer::AttributeType::FLOAT3, 0, floatSize * 3
		)
		.attribute(
			1, VertexBuffer::VertexAttribute::COLOR,
			VertexBuffer::AttributeType::FLOAT4, 0, floatSize * 4
		)
		.attribute(
			2, VertexBuffer::VertexAttribute::NORMAL,
			VertexBuffer::AttributeType::FLOAT3, 0, floatSize * 3
		)
		.build(engine);
	vertexBuffer->setBufferAt(0, positions.data());
	vertexBuffer->setBufferAt(1, colors.data());
	vertexBuffer->setBufferAt(2, normals.data());

	const auto indexBuffer = IndexBuffer::Builder()
		.indexCount(static_cast<int>(indices.size()))
		.indexType(IndexBuffer::Builder::IndexType::UINT)
		.build(engine);
	indexBuffer->setBuffer(indices.data());

	const auto shader = Shader::Builder(_shaderModel).build(engine);
	if (_shaderModel == Shader::Model::PHONG) {
        shader->use();
        shader->setUniform(Shader::Uniform::MATERIAL_AMBIENT, _phongAmbient.r, _phongAmbient.g, _phongAmbient.b);
        shader->setUniform(Shader::Uniform::MATERIAL_DIFFUSE, _phongDiffuse.r, _phongDiffuse.g, _phongDiffuse.b);
        shader->setUniform(Shader::Uniform::MATERIAL_SPECULAR, _phongSpecular.r, _phongSpecular.g, _phongSpecular.b);
        shader->setUniform(Shader::Uniform::MATERIAL_SHININESS, _phongShininess);
	}

	const auto entity = EntityManager::get()->create();
	RenderableManager::Builder(1)
		.geometry(
			0, RenderableManager::PrimitiveType::TRIANGLES, *vertexBuffer, *indexBuffer,
			static_cast<int>(indices.size()), 0
		)
		.shader(0, shader)
		.build(entity);

	return std::unique_ptr<Drawable>(new Sphere(entity, shader));
}

std::vector<float> Sphere::SubdivisionBuilder::subdivide(
	const glm::vec3& p0, 
	const glm::vec3& p1, 
	const glm::vec3& p2,
	const int depth
) const {
	if (depth == 0) {
		return std::vector{ p0.x, p0.y, p0.z, p1.x, p1.y, p1.z, p2.x, p2.y, p2.z };
	}
	const auto m0 = _radius * normalize((p1 + p2) / 2.0f);
	const auto m1 = _radius * normalize((p0 + p2) / 2.0f);
	const auto m2 = _radius * normalize((p0 + p1) / 2.0f);

	auto vec0 = subdivide(p0, m2, m1, depth - 1);
	const auto vec1 = subdivide(m2, p1, m0, depth - 1);
	const auto vec2 = subdivide(m0, m1, m2, depth - 1);
	const auto vec3 = subdivide(m1, m0, p2, depth - 1);

	vec0.insert(vec0.end(), vec1.begin(), vec1.end());
	vec0.insert(vec0.end(), vec2.begin(), vec2.end());
	vec0.insert(vec0.end(), vec3.begin(), vec3.end());

	return vec0;
}

std::vector<glm::vec3> Sphere::SubdivisionBuilder::getFaces() const {
	if (_polyhedron == Polyhedron::TETRAHEDRON) {
		const auto v1 = glm::vec3{ glm::sqrt(8.0f / 9.0f),  0.0f,                       -1.0f / 3.0f };
		const auto v2 = glm::vec3{ -glm::sqrt(2.0f / 9.0f), glm::sqrt(2.0f / 3.0f),  -1.0f / 3.0f };
		const auto v3 = glm::vec3{ -glm::sqrt(2.0f / 9.0f), -glm::sqrt(2.0f / 3.0f), -1.0f / 3.0f };
		const auto v4 = glm::vec3{ 0.0f,                       0.0f,                       1.0f };

		// These points are supposed to get scaled by the _radius,
		// but for the time being it is considered a feature.
		return std::vector{
			v4, v3, v1,
			v4, v2, v3,
			v4, v1, v2,
			v1, v3, v2
		};
	} else { // Polyhedron::ICOSAHEDRON
		const auto ratioPos = (1.0f + 1.0f / glm::sqrt(5.0f)) / 2.0f;
		const auto ratioNeg = (1.0f - 1.0f / glm::sqrt(5.0f)) / 2.0f;
		
		const auto v01 = glm::vec3{  1.0f,  0.0f,  0.0f };
		const auto v02 = glm::vec3{ -1.0f,  0.0f,  0.0f };

		const auto v03 = glm::vec3{  1.0f / glm::sqrt(5.0f),  2.0f / glm::sqrt(5.0f), 0.0f };
		const auto v04 = glm::vec3{ -1.0f / glm::sqrt(5.0f), -2.0f / glm::sqrt(5.0f), 0.0f };

		const auto v05 = glm::vec3{  1.0f / glm::sqrt(5.0f),  ratioNeg,  glm::sqrt(ratioPos) };
		const auto v06 = glm::vec3{  1.0f / glm::sqrt(5.0f),  ratioNeg, -glm::sqrt(ratioPos) };
		const auto v07 = glm::vec3{ -1.0f / glm::sqrt(5.0f), -ratioNeg, -glm::sqrt(ratioPos) };
		const auto v08 = glm::vec3{ -1.0f / glm::sqrt(5.0f), -ratioNeg,  glm::sqrt(ratioPos) };

		const auto v09 = glm::vec3{  1.0f / glm::sqrt(5.0f), -ratioPos,  glm::sqrt(ratioNeg) };
		const auto v10 = glm::vec3{  1.0f / glm::sqrt(5.0f), -ratioPos, -glm::sqrt(ratioNeg) };
		const auto v11 = glm::vec3{ -1.0f / glm::sqrt(5.0f),  ratioPos, -glm::sqrt(ratioNeg) };
		const auto v12 = glm::vec3{ -1.0f / glm::sqrt(5.0f),  ratioPos,  glm::sqrt(ratioNeg) };

		// These points are supposed to get scaled by the _radius,
		// but for the time being it is considered a feature.
		return std::vector{
			v01, v09, v10, // top set (5)
			v01, v10, v06,
			v01, v06, v03,
			v01, v03, v05,
			v01, v05, v09,
			v09, v04, v10, // mid set (10)
			v10, v04, v07,
			v10, v07, v06,
			v06, v07, v11,
			v06, v11, v03,
			v03, v11, v12,
			v03, v12, v05,
			v05, v12, v08,
			v05, v08, v09,
			v09, v08, v04,
			v04, v08, v02, // bot set (5)
			v07, v04, v02,
			v11, v07, v02,
			v12, v11, v02,
			v08, v12, v02,
		};
	}
}
