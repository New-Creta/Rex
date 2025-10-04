#pragma once

#include "rex_engine/event_system/event.h"

#include "rex_std/string_view.h"

namespace rex
{
	// This event gets raised when an asset is about to be loaded
	class BeginAssetLoad : public Event<BeginAssetLoad>
	{
	public:
		BeginAssetLoad(rsl::string_view assetPath)
			: m_asset_path(assetPath)
		{}

		rsl::string_view asset_path() const
		{
			return m_asset_path;
		}

	private:
		rsl::string_view m_asset_path;
	};
}