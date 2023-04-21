// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include "drawable/Aura.h"

Aura::Builder& Aura::Builder::color(const float r, const float g, const float b) {
	uniformColor(r, g, b);
	return *this;
}

std::unique_ptr<Drawable> Aura::Builder::build(Engine& engine) {
	shaderModel(Shader::Model::UNLIT);
	recursiveDepth(RECURSIVE_DEPTH);
	initialPolygon(Polyhedron::ICOSAHEDRON);
	radius(AURA_RADIUS);

	return SubdivisionBuilder::build(engine);
}
