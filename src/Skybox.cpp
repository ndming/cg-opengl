// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include "Skybox.h"
#include "Engine.h"

Skybox::Builder& Skybox::Builder::color(const float r, const float g, const float b, const float a) {
    _color[0] = r; _color[1] = g; _color[2] = b; _color[3] = a;
    return *this;
}

Skybox *Skybox::Builder::build(Engine &engine) {
    const auto skybox = new Skybox(_color);
    engine._skyboxes.insert(skybox);
    return skybox;
}

std::array<float, 4> Skybox::getColor() const {
    return _color;
}
