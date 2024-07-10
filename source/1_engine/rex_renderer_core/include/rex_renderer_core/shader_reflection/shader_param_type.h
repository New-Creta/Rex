#pragma once

namespace rex
{
	namespace gfx
	{
		enum class ShaderParamComponentType
		{
			Unknown,
			Uint,				// A single unsigned integer of 32 bits
			Sint,				// A single signed integer of 32 bits
			Float,			// A single float of 32 bits
		};

		enum class ShaderParameterType
		{
			Unknown,
			Uint,
			Uint2,
			Uint16_2,
			Float,
			Float2,
			Float3,
			Float4,
		};
	}
}