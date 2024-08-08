#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "rex_engine/engine/types.h"
#include "rex_std/bonus/math/deg_angle.h"

namespace rex
{
	namespace gfx
	{
		enum class ProjectionMode
		{
			Perspective,
			Ortographic
		}

		// A basic camera class
		class Camera
		{
		public:
			Camera(rsl::DegAngle fov, f32 width, f32 height, f32 near, f32 far, ProjectionMode projectionMode = ProjectionMode::Perspective);

			// Return the position of the camera
			const glm::vec3& position() const;
			// Return the forward vector of the camera
			const glm::vec3& forward() const;

			// Return the view matrix of the camera
			const glm::mat4& view_mat() const;
			// Return the projection matrix of the camera
			const glm::mat4& projection_mat() const;

		private:
			glm::vec3 m_position;
			glm::vec3 m_forward;

			glm::mat4 m_view;
			glm::mat4 m_projection;

			ProjectionMode m_projection_mode;
		};
	}
}