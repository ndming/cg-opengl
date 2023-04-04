// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <cmath>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <numbers>

#include "../IndexBuffer.h"
#include "../VertexBuffer.h"
#include "../RenderableManager.h"

#include "Cone.h"
#include "Color.h"

Cone::Builder& Cone::Builder::segments(const int segments) {
	_segments = segments;
	return *this;
}

std::unique_ptr<Drawable> Cone::Builder::build(Engine& engine) {
	const auto baseCenter = glm::vec3{ 0.0f, 0.0f, -1.0f };
	const auto up = glm::vec3{ 0.0f, 0.0f, 1.0f };

	auto positions = std::vector{
		baseCenter.x, baseCenter.y, baseCenter.z
	};
	auto colors = std::vector{
		srgb::CYAN[0], srgb::CYAN[1], srgb::CYAN[2], 1.0f
	};

	// Base circle
	for (auto i = 0; i < _segments; ++i) {
		const auto angle = static_cast<float>(i) * 2.0f * 
			std::numbers::pi_v<float> / static_cast<float>(_segments);
		const auto rot = glm::vec3{ std::cos(angle), std::sin(angle), 0.0f };
		const auto dir = normalize(cross(up, rot));
		const auto point = baseCenter + dir * 1.0f;
		positions.push_back(point.x); positions.push_back(point.y); positions.push_back(point.z);
		colors.push_back(srgb::PURPLE[0]); colors.push_back(srgb::PURPLE[1]);
		colors.push_back(srgb::PURPLE[2]); colors.push_back(1.0f);
	}
	// Top vertex
	const auto top = baseCenter + up * 2.0f;
	positions.push_back(top.x); positions.push_back(top.y); positions.push_back(top.z);
	colors.push_back(srgb::RED[0]); colors.push_back(srgb::RED[1]);
	colors.push_back(srgb::RED[2]); colors.push_back(1.0f);

	auto circleIndices = std::vector{ 0u };
	for (auto i = 0; i < _segments; ++i) {
		circleIndices.push_back(i + 1);
	}
	circleIndices.push_back(1u);

	auto coneIndices = std::vector{ static_cast<unsigned>(_segments + 1) };
	for (auto i = 0; i < _segments; ++i) {
		coneIndices.push_back(i + 1);
	}
	coneIndices.push_back(1u);

	constexpr auto floatSize = 4;
	const auto vertexBuffer = VertexBuffer::Builder(2)
		.vertexCount(static_cast<int>(positions.size() / 3))
		.attribute(0, VertexBuffer::VertexAttribute::POSITION, VertexBuffer::AttributeType::FLOAT3, 0, floatSize * 3)
		.attribute(1, VertexBuffer::VertexAttribute::COLOR, VertexBuffer::AttributeType::FLOAT4, 0, floatSize * 4)
		.build(engine);
	vertexBuffer->setBufferAt(0, positions.data());
	vertexBuffer->setBufferAt(1, colors.data());

	const auto circleBuffer = IndexBuffer::Builder()
		.indexCount(static_cast<int>(circleIndices.size()))
		.indexType(IndexBuffer::Builder::IndexType::UINT)
		.build(engine);
	circleBuffer->setBuffer(circleIndices.data());

	const auto coneBuffer = IndexBuffer::Builder()
		.indexCount(static_cast<int>(coneIndices.size()))
		.indexType(IndexBuffer::Builder::IndexType::UINT)
		.build(engine);
	coneBuffer->setBuffer(coneIndices.data());

	const auto shader = Shader::Builder(_shaderModel).build(engine);
	const auto entity = EntityManager::get()->create();

	RenderableManager::Builder(2)
		.geometry(
			0, RenderableManager::PrimitiveType::TRIANGLE_FAN, *vertexBuffer, *circleBuffer,
			static_cast<int>(circleIndices.size()), 0
		)
		.shader(0, shader)
		.geometry(
			1, RenderableManager::PrimitiveType::TRIANGLE_FAN, *vertexBuffer, *coneBuffer,
			static_cast<int>(coneIndices.size()), 0
		)
		.shader(1, shader)
		.build(entity);

	return std::unique_ptr<Drawable>(new Cone(entity, shader));
}
