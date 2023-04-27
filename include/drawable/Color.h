// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#pragma once

#include <array>

namespace srgb {
	constexpr float BLACK	[]{ 0.0f, 0.0f, 0.0f };
	constexpr float RED     []{ 1.0f, 0.0f, 0.0f };
	constexpr float GREEN	[]{ 0.0f, 1.0f, 0.0f };
	constexpr float BLUE	[]{ 0.0f, 0.0f, 1.0f };
	constexpr float YELLOW	[]{ 1.0f, 0.96863f, 0.0f};
	constexpr float CYAN	[]{ 0.0f, 1.0f, 1.0f };
	constexpr float MAGENTA	[]{ 1.0f, 0.0f, 0.56471f };
	constexpr float PURPLE	[]{ 0.43529f, 0.05098f, 0.54118f };
	constexpr float BROWN	[]{ 0.54118f, 0.20392f, 0.04706f };
	constexpr float WHITE	[]{ 1.0f, 1.0f, 1.0f };

	std::array<float, 3> heatColorAt(float height);
	std::array<float, 3> hueAt(int ordinal);
}

// 0xAARRGGBB
namespace html {
	constexpr auto BLACK = 0xFF000000;
	constexpr auto WHITE = 0xFFFFFFFF;

	constexpr auto RED   = 0xFFFF0000;
	constexpr auto GREEN = 0xFF00FF00;
	constexpr auto BLUE  = 0xFF0000FF;

	constexpr auto YELLOW  = 0xFFFFF700;
	constexpr auto CYAN    = 0xFF00FFFF;
	constexpr auto MAGENTA = 0xFFFF0090;
	constexpr auto PURPLE  = 0xFF6F0D8A;
	constexpr auto BROWN   = 0xFF8A340C;
}
