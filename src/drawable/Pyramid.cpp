// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include "Engine.h"
#include "EntityManager.h"
#include "RenderableManager.h"

#include "drawable/Pyramid.h"
#include "drawable/Color.h"

std::unique_ptr<Drawable> Pyramid::Builder::build(Engine& engine) {
	const auto positions = std::vector{
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		 0.0f,  0.0f,  1.0f,
	};

	const auto colors = std::vector{
		srgb::RED[0],       srgb::RED[1],       srgb::RED[2],	  1.0f,
		srgb::GREEN[0],     srgb::GREEN[1],     srgb::GREEN[2],	  1.0f,
		srgb::BLUE[0],      srgb::BLUE[1],      srgb::BLUE[2],	  1.0f,
		srgb::MAGENTA[0],   srgb::MAGENTA[1],   srgb::MAGENTA[2], 1.0f,
		srgb::WHITE[0],	    srgb::WHITE[1],	    srgb::WHITE[2],	  1.0f,
	};

	const auto indices = std::vector{
		4u, 0u, 1u, 2u, 3u, 0u,	// top
		0u, 3u, 1u, 2u,         // base
	};

	constexpr auto floatSize = 4;
	const auto vertexBuffer = VertexBuffer::Builder(2)
		.vertexCount(5)
		.attribute(0, VertexBuffer::VertexAttribute::POSITION, VertexBuffer::AttributeType::FLOAT3, 0, floatSize * 3)
		.attribute(1, VertexBuffer::VertexAttribute::COLOR, VertexBuffer::AttributeType::FLOAT4, 0, floatSize * 4)
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
	RenderableManager::Builder(2)
		.geometry(0, RenderableManager::PrimitiveType::TRIANGLE_FAN, *vertexBuffer, *indexBuffer, 6, 0)
		.shader(0, shader)
		.geometry(1, RenderableManager::PrimitiveType::TRIANGLE_STRIP, *vertexBuffer, *indexBuffer, 4, 6)
		.shader(1, shader)
		.build(entity);

	return std::unique_ptr<Drawable>(new Pyramid(entity, shader));
}