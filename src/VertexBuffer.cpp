// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <cstring>
#include <stdexcept>
#include <utility>
#include <ranges>

#include "VertexBuffer.h"
#include "Engine.h"

VertexBuffer::Builder::Builder(const int bufferCount) {
	_layout.reserve(bufferCount);
	_layout.resize(bufferCount);
}

VertexBuffer::Builder& VertexBuffer::Builder::vertexCount(const int count) {
	_vertexCount = count;
	return *this;
}

VertexBuffer::Builder& VertexBuffer::Builder::attribute(
	const int index,
	const VertexAttribute attr,
	const AttributeType type, 
	const int byteOffset, 
	const int byteStride
) {
	_layout[index].emplace(attr, type, byteOffset, byteStride);
	return *this;
}

VertexBuffer::Builder& VertexBuffer::Builder::normalized(const VertexAttribute attr) {
	_normAttrs.insert(attr);
	return *this;
}

VertexBuffer* VertexBuffer::Builder::build(Engine& engine) {
	const auto objects = new GLuint[_layout.size()];
	glGenBuffers(static_cast<GLsizei>(_layout.size()), objects);
    const auto buffer = new VertexBuffer(
            objects,
            _vertexCount,
            std::move(_layout),
            std::move(_normAttrs)
    );
	engine._vertexBuffers.insert(buffer);
	return buffer;
}

VertexBuffer::VertexBuffer(
	GLuint* const bufferObjects,
	const int vertexCount,
	std::vector<std::set<AttributeInfo>>&& layout,
	std::set<VertexAttribute>&& normAttrs
) noexcept : _bufferObjects{ bufferObjects }, _vertexCount{ vertexCount }, _layout{ layout }, _normAttrs{ normAttrs } {}

GLuint VertexBuffer::getNativeObject() const {
	return _bufferObjects[0];
}

int VertexBuffer::getVertexCount() const {
	return _vertexCount;
}

int VertexBuffer::getBufferCount() const {
	return static_cast<int>(_layout.size());
}

void VertexBuffer::setBufferAt(const int index, const void* const data) const {
	const auto vertexSize = computeVertexByteSize(index);

	glBindBuffer(GL_ARRAY_BUFFER, _bufferObjects[index]);

	// We transfer the data down the GPU by mean of std::mem copy.
	// Create a dedicated memory region in the GPU.
	glBufferStorage(
		GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexSize) * _vertexCount,
		nullptr, GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT
	);

	// Acquire the pointer to the newly created memory.
	const auto vertexData = glMapBufferRange(
		GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(vertexSize) * _vertexCount,
		GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT
	);
	if (!vertexData) {
		throw std::runtime_error("[VertexBuffer] \t- Could not map vertex buffer.");
	}

	// Transfer the vertex data.
	std::memcpy(vertexData, data, static_cast<size_t>(vertexSize) * _vertexCount);

	// Finally we notify the GPU the change.
	glFlushMappedBufferRange(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(vertexSize) * _vertexCount);
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int VertexBuffer::computeVertexByteSize(const int bufferIndex) const {
	auto byteSize = 0;
	for (const auto stride : _layout[bufferIndex] |
		std::views::transform([](const AttributeInfo& info) { return info.byteStride; })
	) {
		byteSize += stride;
	}
	return byteSize;
}
