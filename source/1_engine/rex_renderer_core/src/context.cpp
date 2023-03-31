#include "rex_renderer_core/context.h"

#include "rex_engine/diagnostics/legacy/logging.h"

namespace rex
{
  namespace context
  {
    bool create(void* userData)
    {
      bool result = true;

      result &= os::pre_setup(userData);
      if(!result)
      {
        os::rollback(userData);

        REX_ERROR("Context OS setup failed!");
        return false;
      }

      result &= backend::create(userData);
      if(!result)
      {
        os::rollback(userData);

        REX_ERROR("Context backend initialization failed!");
        return false;
      }

      result &= os::post_setup(userData);
      if(!result)
      {
        os::rollback(userData);

        REX_ERROR("Context OS setup failed!");
        return false;
      }

      return result;
    }
  } // namespace context
} // namespace rex