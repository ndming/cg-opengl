// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <vector>

#include "../Engine.h"
#include "../EntityManager.h"
#include "../RenderableManager.h"

#include "Tetrahedron.h"
#include "Color.h"

std::unique_ptr<Drawable> Tetrahedron::Builder::build(Engine& engine) {
	const auto positions = std::vector{
		-1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,

		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
	};

	const auto normals = std::vector{
		 1.0f,  1.0f,  0.0f,
		 1.0f,  1.0f,  0.0f,
		 1.0f,  1.0f,  0.0f,

		 0.0f, -1.0f,  1.0f,
		 0.0f, -1.0f,  1.0f,
		 0.0f, -1.0f,  1.0f,

		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,

		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
	};

	const auto colors = std::vector{
		srgb::WHITE[0],		srgb::WHITE[1],		srgb::WHITE[2],	  1.0f,
		srgb::GREEN[0],		srgb::GREEN[1],		srgb::GREEN[2],	  1.0f,
		srgb::BLUE[0],		srgb::BLUE[1],		srgb::BLUE[2],	  1.0f,

		srgb::WHITE[0],		srgb::WHITE[1],		srgb::WHITE[2],	  1.0f,
		srgb::RED[0],		srgb::RED[1],		srgb::RED[2],	  1.0f,
		srgb::GREEN[0],		srgb::GREEN[1],		srgb::GREEN[2],	  1.0f,

		srgb::WHITE[0],		srgb::WHITE[1],		srgb::WHITE[2],	  1.0f,
		srgb::BLUE[0],		srgb::BLUE[1],		srgb::BLUE[2],	  1.0f,
		srgb::RED[0],		srgb::RED[1],		srgb::RED[2],	  1.0f,

		srgb::BLUE[0],		srgb::BLUE[1],		srgb::BLUE[2],	  1.0f,
		srgb::RED[0],		srgb::RED[1],		srgb::RED[2],	  1.0f,
		srgb::GREEN[0],		srgb::GREEN[1],		srgb::GREEN[2],	  1.0f,
	};

	auto indices = std::vector<unsigned>{};
	for (unsigned i = 0; i < positions.size(); ++i) {
		indices.push_back(i);
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

	const auto indexBuffer = IndexBuffer::Builder()
		.indexCount(static_cast<int>(indices.size()))
		.indexType(IndexBuffer::Builder::IndexType::UINT)
		.build(engine);
	indexBuffer->setBuffer(indices.data());

	const auto shader = Shader::Builder(_shaderModel).build(engine);
	if (_shaderModel == Shader::Model::PHONG) {
		const auto [ambient, diffuse, specular, shininess] = MATERIAL;
		shader->use();
		shader->setUniform(
			Shader::Uniform::MATERIAL_AMBIENT,
			ambient.x, ambient.y, ambient.z
		);
		shader->setUniform(
			Shader::Uniform::MATERIAL_DIFFUSE,
			diffuse.x, diffuse.y, diffuse.z
		);
		shader->setUniform(
			Shader::Uniform::MATERIAL_SPECULAR,
			specular.x, specular.y, specular.z
		);
		shader->setUniform(Shader::Uniform::MATERIAL_SHININESS, shininess);
	}

	const auto entity = EntityManager::get()->create();
	RenderableManager::Builder(1)
		.geometry(0, RenderableManager::PrimitiveType::TRIANGLES, *vertexBuffer, *indexBuffer, 
			static_cast<int>(indices.size()), 0
		)
		.shader(0, shader)
		.build(entity);

	return std::unique_ptr<Drawable>(new Tetrahedron(entity, shader));
}