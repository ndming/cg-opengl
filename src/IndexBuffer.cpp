// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <glad/glad.h>
#include <variant>

#include "IndexBuffer.h"
#include "Engine.h"

IndexBuffer::Builder& IndexBuffer::Builder::indexCount(const int count) {
	_indexCount = count;
	return *this;
}

IndexBuffer::Builder& IndexBuffer::Builder::indexType(const IndexType type) {
	_indexType = type;
	return *this;
}

IndexBuffer* IndexBuffer::Builder::build(Engine& engine) const {
	GLuint ibo;
	glGenBuffers(1, &ibo);
	const auto buffer = new IndexBuffer(ibo, _indexCount, _indexType);
	engine._indexBuffers.insert(buffer);
	return buffer;
}

GLuint IndexBuffer::getNativeObject() const {
	return _ibo;
}

IndexBuffer::Builder::IndexType IndexBuffer::getIndexType() const {
	return _indexType;
}

void IndexBuffer::setBuffer(const unsigned int* buffer) const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(4) * _indexCount,
		buffer, GL_STATIC_DRAW
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::setBuffer(const unsigned short* buffer) const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(2) * _indexCount,
		buffer, GL_STATIC_DRAW
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
