#pragma once

namespace rex
{
	namespace gfx
	{
		//enum class ShaderParamComponentType
		//{
		//	Unknown,
		//	Uint,				// A single unsigned integer of 32 bits
		//	Sint,				// A single signed integer of 32 bits
		//	Float,			// A single float of 32 bits
		//};

		// The following scalar types are supported, each of which are supported up to 4 components
		// - bool
		// - signed int (32 bits)
		// - unsigned int32 (32 bits)
		// - float (16 bits)
		// - float (32 bits)
		// - float (64 bits)
		enum class ShaderParameterType
		{
			Unknown,

			Bool,				// bool - 1 bit - 1 component

			Uint,				// unsigned integer - 32 bits - 1 component
			Uint2,			// unsigned integer - 32 bits - 2 component
			Uint3,			// unsigned integer - 32 bits - 3 component
			Uint4,			// unsigned integer - 32 bits - 4 component

			Ushort,				// unsigned integer - 16 bits - 1 component
			Ushort2,			// unsigned integer - 16 bits - 2 component
			Ushort3,			// unsigned integer - 16 bits - 3 component
			Ushort4,			// unsigned integer - 16 bits - 4 component

			UshortNorm,				// unsigned integer - 16 bits - 1 component
			Ushort2Norm,			// unsigned integer - 16 bits - 2 component
			Ushort3Norm,			// unsigned integer - 16 bits - 3 component
			Ushort4Norm,			// unsigned integer - 16 bits - 4 component

			UChar,		  // unsigned integer - 8 bits - 1 component
			UChar2,			// unsigned integer - 8 bits - 2 component
			UChar3,			// unsigned integer - 8 bits - 3 component
			UChar4,			// unsigned integer - 8 bits - 4 component

			UCharNorm,		  // unsigned integer - 8 bits - 1 component
			UChar2Norm,			// unsigned integer - 8 bits - 2 component
			UChar3Norm,			// unsigned integer - 8 bits - 3 component
			UChar4Norm,			// unsigned integer - 8 bits - 4 component

			Int,				// signed integer		- 32 bits - 1 component
			Int2,				// signed integer		- 32 bits - 2 component
			Int3,				// signed integer		- 32 bits - 3 component
			Int4,				// signed integer		- 32 bits - 4 component

			Short,			// unsigned integer - 16 bits - 1 component
			Short2,			// unsigned integer - 16 bits - 2 component
			Short3,			// unsigned integer - 16 bits - 3 component
			Short4,			// unsigned integer - 16 bits - 4 component

			ShortNorm,			// unsigned integer - 16 bits - 1 component
			Short2Norm,			// unsigned integer - 16 bits - 2 component
			Short3Norm,			// unsigned integer - 16 bits - 3 component
			Short4Norm,			// unsigned integer - 16 bits - 4 component

			Char,			  // unsigned integer - 8 bits - 1 component
			Char2,			// unsigned integer - 8 bits - 2 component
			Char3,			// unsigned integer - 8 bits - 3 component
			Char4,			// unsigned integer - 8 bits - 4 component

			CharNorm,			  // unsigned integer - 8 bits - 1 component
			Char2Norm,			// unsigned integer - 8 bits - 2 component
			Char3Norm,			// unsigned integer - 8 bits - 3 component
			Char4Norm,			// unsigned integer - 8 bits - 4 component

			Half,				// float						- 16 bits - 1 component
			Half2,			// float						- 16 bits - 2 component
			Half3,			// float						- 16 bits - 3 component
			Half4,			// float						- 16 bits - 4 component

			Float,			// float						- 32 bits - 1 component
			Float2,			// float						- 32 bits - 2 component
			Float3,			// float						- 32 bits - 3 component
			Float4,			// float						- 32 bits - 4 component

			Double,			// float						- 64 bits - 1 component
			Double2,		// float						- 64 bits - 2 component
			Double3,		// float						- 64 bits - 3 component
			Double4,		// float						- 64 bits - 4 component
		};

		enum class ScalarType
		{
			Unknown,
			Double,
			Uint,
			Int,
			Float,
			Ushort,
			Short,
			Half,
			Uchar,
			Char,
			Bool,
		};

		// Return the size in bytes of the scalar type
		constexpr s32 scalar_size(ScalarType type)
		{
			switch (type)
			{
			case rex::gfx::ScalarType::Double:	return 8;
			case rex::gfx::ScalarType::Uint:		return 4;
			case rex::gfx::ScalarType::Int:			return 4;
			case rex::gfx::ScalarType::Float:		return 4;
			case rex::gfx::ScalarType::Ushort:	return 2;
			case rex::gfx::ScalarType::Short:		return 2;
			case rex::gfx::ScalarType::Half:		return 2;
			case rex::gfx::ScalarType::Uchar:		return 1;
			case rex::gfx::ScalarType::Char:		return 1;
			case rex::gfx::ScalarType::Bool:		return 1;
			}

			return 0;
		}

		// Return the component count of a shader parameter type
		constexpr s32 component_count(ShaderParameterType type)
		{
			switch (type)
			{
			case rex::gfx::ShaderParameterType::Bool:
			case rex::gfx::ShaderParameterType::Uint:
			case rex::gfx::ShaderParameterType::Ushort:
			case rex::gfx::ShaderParameterType::UshortNorm:
			case rex::gfx::ShaderParameterType::UChar:
			case rex::gfx::ShaderParameterType::UCharNorm:
			case rex::gfx::ShaderParameterType::Int:
			case rex::gfx::ShaderParameterType::Short:
			case rex::gfx::ShaderParameterType::ShortNorm:
			case rex::gfx::ShaderParameterType::Char:
			case rex::gfx::ShaderParameterType::CharNorm:
			case rex::gfx::ShaderParameterType::Half:
			case rex::gfx::ShaderParameterType::Float:
			case rex::gfx::ShaderParameterType::Double:
				return 1;

			case rex::gfx::ShaderParameterType::Uint2:
			case rex::gfx::ShaderParameterType::Ushort2:
			case rex::gfx::ShaderParameterType::Ushort2Norm:
			case rex::gfx::ShaderParameterType::UChar2:
			case rex::gfx::ShaderParameterType::UChar2Norm:
			case rex::gfx::ShaderParameterType::Int2:
			case rex::gfx::ShaderParameterType::Short2:
			case rex::gfx::ShaderParameterType::Short2Norm:
			case rex::gfx::ShaderParameterType::Char2:
			case rex::gfx::ShaderParameterType::Char2Norm:
			case rex::gfx::ShaderParameterType::Half2:
			case rex::gfx::ShaderParameterType::Float2:
			case rex::gfx::ShaderParameterType::Double2:
				return 2;

			case rex::gfx::ShaderParameterType::Uint3:
			case rex::gfx::ShaderParameterType::Ushort3:
			case rex::gfx::ShaderParameterType::Ushort3Norm:
			case rex::gfx::ShaderParameterType::UChar3:
			case rex::gfx::ShaderParameterType::UChar3Norm:
			case rex::gfx::ShaderParameterType::Int3:
			case rex::gfx::ShaderParameterType::Short3:
			case rex::gfx::ShaderParameterType::Short3Norm:
			case rex::gfx::ShaderParameterType::Char3:
			case rex::gfx::ShaderParameterType::Char3Norm:
			case rex::gfx::ShaderParameterType::Half3:
			case rex::gfx::ShaderParameterType::Float3:
			case rex::gfx::ShaderParameterType::Double3:
				return 3;

			case rex::gfx::ShaderParameterType::Uint4:
			case rex::gfx::ShaderParameterType::Ushort4:
			case rex::gfx::ShaderParameterType::Ushort4Norm:
			case rex::gfx::ShaderParameterType::UChar4:
			case rex::gfx::ShaderParameterType::UChar4Norm:
			case rex::gfx::ShaderParameterType::Int4:
			case rex::gfx::ShaderParameterType::Short4:
			case rex::gfx::ShaderParameterType::Short4Norm:
			case rex::gfx::ShaderParameterType::Char4:
			case rex::gfx::ShaderParameterType::Char4Norm:
			case rex::gfx::ShaderParameterType::Half4:
			case rex::gfx::ShaderParameterType::Float4:
			case rex::gfx::ShaderParameterType::Double4:
				return 4;
			}

			return 0;
		}

		// Return the scalar type of a shader parameter type
		constexpr ScalarType scalar_type(ShaderParameterType type)
		{
			switch (type)
			{
			case rex::gfx::ShaderParameterType::Bool:							return ScalarType::Bool;

			case rex::gfx::ShaderParameterType::Uint:							return ScalarType::Uint;
			case rex::gfx::ShaderParameterType::Uint2:						return ScalarType::Uint;
			case rex::gfx::ShaderParameterType::Uint3:						return ScalarType::Uint;
			case rex::gfx::ShaderParameterType::Uint4:						return ScalarType::Uint;

			case rex::gfx::ShaderParameterType::Ushort:						return ScalarType::Ushort;
			case rex::gfx::ShaderParameterType::Ushort2:					return ScalarType::Ushort;
			case rex::gfx::ShaderParameterType::Ushort3:					return ScalarType::Ushort;
			case rex::gfx::ShaderParameterType::Ushort4:					return ScalarType::Ushort;
			case rex::gfx::ShaderParameterType::UshortNorm:				return ScalarType::Ushort;
			case rex::gfx::ShaderParameterType::Ushort2Norm:			return ScalarType::Ushort;
			case rex::gfx::ShaderParameterType::Ushort3Norm:			return ScalarType::Ushort;
			case rex::gfx::ShaderParameterType::Ushort4Norm:			return ScalarType::Ushort;

			case rex::gfx::ShaderParameterType::UChar:						return ScalarType::Uchar;
			case rex::gfx::ShaderParameterType::UChar2:						return ScalarType::Uchar;
			case rex::gfx::ShaderParameterType::UChar3:						return ScalarType::Uchar;
			case rex::gfx::ShaderParameterType::UChar4:						return ScalarType::Uchar;
			case rex::gfx::ShaderParameterType::UCharNorm:				return ScalarType::Uchar;
			case rex::gfx::ShaderParameterType::UChar2Norm:				return ScalarType::Uchar;
			case rex::gfx::ShaderParameterType::UChar3Norm:				return ScalarType::Uchar;
			case rex::gfx::ShaderParameterType::UChar4Norm:				return ScalarType::Uchar;

			case rex::gfx::ShaderParameterType::Int:							return ScalarType::Int;
			case rex::gfx::ShaderParameterType::Int2:							return ScalarType::Int;
			case rex::gfx::ShaderParameterType::Int3:							return ScalarType::Int;
			case rex::gfx::ShaderParameterType::Int4:							return ScalarType::Int;

			case rex::gfx::ShaderParameterType::Short:						return ScalarType::Short;
			case rex::gfx::ShaderParameterType::Short2:						return ScalarType::Short;
			case rex::gfx::ShaderParameterType::Short3:						return ScalarType::Short;
			case rex::gfx::ShaderParameterType::Short4:						return ScalarType::Short;
			case rex::gfx::ShaderParameterType::ShortNorm:				return ScalarType::Short;
			case rex::gfx::ShaderParameterType::Short2Norm:				return ScalarType::Short;
			case rex::gfx::ShaderParameterType::Short3Norm:				return ScalarType::Short;
			case rex::gfx::ShaderParameterType::Short4Norm:				return ScalarType::Short;

			case rex::gfx::ShaderParameterType::Char:							return ScalarType::Char;
			case rex::gfx::ShaderParameterType::Char2:						return ScalarType::Char;
			case rex::gfx::ShaderParameterType::Char3:						return ScalarType::Char;
			case rex::gfx::ShaderParameterType::Char4:						return ScalarType::Char;
			case rex::gfx::ShaderParameterType::CharNorm:					return ScalarType::Char;
			case rex::gfx::ShaderParameterType::Char2Norm:				return ScalarType::Char;
			case rex::gfx::ShaderParameterType::Char3Norm:				return ScalarType::Char;
			case rex::gfx::ShaderParameterType::Char4Norm:				return ScalarType::Char;

			case rex::gfx::ShaderParameterType::Half:							return ScalarType::Half;
			case rex::gfx::ShaderParameterType::Half2:						return ScalarType::Half;
			case rex::gfx::ShaderParameterType::Half3:						return ScalarType::Half;
			case rex::gfx::ShaderParameterType::Half4:						return ScalarType::Half;

			case rex::gfx::ShaderParameterType::Float:						return ScalarType::Float;
			case rex::gfx::ShaderParameterType::Float2:						return ScalarType::Float;
			case rex::gfx::ShaderParameterType::Float3:						return ScalarType::Float;
			case rex::gfx::ShaderParameterType::Float4:						return ScalarType::Float;

			case rex::gfx::ShaderParameterType::Double:						return ScalarType::Double;
			case rex::gfx::ShaderParameterType::Double2:					return ScalarType::Double;
			case rex::gfx::ShaderParameterType::Double3:					return ScalarType::Double;
			case rex::gfx::ShaderParameterType::Double4:					return ScalarType::Double;
			}

			return ScalarType::Unknown;
		}

		// Return byte size of a shader parameter type
		constexpr s32 format_byte_size(ShaderParameterType type)
		{
			return component_count(type) * scalar_size(scalar_type(type));
		}

		// Return if the shader parameter type is normalized or not
		constexpr bool is_normalized_format(ShaderParameterType type)
		{
			switch (type)
			{
			case rex::gfx::ShaderParameterType::UshortNorm:
			case rex::gfx::ShaderParameterType::Ushort2Norm:
			case rex::gfx::ShaderParameterType::Ushort3Norm:
			case rex::gfx::ShaderParameterType::Ushort4Norm:

			case rex::gfx::ShaderParameterType::UCharNorm:
			case rex::gfx::ShaderParameterType::UChar2Norm:
			case rex::gfx::ShaderParameterType::UChar3Norm:
			case rex::gfx::ShaderParameterType::UChar4Norm:

			case rex::gfx::ShaderParameterType::ShortNorm:
			case rex::gfx::ShaderParameterType::Short2Norm:
			case rex::gfx::ShaderParameterType::Short3Norm:
			case rex::gfx::ShaderParameterType::Short4Norm:

			case rex::gfx::ShaderParameterType::CharNorm:
			case rex::gfx::ShaderParameterType::Char2Norm:
			case rex::gfx::ShaderParameterType::Char3Norm:
			case rex::gfx::ShaderParameterType::Char4Norm:
				return true;
			}

			return false;
		}

		// Determine if the gpu will be able to convert one format to the other
		constexpr bool is_convertible_shader_param_type(ShaderParameterType from, ShaderParameterType to)
		{
			if (from == to)
			{
				return true;
			}

			if (is_normalized_format(from) ^ is_normalized_format(to))
			{
				return true;
			}

			if (component_count(from) <= component_count(to))
			{
				return scalar_type(from) == scalar_type(to);
			}

			return false;
		}
	}
}