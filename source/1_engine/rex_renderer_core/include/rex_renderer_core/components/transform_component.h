#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace rex
{
	namespace gfx
	{
		struct TransformComponent
		{
			glm::vec3 translation;
			glm::vec3 rotation;
			glm::vec3 scale;

			glm::mat4 transform()
			{
				glm::mat4 rotation_mat = glm::toMat4(glm::quat(rotation));

				return glm::translate(glm::mat4(1.0f), translation) * rotation_mat * glm::scale(glm::mat4(1.0f), scale);
			}
		};
	}
}