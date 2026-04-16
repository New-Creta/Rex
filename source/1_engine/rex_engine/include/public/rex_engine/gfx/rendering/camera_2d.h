#pragma once

#include "rex_engine/math/coords.h"
#include "glm/vec2.hpp"

namespace rex
{
	namespace gfx
	{
		class Camera2D
		{
		public:
			Camera2D(PixelCoord resolution, PixelCoord lookAhead, glm::vec2 zoom);

			void set_pos(PixelCoord pos);

			PixelCoord center_pos() const;
			PixelCoord top_left() const;
			glm::vec2 zoom() const;

		private:
			// The resolution for the camera
			// this resolution doesn't have to match the render target
			// (eg. Pokemon resolution was 160x144 camera resolution)
			// even though it's rendered on a bigger render target
			PixelCoord m_resolution;

			// The amount of pixels the camera should be offsetted from the position given
			// An offset of { 1, 1 } will offset the camera 1 extra pixel for X and Y
			// when its position is updated through a "set_pos" operation
			PixelCoord m_look_ahead;
			
			// The zooming into the resolution
			// This value represents how big an old pixel is represented in the render target
			// A zoom value of {5, 5} means every pixel on the old gameboy is 5 pixels on the render target
			glm::vec2 m_zoom;

			// The center position of the camera
			PixelCoord m_pos;
		};
	}
}