// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <array>

#include "Camera.h"
#include "Scene.h"
#include "Skybox.h"

using Viewport = std::array<int, 4>;

class View {
public:
	~View() = default;
	View(const View&) = delete;
	View(View&&) noexcept = delete;
	View& operator=(const View&) = delete;
	View& operator=(View&&) = delete;

	[[nodiscard]] Camera* getCamera() const;

	void setCamera(Camera* camera);

	[[nodiscard]] Scene* getScene() const;

	void setScene(Scene* scene);

	[[nodiscard]] Viewport getViewport() const;

	void setViewport(const Viewport& viewport);

	void setViewport(Viewport&& viewport) noexcept;

    void setSkybox(Skybox* skybox);

    [[nodiscard]] Skybox* getSkybox() const;

private:
	View() = default;

	Viewport _viewport{ 0, 0, 0, 0 };

	Camera* _camera{ nullptr };

	Scene* _scene{ nullptr };

    Skybox* _skybox{ nullptr };

	friend class Engine;
};