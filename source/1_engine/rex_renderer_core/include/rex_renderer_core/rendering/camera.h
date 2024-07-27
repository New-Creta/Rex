#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "rex_engine/engine/types.h"
#include "rex_std/bonus/math/deg_angle.h"

namespace rex
{
	namespace gfx
	{
		class Camera
		{
		public:
			Camera(rsl::DegAngle fov, f32 width, f32 height, f32 near, f32 far);

			glm::vec3 position() const;
			glm::vec3 forward() const;
	
			glm::mat4 view_mat() const;
			glm::mat4 projection_mat() const;

		private:
			glm::vec3 m_position;
			glm::vec3 m_forward;

			glm::mat4 m_view;
			glm::mat4 m_projection;
		};
	}
}