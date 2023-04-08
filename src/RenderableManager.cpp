// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <glad/glad.h>
#include <utility>

#include "RenderableManager.h"

RenderableManager::Builder::Builder(const int count) {
	_elements.reserve(count);
	_elements.resize(count);
	_shaders.reserve(count);
	_shaders.resize(count);
}

RenderableManager::Builder& RenderableManager::Builder::shader(const int index, Shader* const shader) {
	_shaders[index] = shader;
	return *this;
}

RenderableManager::Builder& RenderableManager::Builder::geometry(
	const int index, 
	const PrimitiveType topology,
	const VertexBuffer& vertices,
	const IndexBuffer& indices,
	const int count, 
	const int offset
) {
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Configure vertex attributes
	for (auto i = 0; i < vertices.getBufferCount(); ++i) {
		glBindBuffer(GL_ARRAY_BUFFER, vertices._bufferObjects[i]);

		for (const auto& [attr, type, byteOffset, byteStride] : vertices._layout[i]) {
			const auto& [glType, components] = resolveAttributeType(type);

			glVertexAttribPointer(
				static_cast<GLuint>(attr), components, glType, vertices._normAttrs.contains(attr), byteStride, 
				reinterpret_cast<void*>(static_cast<uint64_t>(byteOffset))	// NOLINT(performance-no-int-to-ptr)
			);
			glEnableVertexAttribArray(static_cast<GLuint>(attr));
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	// Configure indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices.getNativeObject());

	// Done setting up VAO
	glBindVertexArray(0);

	const auto indexByteOffset = offset * resolveIndexSize(indices.getIndexType());
	_elements[index] = std::make_unique<Element>(
		vao, static_cast<GLenum>(topology), count, indexByteOffset,
		static_cast<GLenum>(indices.getIndexType())
	);
	
	return *this;
}

std::pair<int, int> RenderableManager::Builder::resolveAttributeType(const VertexBuffer::AttributeType type) {
	switch (type) {
	case VertexBuffer::AttributeType::UBYTE4:
		return std::make_pair(GL_UNSIGNED_BYTE, 4);
	case VertexBuffer::AttributeType::FLOAT2:
		return std::make_pair(GL_FLOAT, 2);
	case VertexBuffer::AttributeType::FLOAT3: 
		return std::make_pair(GL_FLOAT, 3);
	case VertexBuffer::AttributeType::FLOAT4:
		return std::make_pair(GL_FLOAT, 4);
	case VertexBuffer::AttributeType::UINT:
		return std::make_pair(GL_UNSIGNED_INT, 1);
	}
	return {};
}

int RenderableManager::Builder::resolveIndexSize(const IndexBuffer::Builder::IndexType type) {
	switch (type) {
	case IndexBuffer::Builder::IndexType::UINT:
		return sizeof(GLuint);
	case IndexBuffer::Builder::IndexType::USHORT: 
		return sizeof(GLushort);
	}
	return 0;
}

void RenderableManager::Builder::build(const Entity entity) {
	const auto renderableManager = getInstance();
	renderableManager->_meshes[entity] = std::make_unique<Mesh>(std::move(_elements), std::move(_shaders));

	// Record this entity as renderable component
	const auto entityManager = EntityManager::get();
	entityManager->_entities[entity] = EntityManager::Component::RENDERABLE;
}

RenderableManager* RenderableManager::getInstance() {
	if (!_instance) {
		_instance = new RenderableManager();
	}
	return _instance;
}

bool RenderableManager::hasComponent(const Entity entity) const {
	return _meshes.contains(entity);
}




