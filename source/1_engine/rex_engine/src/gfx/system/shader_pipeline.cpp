#include "rex_engine/gfx/system/shader_pipeline.h"

namespace rex
{
	namespace gfx
	{
		bool operator==(const ShaderPipeline& lhs, const ShaderPipeline& rhs)
		{
			return
				lhs.vs == rhs.vs &&
				lhs.ps == rhs.ps
				;
		}
		bool operator!=(const ShaderPipeline& lhs, const ShaderPipeline& rhs)
		{
			return !(lhs == rhs);
		}
	}
}