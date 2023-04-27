// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <glm/geometric.hpp>
#include <vector>

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RenderableManager.h"

#include "drawable/Trace.h"

Trace::Builder &Trace::Builder::position(const float x, const float y, const float z) {
    _position.x = x; _position.y = y; _position.z = z;
    return *this;
}

Trace::Builder &Trace::Builder::direction(const glm::vec3 &direction) {
    _direction = direction;
    return *this;
}

Trace::Builder &Trace::Builder::normal(const glm::vec3 &normal) {
    _normal = normal;
    return *this;
}

Trace::Builder &Trace::Builder::color(const float r, const float g, const float b) {
    _color.r = r; _color.b = b; _color.g = g;
    return *this;
}

Trace::Builder &Trace::Builder::size(const float size) {
    _size = size;
    return *this;
}

std::unique_ptr<Drawable> Trace::Builder::build(Engine &engine) {
    const auto sideDir = glm::normalize(glm::cross(_direction, _normal));
    const auto pM = _position + (_size / 2.0f) * sideDir;
    const auto cornDir = glm::normalize(glm::cross(pM - _position, _normal));

    const auto p0 = pM + (_size / 2.0f) *  cornDir;
    const auto p1 = pM + (_size / 2.0f) * -cornDir;
    const auto p2 = p0 + _size * -sideDir;
    const auto p3 = p1 + _size * -sideDir;

    const auto positions = std::vector{
        p0.x, p0.y, p0.z,
        p1.x, p1.y, p1.z,
        p2.x, p2.y, p2.z,
        p3.x, p3.y, p3.z,
    };

    const auto colors = std::vector{
        _color.r, _color.g, _color.b, 1.0f,
        _color.r, _color.g, _color.b, 1.0f,
        _color.r, _color.g, _color.b, 1.0f,
        _color.r, _color.g, _color.b, 1.0f,
    };

    const auto normal = glm::normalize(_normal);
    const auto normals = std::vector{
        normal.x, normal.y, normal.z,
        normal.x, normal.y, normal.z,
        normal.x, normal.y, normal.z,
        normal.x, normal.y, normal.z,
    };

    const auto indices = std::vector{ 0u, 1u, 2u, 3u };

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

    const auto shader = defaultShader(engine);
    const auto entity = EntityManager::get()->create();
    RenderableManager::Builder(1)
        .geometry(0, RenderableManager::PrimitiveType::TRIANGLE_STRIP, *vertexBuffer, *indexBuffer,static_cast<int>(indices.size()), 0)
        .shader(0, shader)
        .build(entity);

    return std::unique_ptr<Drawable>(new Trace(entity, shader));
}


