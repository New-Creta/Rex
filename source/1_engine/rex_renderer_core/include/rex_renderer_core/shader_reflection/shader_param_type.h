#pragma once

namespace rex
{
	namespace gfx
	{
		enum class ShaderParameterType
		{
			Unknown,
			Uint,				// A single unsigned integer of 32 bits
			Uint2,			// 2 unsigned integers of 32 bits packed together
			Uint16_2,   // 2 unsigned integers of 16 bits packed together
			Float,			// A single float of 32 bits
			Float2,			// 2 floats of 32 bits packed together
			Float3,			// 3 floats of 32 bits packed together
			Float4			// 4 floats of 32 bits packed together
		};
	}
}