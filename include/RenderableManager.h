// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <glad/glad.h>
#include <memory>
#include <vector>
#include <unordered_map>

#include "EntityManager.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "VertexBuffer.h"

class RenderableManager {
	struct Element {
		const GLuint vao;
		const GLenum topology;
		const std::size_t count;
		const int offset;
		const GLenum indexType;
	};

	struct Mesh {
		const std::vector<std::unique_ptr<Element>> elements;
		const std::vector<Shader*> shaders;
	};

public:
	~RenderableManager() = default;
	RenderableManager(const RenderableManager&) = delete;
	RenderableManager(RenderableManager&&) noexcept = delete;
	RenderableManager& operator=(const RenderableManager&) = delete;
	RenderableManager& operator=(RenderableManager&&) noexcept = delete;

	enum class PrimitiveType {
        LINE_STRIP = GL_LINE_STRIP,
		TRIANGLES = GL_TRIANGLES,
		TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
		TRIANGLE_FAN = GL_TRIANGLE_FAN
	};

	class Builder {
	public:
		explicit Builder(int count);

		Builder& shader(int index, Shader* shader);

		Builder& geometry(
			int index, 
			PrimitiveType topology,
			const VertexBuffer& vertices,
			const IndexBuffer& indices,
			int count, 
			int offset
		);

		void build(Entity entity);

	private:
		std::vector<std::unique_ptr<Element>> _elements;

		std::vector<Shader*> _shaders;

		static std::pair<int, int> resolveAttributeType(VertexBuffer::AttributeType type);

		static int resolveIndexSize(IndexBuffer::Builder::IndexType type);
	};

	[[nodiscard]] bool hasComponent(Entity entity) const;

private:
	RenderableManager() = default;

	inline static RenderableManager* _instance{ nullptr };

	static RenderableManager* getInstance();

	std::unordered_map<Entity, std::unique_ptr<Mesh>> _meshes{};

	friend class Engine;
	friend class Renderer;
};