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

    void save_to_file(rsl::string_view filepath, const void* data, card64 size, bool append);
  }
}