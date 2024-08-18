#include "rex_renderer_core/rendering/camera.h"

namespace rex
{
	namespace gfx
	{
		Camera::Camera(const glm::vec3& pos, rsl::DegAngle fov, f32 width, f32 height, f32 near, f32 far, ProjectionMode projectionMode)
			: m_position(pos)
			, m_forward(glm::vec3() - m_position) // Always looking at center
			, m_view(glm::translate(glm::mat4(1.0f), m_position))
			, m_projection(1.0f)
			, m_projection_mode(projectionMode)
			, m_width(width)
			, m_height(height)
			, m_fov(fov)
			, m_near(near)
			, m_far(far)
		{
			calc_proj_matrix();
		}

		const glm::vec3& Camera::position() const
		{
			return m_position;
		}
		const glm::vec3& Camera::forward() const
		{
			return m_forward;
		}

		glm::mat4 Camera::view_mat() const
		{
			const glm::vec3 target = m_position + m_forward;
			const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

			return glm::lookAt(m_position, target, up);
		}
		const glm::mat4& Camera::projection_mat() const
		{
			return m_projection;
		}

		void Camera::switch_mode(ProjectionMode newMode)
		{
			if (newMode != m_projection_mode)
			{
				m_projection_mode = newMode;
				calc_proj_matrix();
			}
		}

		void Camera::calc_proj_matrix()
		{
			f32 fov_rad = glm::radians(m_fov.get());

			switch (m_projection_mode)
			{
			case ProjectionMode::Perspective:
				m_projection = glm::perspectiveFov(fov_rad, m_width, m_height, m_near, m_far);
				break;
			case ProjectionMode::Ortographic:
			{
				// Calculate the dimension of the near plane
				f32 h = glm::cos(0.5f * fov_rad) / glm::sin(0.5f * fov_rad);
				f32 w = 2 * h * m_height / m_width;
				f32 left = -w;
				f32 right = w;
				f32 top = -h;
				f32 bottom = h;

				m_projection = glm::ortho(left, right, top, bottom, m_near, m_far);
				break;
			}
			}
		}
	}
}