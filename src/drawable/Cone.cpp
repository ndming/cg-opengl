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

    const auto baseCenter = glm::vec3{ 0.0f, 0.0f, -1.0f };
    const auto up = glm::vec3{ 0.0f, 0.0f, 1.0f };

    // Top vertex
    const auto top = baseCenter + up * 2.0f;
    positions.push_back(top.x); positions.push_back(top.y); positions.push_back(top.z);
    colors.push_back(RED[0]); colors.push_back(RED[1]); colors.push_back(RED[2]); colors.push_back(1.0f);
    normals.push_back(up.x); normals.push_back(up.y); normals.push_back(up.z);

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
            } else {
                normals.push_back(-up.x); normals.push_back(-up.y); normals.push_back(-up.z);
            }
        }
    }

    // Additional bottom center vertex for circle drawing with GL_TRIANGLE_FAN
    positions.push_back(baseCenter.x); positions.push_back(baseCenter.y); positions.push_back(baseCenter.z);
    colors.push_back(CYAN[0]); colors.push_back(CYAN[1]); colors.push_back(CYAN[2]); colors.push_back(1.0f);
    normals.push_back(-up.x); normals.push_back(-up.y); normals.push_back(-up.z);

    // We will need 2 index buffers to draw the cone's side and the cone's base circle
    const auto vertexCount = static_cast<int>(positions.size() / 3);
	auto baseIndices = std::vector{ static_cast<unsigned>(vertexCount - 1) };
	for (auto i = 0; i <= _segments; ++i) {
		baseIndices.push_back(vertexCount - 2 - i);
	}

	auto sideIndices = std::vector{ 0u };
	for (auto i = 0; i <= _segments; ++i) {
		sideIndices.push_back(i + 1);
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

	const auto shader = Shader::Builder(_shaderModel).build(engine);
    if (_shaderModel == Shader::Model::PHONG) {
        shader->use();
        shader->setUniform(Shader::Uniform::MATERIAL_AMBIENT, _phongAmbient.r, _phongAmbient.g, _phongAmbient.b);
        shader->setUniform(Shader::Uniform::MATERIAL_DIFFUSE, _phongDiffuse.r, _phongDiffuse.g, _phongDiffuse.b);
        shader->setUniform(Shader::Uniform::MATERIAL_SPECULAR, _phongSpecular.r, _phongSpecular.g, _phongSpecular.b);
        shader->setUniform(Shader::Uniform::MATERIAL_SHININESS, _phongShininess);
    }

	const auto entity = EntityManager::get()->create();
	RenderableManager::Builder(2)
		.geometry(0, RenderableManager::PrimitiveType::TRIANGLE_FAN, *vertexBuffer, *baseIdxBuff, static_cast<int>(baseIndices.size()), 0)
		.shader(0, shader)
		.geometry(1, RenderableManager::PrimitiveType::TRIANGLE_FAN, *vertexBuffer, *sideIdxBuff, static_cast<int>(sideIndices.size()), 0)
		.shader(1, shader)
		.build(entity);

	return std::unique_ptr<Drawable>(new Cone(entity, shader));
}
