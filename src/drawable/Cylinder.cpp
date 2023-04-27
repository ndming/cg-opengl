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

using namespace srgb;

Cylinder::Builder& Cylinder::Builder::segments(const int segments) {
	_segments = segments;
	return *this;
}

std::unique_ptr<Drawable> Cylinder::Builder::build(Engine& engine) {
	auto positions = std::vector<float>{};
	auto colors = std::vector<float>{};
    auto normals = std::vector<float>{};

	const auto up = glm::vec3{ 0.0f, 0.0f, 1.0f };

    // Top center vertex
    const auto topCenter = glm::vec3{ 0.0f, 0.0f, 1.0f };
    positions.push_back(topCenter.x); positions.push_back(topCenter.y); positions.push_back(topCenter.z);
    colors.push_back(YELLOW[0]); colors.push_back(YELLOW[1]); colors.push_back(YELLOW[2]); colors.push_back(1.0f);
    normals.push_back(topCenter.x); normals.push_back(topCenter.y); normals.push_back(topCenter.z);

	// Construct circular vertices, doing this twice for each base circles for accurate normal mapping. This means
    // we are looping around 4 times
	for (auto i = 0; i < 4; ++i) {
		// The actual center, for transform reference only
        const auto multiplier = i / 2;
		const auto center = topCenter - up * (2.0f * static_cast<float>(multiplier));

		// Circular vertices, repeated at the start and end for accurate texture mapping
		for (auto j = 0; j <= _segments; ++j) {
			// The angle of rotation
			const auto angle = static_cast<float>(j) * 2.0f * std::numbers::pi_v<float> / static_cast<float>(_segments);
			// The direction to the point on circle
			const auto dir = glm::normalize(glm::vec3{ std::cos(angle), std::sin(angle), 0.0f });
			// Translate to that point
			const auto point = center + dir * 1.0f;
			positions.push_back(point.x); positions.push_back(point.y); positions.push_back(point.z);
			colors.push_back(BROWN[0]); colors.push_back(BROWN[1]); colors.push_back(BROWN[2]); colors.push_back(1.0f);
            if (i == 0) {
                normals.push_back(up.x); normals.push_back(up.y); normals.push_back(up.z);
            } else if (i == 3) {
                normals.push_back(-up.x); normals.push_back(-up.y); normals.push_back(-up.z);
            } else {
                normals.push_back(dir.x); normals.push_back(dir.y); normals.push_back(dir.z);
            }
		}
	}

    // Bottom center vertex
    const auto botCenter = glm::vec3{ 0.0f, 0.0f, -1.0f };
    positions.push_back(botCenter.x); positions.push_back(botCenter.y); positions.push_back(botCenter.z);
    colors.push_back(YELLOW[0]); colors.push_back(YELLOW[1]); colors.push_back(YELLOW[2]); colors.push_back(1.0f);
    normals.push_back(-up.x); normals.push_back(-up.y); normals.push_back(-up.z);

    const auto vertexCount = static_cast<int>(positions.size() / 3);

	auto topIndices = std::vector{ 0u };
	for (auto i = 0; i <= _segments; ++i) {
		topIndices.push_back(i + 1);
	}

	auto botIndices = std::vector{ static_cast<unsigned>(vertexCount - 1) };
	for (auto i = 0; i <= _segments; ++i) {
		botIndices.push_back(static_cast<unsigned>(vertexCount - i - 2));
	}

	auto sideIndices = std::vector<unsigned>{};
	for (auto i = 0; i <= _segments; ++i) {
        sideIndices.push_back(i + 1 + 1 * (_segments + 1));
		sideIndices.push_back(i + 1 + 2 * (_segments + 1));
	}

	constexpr auto floatSize = 4;
	const auto vertexBuffer = VertexBuffer::Builder(3)
		.vertexCount(vertexCount)
		.attribute(0, VertexBuffer::VertexAttribute::POSITION, VertexBuffer::AttributeType::FLOAT3, 0, floatSize * 3)
		.attribute(1, VertexBuffer::VertexAttribute::COLOR, VertexBuffer::AttributeType::FLOAT4, 0, floatSize * 4)
		.attribute(2, VertexBuffer::VertexAttribute::NORMAL, VertexBuffer::AttributeType::FLOAT3, 0, floatSize * 3)
        .build(engine);
	vertexBuffer->setBufferAt(0, positions.data());
	vertexBuffer->setBufferAt(1, colors.data());
    vertexBuffer->setBufferAt(2, normals.data());

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

	const auto shader = defaultShader(engine);
    const auto entity = EntityManager::get()->create();
	RenderableManager::Builder(3)
		.geometry(0, RenderableManager::PrimitiveType::TRIANGLE_STRIP, *vertexBuffer, *sideBuffer,static_cast<int>(sideIndices.size()), 0)
		.shader(0, shader)
		.geometry(1, RenderableManager::PrimitiveType::TRIANGLE_FAN, *vertexBuffer, *topBuffer,static_cast<int>(topIndices.size()), 0)
		.shader(1, shader)
		.geometry(2, RenderableManager::PrimitiveType::TRIANGLE_FAN, *vertexBuffer, *botBuffer,static_cast<int>(botIndices.size()), 0)
		.shader(2, shader)
		.build(entity);

	return std::unique_ptr<Drawable>(new Cylinder(entity, shader));
}
