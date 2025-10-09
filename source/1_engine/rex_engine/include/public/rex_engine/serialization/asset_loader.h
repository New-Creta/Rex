#pragma once

#include "rex_engine/assets/asset.h"
#include "rex_engine/engine/defines.h"
#include "rex_engine/diagnostics/log.h"

#include "rex_std/string.h"
#include "rex_std/vector.h"
#include "rex_std/bonus/utility/version.h"
#include "rex_std/bonus/utility/output_param.h"

namespace rex
{
	struct AssetLoaderDesc
	{
		rsl::version version;
		rsl::vector<rsl::string_view> extensions;
	};

	enum class LoadFlags
	{
		None,
		PartialLoad = BIT(0)
	};

	DEFINE_LOG_CATEGORY(LogAssetLoader);

	class AssetLoader
	{
	public:
		AssetLoader(AssetLoaderDesc&& desc);
		virtual ~AssetLoader() = default;

		void allowed_extensions(rsl::Out<rsl::vector<rsl::string_view>> extensions) const;

		virtual bool can_load(rsl::string_view assetPath) const;
		virtual void hydrate_asset(Asset* asset, rsl::string_view assetPath);

		virtual rsl::unique_ptr<Asset> load(rsl::string_view assetPath, LoadFlags loadFlags) = 0;

	protected:
		bool has_allowed_extension(rsl::string_view assetPath) const;

	private:
		AssetLoaderDesc m_desc;
	};
}