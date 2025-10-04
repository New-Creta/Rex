#pragma once

#include "rex_engine/event_system/event.h"
#include "rex_engine/assets/asset.h"

#include "rex_std/string_view.h"

namespace rex
{
	// this event is raised when an asset is successfully loaded
	class EndAssetLoad : public Event<EndAssetLoad>
	{
	public:
		EndAssetLoad(rsl::string_view assetPath, const Asset* asset)
			: m_asset_path(assetPath)
			, m_asset(asset)
		{}

		rsl::string_view asset_path() const
		{
			return m_asset_path;
		}
		const Asset* asset() const
		{
			return m_asset;
		}

	private:
		rsl::string_view m_asset_path;
		const Asset* m_asset;
	};
}