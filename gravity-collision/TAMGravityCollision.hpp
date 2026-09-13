#pragma once

#include <ModBase.hpp>
#include <UtilClass.hpp>
#include <BaseStateClass.hpp>
#include <WorldModelClass.hpp>
#include <glm/glm.hpp>

class TAM::GravityCollision {
public:
	static inline unsigned int MAX_ITERATIONS = 5;

	float grav_accel = 10.0f; // Acceleration of object under gravity
	float bounce_factor = 0.0f; // Factor of velocity perpendicular to surface returned after clipping into surface
	float friction = 0.5f; // Factor of weight used for friction
	float air_resistance = 0.0f; // Factor of velocity squared used as air resistance force
	glm::vec3 grav_dir = { 0.0f, -1.0f, 0.0f }; // Should always be normalized for accurate results

	/**
	* Calculate the new velocity of an object to avoid clipping with any given surfaces it has collided with.
	* Applies gravitational acceleration if enabled.
	*/
	glm::vec3 calculateVelocity(const std::vector<Collision::Info>& collisions, const glm::vec3& velocity) const {
		// Calculate velocity
		glm::vec3 final_vel = velocity;
		for (size_t i = 0; i < MAX_ITERATIONS; i++) {
			bool clipping = false;

			for (const auto& collision : collisions) {
				const glm::vec3& norm = collision.collision_normal;
				if (glm::dot(norm, final_vel) < 0.0f || glm::dot(norm, grav_dir) < 0.0f) {
					clipping = true;
				}
				else {
					continue;
				}

				const glm::vec3 perp_comp = TAGUtil::parallelCompNorm(norm, final_vel);
				final_vel = -perp_comp * bounce_factor + (final_vel - perp_comp);
			}

			if (!clipping) break;
		}

		// Calculate acceleration
		glm::vec3 final_accel = grav_dir * grav_accel;
		for (size_t i = 0; i < MAX_ITERATIONS; i++) {
			bool clipping = false;

			for (const auto& collision : collisions) {
				const glm::vec3& norm = collision.collision_normal;
				if (glm::dot(norm, final_accel) < 0.0f) {
					clipping = true;
				}
				else {
					continue;
				}

				const glm::vec3 perp_comp = TAGUtil::parallelCompNorm(norm, final_accel);
				const glm::vec3 para_comp = final_accel - perp_comp;
				float length = glm::length(para_comp);
				if (length > 0.0001f) {
					const float max_resist = friction * glm::length(perp_comp);
					final_accel = para_comp * (max_resist >= length ? 0.0f : 1.0f - (max_resist / length));
				}
				length = glm::length(final_vel);
				if (length > 0.0001f) {
					final_accel = (final_vel / length) * grav_accel * friction * 0.02f;
				}
			}

			if (!clipping) break;
		}

		// Air resistance is also applied in the final formula
		return final_vel + (final_accel - final_vel * air_resistance * glm::length(final_vel) * 0.5f) * (float)TAGBaseState::delta_time;
	};
};
