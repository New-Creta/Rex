#include "rex_renderer_core/system/input_layout_cache.h"

#include "rex_renderer_core/gfx/rhi.h"

#include "rex_std/functional.h"
#include "rex_std/unordered_map.h"

namespace rex
{
	namespace gfx
	{
		namespace input_layout_cache
		{
			rsl::unordered_map<InputLayoutDesc, rsl::unique_ptr<InputLayout>> g_cached_layouts;

			InputLayout* load(const InputLayoutDesc& desc)
			{
				auto it = g_cached_layouts.find(desc);
				if (it != g_cached_layouts.end())
				{
					return it->value.get();
				}

				auto layout = rhi::create_input_layout(desc);
				return g_cached_layouts.emplace(desc, rsl::move(layout)).inserted_element->value.get();
			}
		}
	}
}