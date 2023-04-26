// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include "Engine.h"
#include "EntityManager.h"
#include "RenderableManager.h"

#include "drawable/Pyramid.h"
#include "drawable/Color.h"

using namespace glm;
using namespace srgb;

std::unique_ptr<Drawable> Pyramid::Builder::build(Engine& engine) {
    const auto positions = std::vector{
        // Side +X
         0.0f,  0.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        // Side +Y
         0.0f,  0.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        // Side -X
         0.0f,  0.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        // Side -Y
         0.0f,  0.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        // Face -Z
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
    };


    const auto xpNorm = normalize(cross(vec3{  0.0f,  1.0f, 0.0f }, vec3{ -1.0f,  0.0f, 1.0f }));
    const auto xnNorm = normalize(cross(vec3{  0.0f, -1.0f, 0.0f }, vec3{  1.0f,  0.0f, 1.0f }));
    const auto ypNorm = normalize(cross(vec3{ -1.0f,  0.0f, 0.0f }, vec3{  0.0f, -1.0f, 1.0f }));
    const auto ynNorm = normalize(cross(vec3{  1.0f,  0.0f, 0.0f }, vec3{  0.0f,  1.0f, 1.0f }));
    const auto normals = std::vector{
        // Side +X
        xpNorm.x, xpNorm.y, xpNorm.z,
        xpNorm.x, xpNorm.y, xpNorm.z,
        xpNorm.x, xpNorm.y, xpNorm.z,
        // Side +Y
        ypNorm.x, ypNorm.y, ypNorm.z,
        ypNorm.x, ypNorm.y, ypNorm.z,
        ypNorm.x, ypNorm.y, ypNorm.z,
        // Side -X
        xnNorm.x, xnNorm.y, xnNorm.z,
        xnNorm.x, xnNorm.y, xnNorm.z,
        xnNorm.x, xnNorm.y, xnNorm.z,
        // Side -Y
        ynNorm.x, ynNorm.y, ynNorm.z,
        ynNorm.x, ynNorm.y, ynNorm.z,
        ynNorm.x, ynNorm.y, ynNorm.z,
        // Face -Z
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
    };

    const auto colors = std::vector{
        // Side X+
        WHITE[0],    WHITE[1],    WHITE[2],    1.0f,
        GREEN[0],    GREEN[1],    GREEN[2],    1.0f,
        BLUE[0],     BLUE[1],     BLUE[2],     1.0f,
        // Face Y+
        WHITE[0],    WHITE[1],    WHITE[2],    1.0f,
        BLUE[0],     BLUE[1],     BLUE[2],     1.0f,
        MAGENTA[0],  MAGENTA[1],  MAGENTA[2],  1.0f,
        // Face X-
        WHITE[0],    WHITE[1],    WHITE[2],    1.0f,
        MAGENTA[0],  MAGENTA[1],  MAGENTA[2],  1.0f,
        RED[0],      RED[1],      RED[2],      1.0f,
        // Face Y-
        WHITE[0],    WHITE[1],    WHITE[2],    1.0f,
        RED[0],      RED[1],      RED[2],      1.0f,
        GREEN[0],    GREEN[1],    GREEN[2],    1.0f,
        // Face Z-
        RED[0],      RED[1],      RED[2],      1.0f,
        MAGENTA[0],  MAGENTA[1],  MAGENTA[2],  1.0f,
        GREEN[0],    GREEN[1],    GREEN[2],    1.0f,
        BLUE[0],     BLUE[1],     BLUE[2],     1.0f,
    };

    // Draw 4 triangles first, then draw the base square
    const auto vertexCount = 4 * 3 + 4;
    auto indices = std::vector<unsigned>{};
    for (auto i = 0; i < vertexCount; ++i) {
        indices.push_back(i);
    }

	constexpr auto floatSize = 4;
	const auto vertexBuffer = VertexBuffer::Builder(3)
		.vertexCount(vertexCount)
		.attribute(0, VertexBuffer::VertexAttribute::POSITION, VertexBuffer::AttributeType::FLOAT3, 0, floatSize * 3)
		.attribute(1, VertexBuffer::VertexAttribute::NORMAL, VertexBuffer::AttributeType::FLOAT3, 0, floatSize * 3)
        .attribute(2, VertexBuffer::VertexAttribute::COLOR, VertexBuffer::AttributeType::FLOAT4, 0, floatSize * 4)
		.build(engine);
	vertexBuffer->setBufferAt(0, positions.data());
    vertexBuffer->setBufferAt(1, normals.data());
	vertexBuffer->setBufferAt(2, colors.data());

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
	RenderableManager::Builder(2)
		.geometry(0, RenderableManager::PrimitiveType::TRIANGLES, *vertexBuffer, *indexBuffer, 4 * 3, 0)
		.shader(0, shader)
		.geometry(1, RenderableManager::PrimitiveType::TRIANGLE_STRIP, *vertexBuffer, *indexBuffer, 4, 4 * 3)
		.shader(1, shader)
		.build(entity);

	return std::unique_ptr<Drawable>(new Pyramid(entity, shader));
}