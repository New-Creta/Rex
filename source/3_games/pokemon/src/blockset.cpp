#include "pokemon/blockset.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"

#include "rex_engine/memory/blob.h"
#include "rex_engine/memory/blob_reader.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/engine/object_pool.h"

namespace pokemon
{
  DEFINE_LOG_CATEGORY(LogBlockSet);

  //Block::Block(const tile_indices& tileIndices)
  //  : m_tile_indices(rsl::move(tileIndices))
  //{}

  //const Block::tile_indices& Block::indices() const
  //{
  //  return m_tile_indices;
  //}

  BlockSet::BlockSet(rsl::unique_array<Block> blocks)
    : m_blocks(rsl::move(blocks))
  {

  }

  const Block& BlockSet::block(s32 idx) const
  {
    return m_blocks[idx];
  }

	rsl::shared_ptr<BlockSet> load_blockset(rsl::string_view filepath)
	{
    if (!rex::vfs::is_file(filepath))
    {
      REX_ERROR(LogBlockSet, "Failed to load blockset. Filepath doesn't exist. filepath: {}", filepath);
      return nullptr;
    }

    s64 file_size = rex::file::size(filepath);
    s32 num_blocks = file_size / Block::num_tiles();
    rex::memory::Blob file_blob = rex::vfs::read_file(filepath);
    rex::memory::BlobReader reader(file_blob);
		Block::indices_array blob_memory;
    rsl::unique_array<Block> blocks = rsl::make_unique<Block[]>(num_blocks);
    for (s32 block_idx = 0; block_idx < num_blocks; ++block_idx)
    {
      reader.read(blob_memory.data(), blob_memory.size());
      blocks[block_idx] = Block(blob_memory);
    }

    return rsl::make_shared<BlockSet>(rsl::move(blocks));
    //return rex::load_object<BlockSet>(rsl::move(blocks));
	}
}