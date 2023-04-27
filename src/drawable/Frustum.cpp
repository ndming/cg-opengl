// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include "Engine.h"
#include "EntityManager.h"
#include "RenderableManager.h"

#include "drawable/Frustum.h"
#include "drawable/Color.h"

using namespace srgb;
using namespace glm;

std::unique_ptr<Drawable> Frustum::Builder::build(Engine& engine) {
    const auto positions = std::vector{
        // Face +X
         0.5f, -0.5f,  1.0f,
         1.0f, -1.0f, -1.0f,
         0.5f,  0.5f,  1.0f,
         1.0f,  1.0f, -1.0f,
        // Face +Y
         0.5f,  0.5f,  1.0f,
         1.0f,  1.0f, -1.0f,
        -0.5f,  0.5f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        // Face +Z
        -0.5f,  0.5f,  1.0f,
        -0.5f, -0.5f,  1.0f,
         0.5f,  0.5f,  1.0f,
         0.5f, -0.5f,  1.0f,
        // Face -X
        -0.5f,  0.5f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        -0.5f, -0.5f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        // Face -Y
        -0.5f, -0.5f,  1.0f,
        -1.0f, -1.0f, -1.0f,
         0.5f, -0.5f,  1.0f,
         1.0f, -1.0f, -1.0f,
        // Face -Z
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
    };


    const auto xpNorm = normalize(cross(vec3{  0.0f,  1.0f, 0.0f }, vec3{ -0.5f,  0.0f, 1.0f }));
    const auto xnNorm = normalize(cross(vec3{  0.0f, -1.0f, 0.0f }, vec3{  0.5f,  0.0f, 1.0f }));
    const auto ypNorm = normalize(cross(vec3{ -1.0f,  0.0f, 0.0f }, vec3{  0.0f, -0.5f, 1.0f }));
    const auto ynNorm = normalize(cross(vec3{  1.0f,  0.0f, 0.0f }, vec3{  0.0f,  0.5f, 1.0f }));
    const auto normals = std::vector{
        // Face +X
        xpNorm.x, xpNorm.y, xpNorm.z,
        xpNorm.x, xpNorm.y, xpNorm.z,
        xpNorm.x, xpNorm.y, xpNorm.z,
        xpNorm.x, xpNorm.y, xpNorm.z,
        // Face +Y
        ypNorm.x, ypNorm.y, ypNorm.z,
        ypNorm.x, ypNorm.y, ypNorm.z,
        ypNorm.x, ypNorm.y, ypNorm.z,
        ypNorm.x, ypNorm.y, ypNorm.z,
        // Face +Z
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
         0.0f,  0.0f,  1.0f,
        // Face -X
        xnNorm.x, xnNorm.y, xnNorm.z,
        xnNorm.x, xnNorm.y, xnNorm.z,
        xnNorm.x, xnNorm.y, xnNorm.z,
        xnNorm.x, xnNorm.y, xnNorm.z,
        // Face -Y
        ynNorm.x, ynNorm.y, ynNorm.z,
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
        // Face X+
        BLUE[0],     BLUE[1],     BLUE[2],     1.0f,
        BLACK[0],    BLACK[1],    BLACK[2],    1.0f,
        CYAN[0],     CYAN[1],     CYAN[2],     1.0f,
        GREEN[0],    GREEN[1],    GREEN[2],    1.0f,
        // Face Y+
        CYAN[0],     CYAN[1],     CYAN[2],	   1.0f,
        GREEN[0],    GREEN[1],    GREEN[2],	   1.0f,
        WHITE[0],    WHITE[1],    WHITE[2],	   1.0f,
        YELLOW[0],   YELLOW[1],   YELLOW[2],   1.0f,
        // Face Z+
        WHITE[0],	 WHITE[1],    WHITE[2],    1.0f,
        MAGENTA[0],  MAGENTA[1],  MAGENTA[2],  1.0f,
        CYAN[0],     CYAN[1],     CYAN[2],     1.0f,
        BLUE[0],     BLUE[1],     BLUE[2],     1.0f,
        // Face X-
        WHITE[0],    WHITE[1],    WHITE[2],    1.0f,
        YELLOW[0],   YELLOW[1],   YELLOW[2],   1.0f,
        MAGENTA[0],  MAGENTA[1],  MAGENTA[2],  1.0f,
        RED[0],      RED[1],      RED[2],      1.0f,
        // Face Y-
        MAGENTA[0],  MAGENTA[1],  MAGENTA[2],  1.0f,
        RED[0],      RED[1],      RED[2],      1.0f,
        BLUE[0],     BLUE[1],     BLUE[2],     1.0f,
        BLACK[0],    BLACK[1],    BLACK[2],    1.0f,
        // Face Z-
        GREEN[0],    GREEN[1],    GREEN[2],    1.0f,
        BLACK[0],    BLACK[1],    BLACK[2],    1.0f,
        YELLOW[0],   YELLOW[1],   YELLOW[2],   1.0f,
        RED[0],      RED[1],      RED[2],      1.0f,
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
		.attribute(0, VertexBuffer::VertexAttribute::POSITION,VertexBuffer::AttributeType::FLOAT3, 0, floatSize * 3)
		.attribute(1, VertexBuffer::VertexAttribute::COLOR,VertexBuffer::AttributeType::FLOAT4, 0, floatSize * 4)
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

	const auto shader = defaultShader(engine);
    const auto entity = EntityManager::get()->create();
    RenderableManager::Builder(1)
            .geometry(0, RenderableManager::PrimitiveType::TRIANGLES, *vertexBuffer, *indexBuffer,static_cast<int>(indices.size()), 0)
            .shader(0, shader)
            .build(entity);

	return std::unique_ptr<Drawable>(new Frustum(entity, shader));
}