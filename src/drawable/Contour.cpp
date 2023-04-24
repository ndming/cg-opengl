// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "RenderableManager.h"

#include "drawable/Contour.h"
#include "drawable/Color.h"

Contour::Builder &Contour::Builder::low(const float lo) {
    _lo = lo;
    return *this;
}

Contour::Builder &Contour::Builder::high(const float hi) {
    _hi = hi;
    return *this;
}

std::unique_ptr<Drawable> Contour::Builder::build(Engine &engine) {
    auto positions = std::vector<float>{};
    auto colors = std::vector<float>{};

    const auto xStep = _halfExtentX * 2 / static_cast<float>(_segmentsX);
    const auto yStep = _halfExtentY * 2 / static_cast<float>(_segmentsY);

    for (auto i = 0; i < _segmentsX + 1; ++i) {
        for (auto j = 0; j < _segmentsY + 1; ++j) {
            // Acquire the x, y coordinate
            const auto x0 = static_cast<float>(i) * xStep - _halfExtentX;	// from top left
            const auto y0 = _halfExtentY - static_cast<float>(j) * yStep;	// to bottom right
            const auto z0 = _func(x0, y0);

            positions.push_back(x0); positions.push_back(y0); positions.push_back(0.0f);

            const auto rgb = srgb::heatColorAt(mapHeat(z0));
            colors.push_back(rgb[0]); colors.push_back(rgb[1]);
            colors.push_back(rgb[2]); colors.push_back(1.0f);
        }
    }

    constexpr auto floatSize = 4;
    const auto vertexBuffer = VertexBuffer::Builder(2)
            .vertexCount(static_cast<int>(positions.size() / 3))
            .attribute(0, VertexBuffer::VertexAttribute::POSITION, VertexBuffer::AttributeType::FLOAT3, 0, floatSize * 3)
            .attribute(1, VertexBuffer::VertexAttribute::COLOR, VertexBuffer::AttributeType::FLOAT4, 0, floatSize * 4)
            .build(engine);
    vertexBuffer->setBufferAt(0, positions.data());
    vertexBuffer->setBufferAt(1, colors.data());

    shaderModel(Shader::Model::UNLIT);
    const auto shader = Shader::Builder(_shaderModel).build(engine);

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

        renderableBuilder.geometry(
                i, RenderableManager::PrimitiveType::TRIANGLE_STRIP, *vertexBuffer, *indexBuffer,
                static_cast<int>(indices.size()), 0
        ).shader(i, shader);
    }
    renderableBuilder.build(entity);

    return std::unique_ptr<Drawable>(new Contour(entity, shader));
}

float Contour::Builder::mapHeat(const float z) const {
    return 2 * (z - _lo) / (_hi - _lo) - 1.0f;
}
