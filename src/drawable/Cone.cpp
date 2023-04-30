// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <cmath>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <numbers>

#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "RenderableManager.h"

#include "drawable/Cone.h"
#include "drawable/Color.h"

using namespace srgb;

Cone::Builder& Cone::Builder::segments(const int segments) {
	_segments = segments;
	return *this;
}

std::unique_ptr<Drawable> Cone::Builder::build(Engine& engine) {
    auto positions = std::vector<float>{};
    auto colors = std::vector<float>{};
    auto normals = std::vector<float>{};
    auto texCoords = std::vector<float>{};

    const auto baseCenter = glm::vec3{ 0.0f, 0.0f, -1.0f };
    const auto up = glm::vec3{ 0.0f, 0.0f, 1.0f };

    // Top vertices, repeated for accurate texture mapping
    const auto top = baseCenter + up * 2.0f;
    for (auto i = 0; i < _segments; ++i) {
        positions.push_back(top.x); positions.push_back(top.y); positions.push_back(top.z);
        colors.push_back(RED[0]); colors.push_back(RED[1]); colors.push_back(RED[2]); colors.push_back(1.0f);
        normals.push_back(up.x); normals.push_back(up.y); normals.push_back(up.z);
        // We divide by (segments - 1) to make sure the final u-texCoord reach 1.0f
        const auto u = static_cast<float>(i) / static_cast<float>(_segments - 1);
        texCoords.push_back(u); texCoords.push_back(0.0f);
    }

	// Circular vertices, twice for accurate normal mapping
    for (auto i = 0; i < 2; ++i) {
        // Repeat the first and last vertex for accurate texture mapping
        for (auto j = 0; j <= _segments; ++j) {
            const auto angle = static_cast<float>(j) * 2.0f * std::numbers::pi_v<float> / static_cast<float>(_segments);
            const auto dir = glm::normalize(glm::vec3{ std::cos(angle), std::sin(angle), 0.0f });
            const auto point = baseCenter + dir * 1.0f;
            positions.push_back(point.x); positions.push_back(point.y); positions.push_back(point.z);
            colors.push_back(PURPLE[0]); colors.push_back(PURPLE[1]); colors.push_back(PURPLE[2]); colors.push_back(1.0f);
            if (i == 0) {
                const auto tangent = glm::cross(up, dir);
                const auto norm = glm::normalize(glm::cross(tangent, top - point));
                normals.push_back(norm.x); normals.push_back(norm.y); normals.push_back(norm.z);

                const auto u = static_cast<float>(j) / static_cast<float>(_segments);
                texCoords.push_back(u); texCoords.push_back(1.0f);
            } else {
                normals.push_back(-up.x); normals.push_back(-up.y); normals.push_back(-up.z);

                const auto u = 0.5f + 0.5f * std::cos(angle);
                const auto v = 0.5f + 0.5f * std::sin(angle);
                texCoords.push_back(u); texCoords.push_back(v);
            }
        }
    }

    // Additional bottom center vertex for circle drawing with GL_TRIANGLE_FAN
    positions.push_back(baseCenter.x); positions.push_back(baseCenter.y); positions.push_back(baseCenter.z);
    colors.push_back(CYAN[0]); colors.push_back(CYAN[1]); colors.push_back(CYAN[2]); colors.push_back(1.0f);
    normals.push_back(-up.x); normals.push_back(-up.y); normals.push_back(-up.z);
    texCoords.push_back(0.5f); texCoords.push_back(0.5f);

    // We will need 2 index buffers to draw the cone's side and the cone's base circle
    const auto vertexCount = static_cast<int>(positions.size() / 3);
	auto baseIndices = std::vector{ static_cast<unsigned>(vertexCount - 1) };
	for (auto i = 0; i <= _segments; ++i) {
		baseIndices.push_back(vertexCount - 2 - i);
	}

	auto sideIndices = std::vector<unsigned>{};
	for (auto i = 0; i < _segments; ++i) {
        // Top vertex at segment i
        sideIndices.push_back(i);
        // Two more vertices that makes up the triangle, padding by segments number of vertices
        sideIndices.push_back(i + _segments);
        sideIndices.push_back(i + _segments + 1);
	}

	constexpr auto floatSize = 4;
	const auto vertexBuffer = VertexBuffer::Builder(4)
		.vertexCount(vertexCount)
		.attribute(0, VertexBuffer::VertexAttribute::POSITION, VertexBuffer::AttributeType::FLOAT3, 0, floatSize * 3)
		.attribute(1, VertexBuffer::VertexAttribute::COLOR, VertexBuffer::AttributeType::FLOAT4, 0, floatSize * 4)
		.attribute(2, VertexBuffer::VertexAttribute::NORMAL, VertexBuffer::AttributeType::FLOAT3, 0, floatSize * 3)
        .attribute(3, VertexBuffer::VertexAttribute::UV0, VertexBuffer::AttributeType::FLOAT2, 0, floatSize * 2)
        .build(engine);
	vertexBuffer->setBufferAt(0, positions.data());
	vertexBuffer->setBufferAt(1, colors.data());
    vertexBuffer->setBufferAt(2, normals.data());
    vertexBuffer->setBufferAt(3, texCoords.data());

	const auto baseIdxBuff = IndexBuffer::Builder()
		.indexCount(static_cast<int>(baseIndices.size()))
		.indexType(IndexBuffer::Builder::IndexType::UINT)
		.build(engine);
	baseIdxBuff->setBuffer(baseIndices.data());

	const auto sideIdxBuff = IndexBuffer::Builder()
		.indexCount(static_cast<int>(sideIndices.size()))
		.indexType(IndexBuffer::Builder::IndexType::UINT)
		.build(engine);
	sideIdxBuff->setBuffer(sideIndices.data());

	const auto shader = defaultShader(engine);
	const auto entity = EntityManager::get()->create();
	RenderableManager::Builder(2)
		.geometry(0, RenderableManager::PrimitiveType::TRIANGLE_FAN, *vertexBuffer, *baseIdxBuff, static_cast<int>(baseIndices.size()), 0)
		.shader(0, shader)
		.geometry(1, RenderableManager::PrimitiveType::TRIANGLES, *vertexBuffer, *sideIdxBuff, static_cast<int>(sideIndices.size()), 0)
		.shader(1, shader)
		.build(entity);

	return std::unique_ptr<Drawable>(new Cone(entity, shader));
}
