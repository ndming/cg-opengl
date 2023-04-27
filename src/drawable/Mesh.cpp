// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <glm/geometric.hpp>
#include <vector>

#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "RenderableManager.h"

#include "drawable/Mesh.h"
#include "drawable/Color.h"

Mesh::Builder& Mesh::Builder::halfExtentX(const float extent) {
	_halfExtentX = extent;
	if (_halfExtentX < MIN_EXTENT) {
		_halfExtentX = MIN_EXTENT;
	}
	return *this;
}

Mesh::Builder& Mesh::Builder::halfExtentY(const float extent) {
	_halfExtentY = extent;
	if (_halfExtentY < MIN_EXTENT) {
		_halfExtentY = MIN_EXTENT;
	}
	return *this;
}

Mesh::Builder& Mesh::Builder::halfExtent(const float extent) {
	return halfExtentX(extent).halfExtentY(extent);
}

Mesh::Builder& Mesh::Builder::segmentsX(const int segments) {
	_segmentsX = segments;
	if (_segmentsX < MIN_SEGMENTS) {
		_segmentsX = MIN_SEGMENTS;
	}
	return *this;
}

Mesh::Builder& Mesh::Builder::segmentsY(const int segments) {
	_segmentsY = segments;
	if (_segmentsY < MIN_SEGMENTS) {
		_segmentsY = MIN_SEGMENTS;
	}
	return *this;
}

Mesh::Builder& Mesh::Builder::segments(const int segments) {
	return segmentsX(segments).segmentsY(segments);
}

std::unique_ptr<Drawable> Mesh::Builder::build(Engine& engine) {
	auto positions = std::vector<float>{};
	auto colors = std::vector<float>{};
	auto normals = std::vector<float>{};

	const auto xStep = _halfExtentX * 2 / static_cast<float>(_segmentsX);
	const auto yStep = _halfExtentY * 2 / static_cast<float>(_segmentsY);

	for (auto i = 0; i < _segmentsX + 1; ++i) {
		for (auto j = 0; j < _segmentsY + 1; ++j) {
			// Acquire the x, y coordinate
			const auto x0 = static_cast<float>(i) * xStep - _halfExtentX;	// from top left
			const auto y0 = _halfExtentY - static_cast<float>(j) * yStep;	// to bottom right
			// Evaluate z, could be a NaN, but let's view it as a feature
			const auto z0 = _func(x0, y0);

			positions.push_back(x0); positions.push_back(y0); positions.push_back(z0);

			// Determine the normals
			const auto x1 = static_cast<float>(i + 1) * xStep - _halfExtentX;
			const auto y1 = y0;
			const auto z1 = _func(x1, y1);

			const auto x2 = x0;
			const auto y2 = _halfExtentY - static_cast<float>(j + 1) * yStep;
			const auto z2 = _func(x2, y2);

			const auto x3 = static_cast<float>(i - 1) * xStep - _halfExtentX;
			const auto y3 = y2;
			const auto z3 = _func(x3, y3);

			const auto x4 = static_cast<float>(i - 1) * xStep - _halfExtentX;
			const auto y4 = y0;
			const auto z4 = _func(x4, y4);

			const auto x5 = x0;
			const auto y5 = _halfExtentY - static_cast<float>(j - 1) * yStep;
			const auto z5 = _func(x5, y5);

			const auto x6 = static_cast<float>(i + 1) * xStep - _halfExtentX;
			const auto y6 = y5;
			const auto z6 = _func(x6, y6);

			const auto vec01 = glm::vec3{ x1 - x0, y1 - y0, z1 - z0 };
			const auto vec02 = glm::vec3{ x2 - x0, y2 - y0, z2 - z0 };
			const auto vec03 = glm::vec3{ x3 - x0, y3 - y0, z3 - z0 };
			const auto vec04 = glm::vec3{ x4 - x0, y4 - y0, z4 - z0 };
			const auto vec05 = glm::vec3{ x5 - x0, y5 - y0, z5 - z0 };
			const auto vec06 = glm::vec3{ x6 - x0, y6 - y0, z6 - z0 };

			const auto norm1 = normalize(cross(vec01, vec06));
			const auto norm2 = normalize(cross(vec02, vec01));
			const auto norm3 = normalize(cross(vec03, vec02));
			const auto norm4 = normalize(cross(vec04, vec03));
			const auto norm5 = normalize(cross(vec05, vec04));
			const auto norm6 = normalize(cross(vec06, vec05));

			const auto normal = (norm1 + norm2 + norm3 + norm4 + norm5 + norm6) / 6.0f;
			normals.push_back(normal.x); normals.push_back(normal.y); normals.push_back(normal.z);

			const auto rgb = srgb::heatColorAt(z0);
			colors.push_back(rgb[0]); colors.push_back(rgb[1]);
			colors.push_back(rgb[2]); colors.push_back(1.0f);
		}
	}

	constexpr auto floatSize = 4;
	const auto vertexBuffer = VertexBuffer::Builder(3)
		.vertexCount(static_cast<int>(positions.size() / 3))
		.attribute(0, VertexBuffer::VertexAttribute::POSITION, VertexBuffer::AttributeType::FLOAT3, 0, floatSize * 3)
		.attribute(1, VertexBuffer::VertexAttribute::COLOR, VertexBuffer::AttributeType::FLOAT4, 0, floatSize * 4)
		.attribute(2, VertexBuffer::VertexAttribute::NORMAL, VertexBuffer::AttributeType::FLOAT3, 0, floatSize * 3)
		.build(engine);
	vertexBuffer->setBufferAt(0, positions.data());
	vertexBuffer->setBufferAt(1, colors.data());
	vertexBuffer->setBufferAt(2, normals.data());

	const auto shader = defaultShader(engine);
	const auto entity = EntityManager::get()->create();

	auto renderableBuilder = RenderableManager::Builder(_segmentsY);

	for (auto i = 0; i < _segmentsY; ++i) {
		auto indices = std::vector<unsigned>{};
		// for each column pair of vertices starting at the least x
		for (auto j = 0; j < _segmentsX + 1; ++j) {
			indices.push_back(j + i * (_segmentsX + 1));
			indices.push_back(j + (i + 1) * (_segmentsX + 1));
		}

		const auto indexBuffer = IndexBuffer::Builder()
			.indexCount(static_cast<int>(indices.size()))
			.indexType(IndexBuffer::Builder::IndexType::UINT)
			.build(engine);
		indexBuffer->setBuffer(indices.data());

		renderableBuilder
            .geometry(i, RenderableManager::PrimitiveType::TRIANGLE_STRIP, *vertexBuffer, *indexBuffer,static_cast<int>(indices.size()), 0)
            .shader(i, shader);
	}
	renderableBuilder.build(entity);

	return std::unique_ptr<Drawable>(new Mesh(entity, shader));
}
