// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <string_view>

#include "Engine.h"
#include "Texture.h"

Texture* loadTexture(std::string_view name, Engine& engine);
