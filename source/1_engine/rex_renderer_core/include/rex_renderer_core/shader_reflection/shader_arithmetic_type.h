#pragma once

namespace rex
{
	namespace gfx
	{
		// The following scalar types are supported, each of which are supported up to 4 components
		// - bool
		// - signed int (32 bits)
		// - unsigned int32 (32 bits)
		// - float (16 bits)
		// - float (32 bits)
		// - float (64 bits)
		enum class ShaderArithmeticType
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
			default: break;
			}

			return 0;
		}

		// Return the component count of a shader parameter type
		constexpr s32 component_count(ShaderArithmeticType type)
		{
			switch (type)
			{
			case rex::gfx::ShaderArithmeticType::Bool:
			case rex::gfx::ShaderArithmeticType::Uint:
			case rex::gfx::ShaderArithmeticType::Ushort:
			case rex::gfx::ShaderArithmeticType::UshortNorm:
			case rex::gfx::ShaderArithmeticType::UChar:
			case rex::gfx::ShaderArithmeticType::UCharNorm:
			case rex::gfx::ShaderArithmeticType::Int:
			case rex::gfx::ShaderArithmeticType::Short:
			case rex::gfx::ShaderArithmeticType::ShortNorm:
			case rex::gfx::ShaderArithmeticType::Char:
			case rex::gfx::ShaderArithmeticType::CharNorm:
			case rex::gfx::ShaderArithmeticType::Half:
			case rex::gfx::ShaderArithmeticType::Float:
			case rex::gfx::ShaderArithmeticType::Double:
				return 1;

			case rex::gfx::ShaderArithmeticType::Uint2:
			case rex::gfx::ShaderArithmeticType::Ushort2:
			case rex::gfx::ShaderArithmeticType::Ushort2Norm:
			case rex::gfx::ShaderArithmeticType::UChar2:
			case rex::gfx::ShaderArithmeticType::UChar2Norm:
			case rex::gfx::ShaderArithmeticType::Int2:
			case rex::gfx::ShaderArithmeticType::Short2:
			case rex::gfx::ShaderArithmeticType::Short2Norm:
			case rex::gfx::ShaderArithmeticType::Char2:
			case rex::gfx::ShaderArithmeticType::Char2Norm:
			case rex::gfx::ShaderArithmeticType::Half2:
			case rex::gfx::ShaderArithmeticType::Float2:
			case rex::gfx::ShaderArithmeticType::Double2:
				return 2;

			case rex::gfx::ShaderArithmeticType::Uint3:
			case rex::gfx::ShaderArithmeticType::Ushort3:
			case rex::gfx::ShaderArithmeticType::Ushort3Norm:
			case rex::gfx::ShaderArithmeticType::UChar3:
			case rex::gfx::ShaderArithmeticType::UChar3Norm:
			case rex::gfx::ShaderArithmeticType::Int3:
			case rex::gfx::ShaderArithmeticType::Short3:
			case rex::gfx::ShaderArithmeticType::Short3Norm:
			case rex::gfx::ShaderArithmeticType::Char3:
			case rex::gfx::ShaderArithmeticType::Char3Norm:
			case rex::gfx::ShaderArithmeticType::Half3:
			case rex::gfx::ShaderArithmeticType::Float3:
			case rex::gfx::ShaderArithmeticType::Double3:
				return 3;

			case rex::gfx::ShaderArithmeticType::Uint4:
			case rex::gfx::ShaderArithmeticType::Ushort4:
			case rex::gfx::ShaderArithmeticType::Ushort4Norm:
			case rex::gfx::ShaderArithmeticType::UChar4:
			case rex::gfx::ShaderArithmeticType::UChar4Norm:
			case rex::gfx::ShaderArithmeticType::Int4:
			case rex::gfx::ShaderArithmeticType::Short4:
			case rex::gfx::ShaderArithmeticType::Short4Norm:
			case rex::gfx::ShaderArithmeticType::Char4:
			case rex::gfx::ShaderArithmeticType::Char4Norm:
			case rex::gfx::ShaderArithmeticType::Half4:
			case rex::gfx::ShaderArithmeticType::Float4:
			case rex::gfx::ShaderArithmeticType::Double4:
				return 4;

			default: break;

			}

			return 0;
		}

		// Return the scalar type of a shader parameter type
		constexpr ScalarType scalar_type(ShaderArithmeticType type)
		{
			switch (type)
			{
			case rex::gfx::ShaderArithmeticType::Bool:							return ScalarType::Bool;

			case rex::gfx::ShaderArithmeticType::Uint:							return ScalarType::Uint;
			case rex::gfx::ShaderArithmeticType::Uint2:						return ScalarType::Uint;
			case rex::gfx::ShaderArithmeticType::Uint3:						return ScalarType::Uint;
			case rex::gfx::ShaderArithmeticType::Uint4:						return ScalarType::Uint;

			case rex::gfx::ShaderArithmeticType::Ushort:						return ScalarType::Ushort;
			case rex::gfx::ShaderArithmeticType::Ushort2:					return ScalarType::Ushort;
			case rex::gfx::ShaderArithmeticType::Ushort3:					return ScalarType::Ushort;
			case rex::gfx::ShaderArithmeticType::Ushort4:					return ScalarType::Ushort;
			case rex::gfx::ShaderArithmeticType::UshortNorm:				return ScalarType::Ushort;
			case rex::gfx::ShaderArithmeticType::Ushort2Norm:			return ScalarType::Ushort;
			case rex::gfx::ShaderArithmeticType::Ushort3Norm:			return ScalarType::Ushort;
			case rex::gfx::ShaderArithmeticType::Ushort4Norm:			return ScalarType::Ushort;

			case rex::gfx::ShaderArithmeticType::UChar:						return ScalarType::Uchar;
			case rex::gfx::ShaderArithmeticType::UChar2:						return ScalarType::Uchar;
			case rex::gfx::ShaderArithmeticType::UChar3:						return ScalarType::Uchar;
			case rex::gfx::ShaderArithmeticType::UChar4:						return ScalarType::Uchar;
			case rex::gfx::ShaderArithmeticType::UCharNorm:				return ScalarType::Uchar;
			case rex::gfx::ShaderArithmeticType::UChar2Norm:				return ScalarType::Uchar;
			case rex::gfx::ShaderArithmeticType::UChar3Norm:				return ScalarType::Uchar;
			case rex::gfx::ShaderArithmeticType::UChar4Norm:				return ScalarType::Uchar;

			case rex::gfx::ShaderArithmeticType::Int:							return ScalarType::Int;
			case rex::gfx::ShaderArithmeticType::Int2:							return ScalarType::Int;
			case rex::gfx::ShaderArithmeticType::Int3:							return ScalarType::Int;
			case rex::gfx::ShaderArithmeticType::Int4:							return ScalarType::Int;

			case rex::gfx::ShaderArithmeticType::Short:						return ScalarType::Short;
			case rex::gfx::ShaderArithmeticType::Short2:						return ScalarType::Short;
			case rex::gfx::ShaderArithmeticType::Short3:						return ScalarType::Short;
			case rex::gfx::ShaderArithmeticType::Short4:						return ScalarType::Short;
			case rex::gfx::ShaderArithmeticType::ShortNorm:				return ScalarType::Short;
			case rex::gfx::ShaderArithmeticType::Short2Norm:				return ScalarType::Short;
			case rex::gfx::ShaderArithmeticType::Short3Norm:				return ScalarType::Short;
			case rex::gfx::ShaderArithmeticType::Short4Norm:				return ScalarType::Short;

			case rex::gfx::ShaderArithmeticType::Char:							return ScalarType::Char;
			case rex::gfx::ShaderArithmeticType::Char2:						return ScalarType::Char;
			case rex::gfx::ShaderArithmeticType::Char3:						return ScalarType::Char;
			case rex::gfx::ShaderArithmeticType::Char4:						return ScalarType::Char;
			case rex::gfx::ShaderArithmeticType::CharNorm:					return ScalarType::Char;
			case rex::gfx::ShaderArithmeticType::Char2Norm:				return ScalarType::Char;
			case rex::gfx::ShaderArithmeticType::Char3Norm:				return ScalarType::Char;
			case rex::gfx::ShaderArithmeticType::Char4Norm:				return ScalarType::Char;

			case rex::gfx::ShaderArithmeticType::Half:							return ScalarType::Half;
			case rex::gfx::ShaderArithmeticType::Half2:						return ScalarType::Half;
			case rex::gfx::ShaderArithmeticType::Half3:						return ScalarType::Half;
			case rex::gfx::ShaderArithmeticType::Half4:						return ScalarType::Half;

			case rex::gfx::ShaderArithmeticType::Float:						return ScalarType::Float;
			case rex::gfx::ShaderArithmeticType::Float2:						return ScalarType::Float;
			case rex::gfx::ShaderArithmeticType::Float3:						return ScalarType::Float;
			case rex::gfx::ShaderArithmeticType::Float4:						return ScalarType::Float;

			case rex::gfx::ShaderArithmeticType::Double:						return ScalarType::Double;
			case rex::gfx::ShaderArithmeticType::Double2:					return ScalarType::Double;
			case rex::gfx::ShaderArithmeticType::Double3:					return ScalarType::Double;
			case rex::gfx::ShaderArithmeticType::Double4:					return ScalarType::Double;

			default: break;

			}

			return ScalarType::Unknown;
		}

		// Return byte size of a shader parameter type
		constexpr s32 format_byte_size(ShaderArithmeticType type)
		{
			return component_count(type) * scalar_size(scalar_type(type));
		}

		// Return if the shader parameter type is normalized or not
		constexpr bool is_normalized_format(ShaderArithmeticType type)
		{
			switch (type)
			{
			case rex::gfx::ShaderArithmeticType::UshortNorm:
			case rex::gfx::ShaderArithmeticType::Ushort2Norm:
			case rex::gfx::ShaderArithmeticType::Ushort3Norm:
			case rex::gfx::ShaderArithmeticType::Ushort4Norm:

			case rex::gfx::ShaderArithmeticType::UCharNorm:
			case rex::gfx::ShaderArithmeticType::UChar2Norm:
			case rex::gfx::ShaderArithmeticType::UChar3Norm:
			case rex::gfx::ShaderArithmeticType::UChar4Norm:

			case rex::gfx::ShaderArithmeticType::ShortNorm:
			case rex::gfx::ShaderArithmeticType::Short2Norm:
			case rex::gfx::ShaderArithmeticType::Short3Norm:
			case rex::gfx::ShaderArithmeticType::Short4Norm:

			case rex::gfx::ShaderArithmeticType::CharNorm:
			case rex::gfx::ShaderArithmeticType::Char2Norm:
			case rex::gfx::ShaderArithmeticType::Char3Norm:
			case rex::gfx::ShaderArithmeticType::Char4Norm:
				return true;

			default: break;

			}

			return false;
		}

		// Determine if the gpu will be able to convert one format to the other
		constexpr bool is_convertible_shader_param_type(ShaderArithmeticType from, ShaderArithmeticType to)
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