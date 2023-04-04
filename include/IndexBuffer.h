// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <glad/glad.h>

class Engine;

class IndexBuffer {
public:
	class Builder {
	public:
		enum class IndexType {
			UINT	= GL_UNSIGNED_INT,
			USHORT	= GL_UNSIGNED_SHORT,
		};

		Builder& indexCount(int count);

		Builder& indexType(IndexType type);

		IndexBuffer* build(Engine& engine) const;

	private:
		int _indexCount{ 0 };

		IndexType _indexType{ IndexType::UINT };
	};

	[[nodiscard]] GLuint getNativeObject() const;

	[[nodiscard]] Builder::IndexType getIndexType() const;

	void setBuffer(const unsigned int* buffer) const;

	void setBuffer(const unsigned short* buffer) const;

private:
	explicit IndexBuffer(const GLuint ibo, const int count, const Builder::IndexType type)
	: _ibo{ ibo }, _indexCount { count }, _indexType{ type } {}

	const GLuint _ibo;

	const int _indexCount;

	const Builder::IndexType _indexType;
};