#include "pokemon/map_blocks.h"

#include "rex_engine/diagnostics/log.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/memory/blob.h"
#include "rex_engine/memory/blob_reader.h"

namespace pokemon
{
	DEFINE_LOG_CATEGORY(LogMapBlocks);

	MapBlocks::MapBlocks(rsl::vector<s8>&& blockIndices)
		: m_block_indices(rsl::move(blockIndices))
	{}

	rsl::shared_ptr<MapBlocks> load_blocks(rsl::string_view filepath)
	{
    if (!rex::vfs::is_file(filepath))
    {
      REX_ERROR(LogMapBlocks, "Failed to load map blocks. Filepath doesn't exist. filepath: {}", filepath);
      return nullptr;
    }

    s32 file_size = rex::file::size(filepath);
    s32 num_blocks = file_size;
    rsl::vector<s8> content;
    content.resize(num_blocks);
    rex::vfs::read_file(content.data(), content.size());

    return rex::load_object<MapBlocks>(rsl::move(content));

	}
}