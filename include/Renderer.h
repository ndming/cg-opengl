// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <array>
#include <glad/glad.h>

#include "Shader.h"
#include "View.h"

class Renderer {
public:
	~Renderer() = default;
	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) noexcept = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer& operator=(Renderer&&) = delete;

	struct ClearOptions {
		bool clear{ true };

		std::array<float, 4> clearColor{ 0.0f, 0.0f, 0.0f, 1.0f };
	};

	void setClearOptions(const ClearOptions& options);

	[[nodiscard]] ClearOptions getClearOptions() const;

	enum class PolygonMode {
		LINE = GL_LINE,
		FILL = GL_FILL
	};

	void togglePolygonMode();

	void render(const View& view) const;

    static void readFramebufferRgba(int x, int y, int width, int height, unsigned char* data);

private:
	Renderer() = default;

	ClearOptions _clearOptions{};

	PolygonMode _polygonMode{ PolygonMode::FILL };

    void preprocessRendering(const View& view) const;
    static void renderDirectionalLight(Entity light, Shader* shader, const glm::mat4& viewMat);
    static void renderPointLight(Entity light, Shader* shader, const glm::mat4& viewMat, int idx);

	friend class Engine;
};