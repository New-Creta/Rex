#include "rex_engine/entrypoint.h"

#include "rex_std/string.h"

namespace rex
{
  namespace internal
  {
    void pre_app_entry(REX_MAYBE_UNUSED const char8* cmdLine) {}

    void post_app_shutdown() {}
  } // namespace internal
} // namespace rex