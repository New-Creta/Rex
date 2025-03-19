#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "rex_engine/engine/types.h"
#include "rex_std/bonus/math/deg_angle.h"

// #TODO: Remaining cleanup of development/Pokemon -> main merge. ID: CAMERA

namespace rex
{
	namespace gfx
	{
		enum class ProjectionMode
		{
			Perspective,
			Ortographic
		};

		// A basic camera class
		class Camera
		{
		public:
			// When the arguments for near and far plane used to be called "near" and "far"
			// A compiler error occured where those paremters would not be used to initialize the members
			// Leaving the members to be zero initialized
			// Renaming them to "nearPlane" and "farPlane" fixes the issue
			Camera(const glm::vec3& pos, rsl::deg_angle fov, f32 width, f32 height, f32 nearPlan, f32 farPlane, ProjectionMode projectionMode = ProjectionMode::Perspective);

			// Return the position of the camera
			const glm::vec3& position() const;
			// Return the forward vector of the camera
			const glm::vec3& forward() const;

			// Return the view matrix of the camera
			glm::mat4 view_mat() const;
			// Return the projection matrix of the camera
			const glm::mat4& projection_mat() const;

			// Switch the projection mode of the camera
			void switch_mode(ProjectionMode newMode);

			// Return the default forward vector if the actual one cannot get calculated
			constexpr static glm::vec3 default_forward()
			{
				return s_default_forward;
			}

		private:
			// Calculate the projection matrix based on the current camera settings
			void calc_proj_matrix();

		private:
			glm::vec3 m_position;
			glm::vec3 m_forward;

			glm::mat4 m_view;
			glm::mat4 m_projection;

			f32 m_width;
			f32 m_height;
			f32 m_near;
			f32 m_far;
			rsl::deg_angle m_fov;
			ProjectionMode m_projection_mode;

			// By default, the forward vector is calculated as the vector from the position to the world's origin
			// However, if the position is the world origin, this vector is used as the forward vector
			constexpr static glm::vec3 s_default_forward = glm::vec3(0.0f, 0.0f, 1.0f);
		};
	}
}