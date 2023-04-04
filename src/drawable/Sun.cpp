// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include "Sun.h"

Sun::Builder& Sun::Builder::color(const float r, const float g, const float b) {
	uniformColor(r, g, b);
	return *this;
}

std::unique_ptr<Drawable> Sun::Builder::build(Engine& engine) {
	shaderModel(Shader::Model::UNLIT);
	recursiveDepth(RECURSIVE_DEPTH);
	initialPolygon(Polyhedron::ICOSAHEDRON);
	radius(SUN_RADIUS);

	return SubdivisionBuilder::build(engine);
}
