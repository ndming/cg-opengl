// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include "View.h"

Camera* View::getCamera() const {
	return _camera;
}

void View::setCamera(Camera* const camera) {
	_camera = camera;
}

Scene* View::getScene() const {
	return _scene;
}

void View::setScene(Scene* const scene) {
	_scene = scene;
}

Viewport View::getViewport() const {
	return _viewport;
}

void View::setViewport(const Viewport& viewport) {
	_viewport = viewport;
}

void View::setViewport(Viewport&& viewport) noexcept {
	_viewport = viewport;
}

void View::setSkybox(Skybox* const skybox) {
    _skybox = skybox;
}

Skybox *View::getSkybox() const {
    return _skybox;
}
