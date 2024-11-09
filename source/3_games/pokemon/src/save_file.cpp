#include "pokemon/save_file.h"

#include "rex_engine/memory/blob.h"
#include "rex_engine/text_processing/json.h"
#include "rex_engine/filesystem/vfs.h"

namespace pokemon
{
  SaveFile::SaveFile(rsl::string_view filepath)
  {
    rex::memory::Blob file_content = rex::vfs::read_file(filepath);
    rex::json::json json_blob = rex::json::parse(file_content);

    current_map.assign(json_blob["map"].get<rsl::string_view>());
    position.x = json_blob["position"]["x"];
    position.y = json_blob["position"]["x"];
  }

}