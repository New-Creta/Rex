#pragma once

namespace rex
{
	namespace gfx
	{
    // A class type of a variable that's specified in a shader's contant buffer or as a standalone constant
		enum class ShaderClassType
		{
      Scalar = 0,
      Vector,
      MatrixRows,
      MatrixColumns,
      Object,
      Struct,
      InterfaceClass,
      InterfacePointer
		};
	}
}