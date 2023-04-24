// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <numbers>
#include <ranges>

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "RenderableManager.h"

#include "drawable/Orbit.h"

std::unique_ptr<Drawable> Orbit::Builder::build(Engine &engine) {
    auto positions = std::vector<float>{};
    auto colors = std::vector<float>{};

    const auto angleStep = 2.0f * std::numbers::pi_v<float> / static_cast<float>(_segments);
    for (auto i = 0; i < _segments; ++i) {
        const auto angle = static_cast<float>(i) * angleStep;
        positions.push_back(_orbitX(angle)); positions.push_back(_orbitY(angle)); positions.push_back(0.0f);
        colors.push_back(_color.r); colors.push_back(_color.g); colors.push_back(_color.b); colors.push_back(1.0f);
    }

    const auto vertexCount = static_cast<int>(positions.size() / 3);
    auto indices = std::vector<unsigned>(vertexCount);
    auto iotaView = std::ranges::iota_view(0);
    std::ranges::copy(iotaView.begin(), iotaView.begin() + vertexCount, indices.begin());
    indices.push_back(0u);

    constexpr auto floatSize = 4;
    const auto vertexBuffer = VertexBuffer::Builder(2)
            .vertexCount(vertexCount)
            .attribute(0, VertexBuffer::VertexAttribute::POSITION,VertexBuffer::AttributeType::FLOAT3, 0, floatSize * 3)
            .attribute(1, VertexBuffer::VertexAttribute::COLOR,VertexBuffer::AttributeType::FLOAT4, 0, floatSize * 4)
            .build(engine);
    vertexBuffer->setBufferAt(0, positions.data());
    vertexBuffer->setBufferAt(1, colors.data());

    const auto indexBuffer = IndexBuffer::Builder()
            .indexCount(static_cast<int>(indices.size()))
            .indexType(IndexBuffer::Builder::IndexType::UINT)
            .build(engine);
    indexBuffer->setBuffer(indices.data());

    shaderModel(Shader::Model::UNLIT);
    const auto shader = Shader::Builder(_shaderModel).build(engine);

    const auto entity = EntityManager::get()->create();
    RenderableManager::Builder(1)
            .geometry(0, RenderableManager::PrimitiveType::LINE_STRIP, *vertexBuffer, *indexBuffer,static_cast<int>(indices.size()), 0)
            .shader(0, shader)
            .build(entity);

    return std::unique_ptr<Drawable>(new Orbit(entity, shader));
}
