// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <cmath>
#include <numbers>
#include <ranges>

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RenderableManager.h"

#include "drawable/Color.h"
#include "drawable/Ring.h"

using namespace srgb;
using namespace std;

std::unique_ptr<Drawable> Ring::Builder::build(Engine &engine) {
    auto positions = vector<float>{};
    auto normals = vector<float>{};
    auto colors = vector<float>{};
    auto texCoords = vector<float>{};

    const auto angleStep = 2.0f * numbers::pi_v<float> / static_cast<float>(_segments);
    const auto textureStep = 1.0f / static_cast<float>(_segments);
    for (auto i = 0; i <= _segments; ++i) {
        const auto angle = static_cast<float>(i) * angleStep;
        const auto v = static_cast<float>(i) * textureStep;

        const auto innerPointX = cos(angle) * _radius;
        const auto innerPointY = sin(angle) * _radius;
        positions.push_back(innerPointX); positions.push_back(innerPointY); positions.push_back(0.0f);
        normals.push_back(0.0f); normals.push_back(0.0f); normals.push_back(1.0f);
        colors.push_back(YELLOW[0]); colors.push_back(YELLOW[1]); colors.push_back(YELLOW[2]); colors.push_back(1.0f);
        texCoords.push_back(1.0f); texCoords.push_back(v);

        const auto outerPointX = cos(angle) * (_radius + _thickness);
        const auto outerPointY = sin(angle) * (_radius + _thickness);
        positions.push_back(outerPointX); positions.push_back(outerPointY); positions.push_back(0.0f);
        normals.push_back(0.0f); normals.push_back(0.0f); normals.push_back(1.0f);
        colors.push_back(BROWN[0]); colors.push_back(BROWN[1]); colors.push_back(BROWN[2]); colors.push_back(1.0f);
        texCoords.push_back(0.0f); texCoords.push_back(v);
    }

    const auto vertexCount = static_cast<int>(positions.size() / 3);
    auto indices = vector<unsigned>(vertexCount);
    auto iotaView = ranges::iota_view(0);
    ranges::copy(iotaView.begin(), iotaView.begin() + vertexCount, indices.begin());

    constexpr auto floatSize = 4;
    const auto vertexBuffer = VertexBuffer::Builder(4)
            .vertexCount(vertexCount)
            .attribute(0, VertexBuffer::VertexAttribute::POSITION,VertexBuffer::AttributeType::FLOAT3, 0, floatSize * 3)
            .attribute(1, VertexBuffer::VertexAttribute::NORMAL,VertexBuffer::AttributeType::FLOAT3, 0, floatSize * 3)
            .attribute(2, VertexBuffer::VertexAttribute::COLOR,VertexBuffer::AttributeType::FLOAT4, 0, floatSize * 4)
            .attribute(3, VertexBuffer::VertexAttribute::UV0,VertexBuffer::AttributeType::FLOAT2, 0, floatSize * 2)
            .build(engine);
    vertexBuffer->setBufferAt(0, positions.data());
    vertexBuffer->setBufferAt(1, normals.data());
    vertexBuffer->setBufferAt(2, colors.data());
    vertexBuffer->setBufferAt(3, texCoords.data());

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

    return std::unique_ptr<Drawable>(new Ring(entity, shader));
}