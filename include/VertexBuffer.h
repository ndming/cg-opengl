// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <glad/glad.h>
#include <set>
#include <vector>

class Engine;

class VertexBuffer {
public:
	~VertexBuffer() = default;
	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer(VertexBuffer&&) noexcept = delete;
	VertexBuffer& operator=(const VertexBuffer&) = delete;
	VertexBuffer& operator=(VertexBuffer&&) noexcept = delete;

	enum class VertexAttribute {
		POSITION = 0,
		NORMAL	 = 1,
		COLOR	 = 2,
		UV0		 = 3,
		UV1		 = 4,
	};

	enum class AttributeType {
		UBYTE4,
		FLOAT2,
		FLOAT3,
		FLOAT4,
		UINT,
	};

private:
	struct AttributeInfo {
		const VertexAttribute attr;
		const AttributeType type;
		const int byteOffset;
		const int byteStride;
	};

public:
	class Builder {
	public:
		explicit Builder(int bufferCount);

		Builder& vertexCount(int count);

		Builder& attribute(
			int index,
			VertexAttribute attr,
			AttributeType type,
			int byteOffset,
			int byteStride
		);

		Builder& normalized(VertexAttribute attr);

		VertexBuffer* build(Engine& engine);

	private:
		int _vertexCount{ 0 };
		
		std::vector<std::set<AttributeInfo>> _layout;

		std::set<VertexAttribute> _normAttrs{};
	};

	[[nodiscard]] GLuint getNativeObject() const;

	[[nodiscard]] int getVertexCount() const;

	[[nodiscard]] int getBufferCount() const;

	void setBufferAt(int index, const void* data) const;

	friend bool operator<(const AttributeInfo& lhs, const AttributeInfo& rhs) {
		return lhs.attr < rhs.attr;
	}

private:
	explicit VertexBuffer(
		GLuint* bufferObjects,
		int vertexCount,
		std::vector<std::set<AttributeInfo>>&& layout,
		std::set<VertexAttribute>&& normAttrs
	) noexcept;

	GLuint* const _bufferObjects;

	const int _vertexCount;

	const std::vector<std::set<AttributeInfo>> _layout;

	const std::set<VertexAttribute> _normAttrs;

	[[nodiscard]] int computeVertexByteSize(int bufferIndex) const;

	friend class Engine;
	friend class RenderableManager;
};