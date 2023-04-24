// Copyright (c) 2023. Minh Nguyen
// All rights reserved.

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

#include "drawable/Color.h"

namespace srgb {
	static const auto hot = glm::vec3{ RED[0], RED[1], RED[2] };
	static const auto warm = glm::vec3{ YELLOW[0], YELLOW[1], YELLOW[2] };
	static const auto neutral = glm::vec3{ GREEN[0], GREEN[1], GREEN[2] };
	static const auto cool = glm::vec3{ CYAN[0], CYAN[1], CYAN[2] };
	static const auto cold = glm::vec3{ BLUE[0], BLUE[1], BLUE[2] };

	std::array<float, 3> heatColorAt(const float height) {
		const auto factor = std::clamp(height, -1.0f, 1.0f);
		glm::vec3 color;

		if (factor < 0.0f) {
			if (factor >= -0.5f) {
				color = mix(neutral, cool, glm::abs(factor) * 2.0f);
			}
			else {
				color = mix(cool, cold, glm::abs(factor + 0.5f) * 2.0f);
			}
		}
		else {
			if (factor < 0.5f) {
				color = mix(neutral, warm, factor * 2.0f);
			}
			else {
				color = mix(warm, hot, (factor - 0.5f) * 2.0f);
			}
		}

		return { color.r, color.g, color.b };
	}

	static constexpr auto HUE_COUNT = 8;
	std::array<float, 3> hueAt(const int ordinal) {
		switch (ordinal % HUE_COUNT) {
		case 0:
			return std::array{ RED[0], RED[1], RED[2] };
		case 1:
			return std::array{ GREEN[0], GREEN[1], GREEN[2] };
		case 2:
			return std::array{ BLUE[0], BLUE[1], BLUE[2] };
		case 3:
			return std::array{ YELLOW[0], YELLOW[1], YELLOW[2] };
		case 4:
			return std::array{ CYAN[0], CYAN[1], CYAN[2] };
		case 5:
			return std::array{ MAGENTA[0], MAGENTA[1], MAGENTA[2] };
		case 6:
			return std::array{ PURPLE[0], PURPLE[1], PURPLE[2] };
		default:
			return std::array{ BROWN[0], BROWN[1], BROWN[2] };
		}
	}

}