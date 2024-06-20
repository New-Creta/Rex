#pragma once

namespace rex
{
	namespace gfx
	{
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