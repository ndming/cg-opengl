// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include "Drawable.h"

Entity Drawable::getEntity() const {
	return _entity;
}

Shader* Drawable::getShader() const {
	return _shader;
}

Drawable::Builder& Drawable::Builder::shaderModel(const Shader::Model model) {
	_shaderModel = model;
	return *this;
}
