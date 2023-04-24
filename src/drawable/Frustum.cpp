// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include "Engine.h"
#include "EntityManager.h"
#include "RenderableManager.h"

#include "drawable/Frustum.h"
#include "drawable/Color.h"

std::unique_ptr<Drawable> Frustum::Builder::build(Engine& engine) {
	const auto positions = std::vector{
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-0.5f, -0.5f,  1.0f,
		 0.5f, -0.5f,  1.0f,
		 0.5f,  0.5f,  1.0f,
		-0.5f,  0.5f,  1.0f,
	};

	const auto colors = std::vector{
		srgb::RED[0],		srgb::RED[1],		srgb::RED[2],	  1.0f,
		srgb::BLACK[0],		srgb::BLACK[1],		srgb::BLACK[2],	  1.0f,
		srgb::GREEN[0],		srgb::GREEN[1],		srgb::GREEN[2],	  1.0f,
		srgb::YELLOW[0],	srgb::YELLOW[1],	srgb::YELLOW[2],  1.0f,
		srgb::MAGENTA[0],	srgb::MAGENTA[1],	srgb::MAGENTA[2], 1.0f,
		srgb::BLUE[0],		srgb::BLUE[1],		srgb::BLUE[2],	  1.0f,
		srgb::CYAN[0],		srgb::CYAN[1],		srgb::CYAN[2],	  1.0f,
		srgb::WHITE[0],		srgb::WHITE[1],		srgb::WHITE[2],	  1.0f,
	};

	const auto indices = std::vector{
		4u, 0u, 5u, 1u, 6u, 2u, 7u, 3u, 4u, 0u,	// side faces
		4u, 5u, 7u, 6u,	// top face
		3u, 2u, 0u, 1u,	// bottom face
	};

	constexpr auto floatSize = 4;
	const auto vertexBuffer = VertexBuffer::Builder(2)
		.vertexCount(8)
		.attribute(
			0, VertexBuffer::VertexAttribute::POSITION,
			VertexBuffer::AttributeType::FLOAT3, 0, floatSize * 3
		)
		.attribute(
			1, VertexBuffer::VertexAttribute::COLOR,
			VertexBuffer::AttributeType::FLOAT4, 0, floatSize * 4
		)
		.build(engine);
	vertexBuffer->setBufferAt(0, positions.data());
	vertexBuffer->setBufferAt(1, colors.data());

	const auto indexBuffer = IndexBuffer::Builder()
		.indexCount(static_cast<int>(indices.size()))
		.indexType(IndexBuffer::Builder::IndexType::UINT)
		.build(engine);
	indexBuffer->setBuffer(indices.data());

	const auto shader = Shader::Builder(_shaderModel).build(engine);
	const auto entity = EntityManager::get()->create();

	RenderableManager::Builder(3)
		.geometry(0, RenderableManager::PrimitiveType::TRIANGLE_STRIP, *vertexBuffer, *indexBuffer, 10, 0)
		.shader(0, shader)
		.geometry(1, RenderableManager::PrimitiveType::TRIANGLE_STRIP, *vertexBuffer, *indexBuffer, 4, 10)
		.shader(1, shader)
		.geometry(2, RenderableManager::PrimitiveType::TRIANGLE_STRIP, *vertexBuffer, *indexBuffer, 4, 14)
		.shader(2, shader)
		.build(entity);

	return std::unique_ptr<Drawable>(new Frustum(entity, shader));
}