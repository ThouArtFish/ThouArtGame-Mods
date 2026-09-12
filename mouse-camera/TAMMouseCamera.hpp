#pragma once

#include <ModBase.hpp>
#include <glm/glm.hpp>

/**
* Simple camera controlled by mouse movements
*/
class TAM::MouseCamera {
public:
	static inline constexpr float twpi = 6.28318530718f;

	float x_rotation = 0.0f; // Total rotation around x axis
	float y_rotation = 0.0f; // Total rotation around y axis
	float sens = 0.001f; // Camera speed factor
	float max_x = twpi / 3.98f; // Max rotation around x axis
	float max_y = -1.0f; // Max rotation around y axis (negative value means no cap)

	glm::vec3 mouseCallback(const float delta_x, const float delta_y) {
		if (max_x >= 0.0f) {
			x_rotation = glm::clamp(delta_y * sens + x_rotation, -max_x, max_x);
		}
		else {
			x_rotation += (x_rotation > twpi ? -twpi : (x_rotation < -twpi ? twpi : 0)) + delta_y * sens;
		}

		if (max_y >= 0.0f) {
			y_rotation = glm::clamp(delta_x * sens + y_rotation, -max_y, max_y);
		}
		else {
			y_rotation += (y_rotation > twpi ? -twpi : (y_rotation < -twpi ? twpi : 0)) + delta_x * sens;
		}

		return glm::normalize(glm::vec3(glm::cos(y_rotation) * glm::cos(x_rotation), glm::sin(x_rotation), glm::sin(y_rotation) * glm::cos(x_rotation)));
	}
};
