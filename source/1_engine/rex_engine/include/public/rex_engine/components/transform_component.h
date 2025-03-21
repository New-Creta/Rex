#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "rex_std/bonus/math/deg_angle.h"
#include "rex_std/bonus/math/rad_angle.h"

// #TODO: Remaining cleanup of development/Pokemon -> main merge. ID: ECS
namespace rex
{
	namespace gfx
	{
		// The component holding the translation, rotation and scale of an object
		// This is added to every object by default
		struct TransformComponent
		{
			glm::vec3 translation{};							// Where in the world is it located
			glm::vec3 rotation_euler{};						// What's the rotation of the object
			glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);	// What's the scale of the object
			
			// Convert the transform of an object to a world matrix
			glm::mat4 world_mat() const
			{
				glm::mat4 rotation_mat = glm::toMat4(glm::quat(glm::radians(rotation_euler)));

				glm::mat4 world = glm::translate(glm::mat4(1.0f), translation) * rotation_mat * glm::scale(glm::mat4(1.0f), scale);
				return glm::transpose(world);
			}

			TransformComponent& rotate_x(rsl::deg_angle angle)
			{
				rotation_euler.x += angle.get();
				return *this;
			}
			TransformComponent& rotate_x(rsl::rad_angle angle)
			{
				rotation_euler.x += angle.to_deg();
				return *this;
			}

			TransformComponent& rotate_y(rsl::deg_angle angle)
			{
				rotation_euler.y += angle.get();
				return *this;
			}
			TransformComponent& rotate_y(rsl::rad_angle angle)
			{
				rotation_euler.y += angle.to_deg();
				return *this;
			}

			TransformComponent& rotate_z(rsl::deg_angle angle)
			{
				rotation_euler.z += angle.get();
				return *this;
			}
			TransformComponent& rotate_z(rsl::rad_angle angle)
			{
				rotation_euler.z += angle.to_deg();
				return *this;
			}
		};
	}
}