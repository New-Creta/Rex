#pragma once

#include "rex_engine/math/coords.h"

namespace rex
{
	namespace gfx
	{
		class Camera2D
		{
		public:
			Camera2D(PixelCoord resolution, PixelCoord lookAhead);

			void set_pos(PixelCoord pos);

			PixelCoord center_pos() const;
			PixelCoord top_left() const;

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
			
			PixelCoord m_pos;
		};
	}
}