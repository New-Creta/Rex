#include "rex_renderer_core/context.h"
#include "rex_renderer_core/log.h"

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

        REX_ERROR(LogRendererCore, "Context OS setup failed!");
        return false;
      }

      result &= backend::create(userData);
      if(!result)
      {
        os::rollback(userData);

        REX_ERROR(LogRendererCore, "Context backend initialization failed!");
        return false;
      }

      result &= os::post_setup(userData);
      if(!result)
      {
        os::rollback(userData);

        REX_ERROR(LogRendererCore, "Context OS setup failed!");
        return false;
      }

      return result;
    }
  } // namespace context
} // namespace rex