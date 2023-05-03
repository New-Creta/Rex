#pragma once

#include "rex_engine/types.h"

#include "rex_std/type_traits.h"
#include "rex_std/functional.h"
#include "rex_std/memory.h"
#include "rex_std/string_view.h"
#include "rex_std/bonus/functional.h"
#include "rex_std_extra/utility/type_id.h"

namespace rex
{
  namespace vfs
  {
    void init(rsl::string_view root = "");

    REX_NO_DISCARD rsl::unique_array<char8> open_file(rsl::string_view filepath);
    void save_to_file(rsl::string_view filepath, const void* data, card64 size, bool append);

    bool exists(rsl::string_view path);
    bool is_dir(rsl::string_view path);
    bool is_file(rsl::string_view path);
    bool is_abs(rsl::string_view path);
    bool is_rel(rsl::string_view path);
  }
}