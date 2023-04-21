// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <vector>

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RenderableManager.h"

#include "drawable/Cube.h"
#include "drawable/Color.h"

std::unique_ptr<Drawable> Cube::Builder::build(Engine& engine) {
	const auto positions = std::vector{
		// Face +X
		 1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		// Face +Y
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
		// Face +Z
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		// Face -X
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		// Face -Y
		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		// Face -Z
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
	};

	const auto normals = std::vector{
		// Face +X
		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		// Face +Y
		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		// Face +Z
		 0.0f,  0.0f,  1.0f,
		 0.0f,  0.0f,  1.0f,
		 0.0f,  0.0f,  1.0f,
		 0.0f,  0.0f,  1.0f,
		// Face -X
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		// Face -Y
		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		// Face -Z
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
	};

	const auto colors = std::vector{
		// Face X+
		srgb::BLUE[0],		srgb::BLUE[1],		srgb::BLUE[2],	  1.0f,
		srgb::BLACK[0],		srgb::BLACK[1],		srgb::BLACK[2],	  1.0f,
		srgb::CYAN[0],		srgb::CYAN[1],		srgb::CYAN[2],	  1.0f,
		srgb::GREEN[0],		srgb::GREEN[1],		srgb::GREEN[2],	  1.0f,
		// Face Y+
		srgb::CYAN[0],		srgb::CYAN[1],		srgb::CYAN[2],	  1.0f,
		srgb::GREEN[0],		srgb::GREEN[1],		srgb::GREEN[2],	  1.0f,
		srgb::WHITE[0],		srgb::WHITE[1],		srgb::WHITE[2],	  1.0f,
		srgb::YELLOW[0],	srgb::YELLOW[1],	srgb::YELLOW[2],  1.0f,
		// Face Z+
		srgb::WHITE[0],		srgb::WHITE[1],		srgb::WHITE[2],	  1.0f,
		srgb::MAGENTA[0],	srgb::MAGENTA[1],	srgb::MAGENTA[2], 1.0f,
		srgb::CYAN[0],		srgb::CYAN[1],		srgb::CYAN[2],	  1.0f,
		srgb::BLUE[0],		srgb::BLUE[1],		srgb::BLUE[2],	  1.0f,
		// Face X-
		srgb::WHITE[0],		srgb::WHITE[1],		srgb::WHITE[2],	  1.0f,
		srgb::YELLOW[0],	srgb::YELLOW[1],	srgb::YELLOW[2],  1.0f,
		srgb::MAGENTA[0],	srgb::MAGENTA[1],	srgb::MAGENTA[2], 1.0f,
		srgb::RED[0],		srgb::RED[1],		srgb::RED[2],	  1.0f,
		// Face Y-
		srgb::MAGENTA[0],	srgb::MAGENTA[1],	srgb::MAGENTA[2], 1.0f,
		srgb::RED[0],		srgb::RED[1],		srgb::RED[2],	  1.0f,
		srgb::BLUE[0],		srgb::BLUE[1],		srgb::BLUE[2],	  1.0f,
		srgb::BLACK[0],		srgb::BLACK[1],		srgb::BLACK[2],	  1.0f,
		// Face Z-
		srgb::GREEN[0],		srgb::GREEN[1],		srgb::GREEN[2],	  1.0f,
		srgb::BLACK[0],		srgb::BLACK[1],		srgb::BLACK[2],	  1.0f,
		srgb::YELLOW[0],	srgb::YELLOW[1],	srgb::YELLOW[2],  1.0f,
		srgb::RED[0],		srgb::RED[1],		srgb::RED[2],	  1.0f,
	};

	auto indices = std::vector<unsigned>{};
	for (auto i = 0; i < 6; ++i) {
		const auto it = i * 4;
		indices.push_back(it); indices.push_back(it + 1); indices.push_back(it + 2);
		indices.push_back(it + 2); indices.push_back(it + 1); indices.push_back(it + 3);
	}

	constexpr auto floatSize = 4;
	const auto vertexBuffer = VertexBuffer::Builder(3)
		.vertexCount(static_cast<int>(positions.size() / 3))
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
	
	return std::unique_ptr<Drawable>(new Cube(entity, shader));
}
