// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <cmath>
#include <glm/geometric.hpp>
#include <numbers>
#include <vector>

#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "RenderableManager.h"

#include "drawable/Cylinder.h"
#include "drawable/Color.h"

Cylinder::Builder& Cylinder::Builder::segments(const int segments) {
	_segments = segments;
	return *this;
}

std::unique_ptr<Drawable> Cylinder::Builder::build(Engine& engine) {
	auto positions = std::vector<float>{};
	auto colors = std::vector<float>{};

	const auto baseCenter = glm::vec3{ 0.0f, 0.0f, -1.0f };
	const auto up = glm::vec3{ 0.0f, 0.0f, 1.0f };

	// Do this twice for two base circles
	for (auto i = 0; i < 2; ++i) {
		// the actual center
		const auto center = baseCenter + up * (2.0f * static_cast<float>(i));

		// center vertex
		positions.push_back(center.x);
		positions.push_back(center.y);
		positions.push_back(center.z);
		colors.push_back(srgb::YELLOW[0]);
		colors.push_back(srgb::YELLOW[1]);
		colors.push_back(srgb::YELLOW[2]);
		colors.push_back(1.0f);

		// circular vertices
		for (auto j = 0; j < _segments; ++j) {
			// the angle of rotation
			const auto angle = static_cast<float>(j) * 2.0f * 
				std::numbers::pi_v<float> / static_cast<float>(_segments);
			// rotation vector on the XY-plane
			const auto rot = glm::vec3{ std::cos(angle), std::sin(angle), 0.0f };
			// the direction to the point on circle
			const auto dir = normalize(cross(up, rot));
			// translate to that point
			const auto point = center + dir * 1.0f;
			positions.push_back(point.x); positions.push_back(point.y); positions.push_back(point.z);
			colors.push_back(srgb::BROWN[0]); colors.push_back(srgb::BROWN[1]);
			colors.push_back(srgb::BROWN[2]); colors.push_back(1.0f);
		}
	}

	auto topIndices = std::vector{ 0u };
	for (auto i = 0; i < _segments; ++i) {
		topIndices.push_back(i + 1);
	}
	topIndices.push_back(1u);

	auto botIndices = std::vector{ static_cast<unsigned>(_segments + 1) };
	for (auto i = 0; i < _segments; ++i) {
		botIndices.push_back(i + _segments + 2);
	}
	botIndices.push_back(_segments + 2);

	auto sideIndices = std::vector<unsigned>{};
	for (auto i = 0; i < _segments; ++i) {
		sideIndices.push_back(i + 2 + _segments);
		sideIndices.push_back(i + 1);
	}
	sideIndices.push_back(2 + _segments);
	sideIndices.push_back(1u);

	constexpr auto floatSize = 4;
	const auto vertexBuffer = VertexBuffer::Builder(2)
		.vertexCount(static_cast<int>(positions.size() / 3))
		.attribute(0, VertexBuffer::VertexAttribute::POSITION, VertexBuffer::AttributeType::FLOAT3, 0, floatSize * 3)
		.attribute(1, VertexBuffer::VertexAttribute::COLOR, VertexBuffer::AttributeType::FLOAT4, 0, floatSize * 4)
		.build(engine);
	vertexBuffer->setBufferAt(0, positions.data());
	vertexBuffer->setBufferAt(1, colors.data());

	const auto topBuffer = IndexBuffer::Builder()
		.indexCount(static_cast<int>(topIndices.size()))
		.indexType(IndexBuffer::Builder::IndexType::UINT)
		.build(engine);
	topBuffer->setBuffer(topIndices.data());

	const auto botBuffer = IndexBuffer::Builder()
		.indexCount(static_cast<int>(botIndices.size()))
		.indexType(IndexBuffer::Builder::IndexType::UINT)
		.build(engine);
	botBuffer->setBuffer(botIndices.data());

	const auto sideBuffer = IndexBuffer::Builder()
		.indexCount(static_cast<int>(sideIndices.size()))
		.indexType(IndexBuffer::Builder::IndexType::UINT)
		.build(engine);
	sideBuffer->setBuffer(sideIndices.data());

	const auto shader = Shader::Builder(_shaderModel).build(engine);
	const auto entity = EntityManager::get()->create();

	RenderableManager::Builder(3)
		.geometry(
			0, RenderableManager::PrimitiveType::TRIANGLE_STRIP, *vertexBuffer, *sideBuffer,
			static_cast<int>(sideIndices.size()), 0
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

	return std::unique_ptr<Drawable>(new Cylinder(entity, shader));
}
