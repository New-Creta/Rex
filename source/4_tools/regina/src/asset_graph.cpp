#include "regina/asset_graph.h"

#include "rex_engine/event_system/event_system.h"

#include "rex_engine/event_system/events/loading/begin_asset_load.h"
#include "rex_engine/event_system/events/loading/end_asset_load.h"

namespace regina
{
	AssetGraph::AssetGraph()
	{
		rex::event_system::instance()->subscribe<rex::BeginAssetLoad>([this](const rex::BeginAssetLoad& evt) { on_begin_asset_load(evt.asset_path()); });
		rex::event_system::instance()->subscribe<rex::EndAssetLoad>([this](const rex::EndAssetLoad& evt) { on_end_asset_load(evt.asset_path(), evt.asset()); });
	}

	void AssetGraph::on_begin_asset_load(rsl::string_view assetPath)
	{
		m_asset_dependency_stack.push_back(assetPath);
	}
	void AssetGraph::on_end_asset_load(rsl::string_view assetPath, const rex::Asset* /*asset*/)
	{
		m_asset_dependency_stack.pop_back();

		// If we still have any assets left in the stack
		// We add the most recently loaded asset as a dependency to the last one
		if (!m_asset_dependency_stack.empty())
		{
			rsl::string_view currently_loading_asset = m_asset_dependency_stack.back();
			m_asset_dependencies[currently_loading_asset].emplace_back(assetPath);
		}
	}
}