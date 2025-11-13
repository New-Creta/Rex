#include "rex_engine/gfx/rendering/camera_2d.h"

namespace rex
{
	namespace gfx
	{
		Camera2D::Camera2D(PixelCoord resolution, PixelCoord lookAhead)
			: m_resolution(resolution)
			, m_look_ahead(lookAhead)
			, m_pos()
		{}

		void Camera2D::set_pos(PixelCoord pos)
		{
			m_pos = pos + m_look_ahead;
		}

		PixelCoord Camera2D::center_pos() const
		{
			return m_pos;
		}
		PixelCoord Camera2D::top_left() const
		{
			return m_pos - (m_resolution / 2);
		}
	}
}