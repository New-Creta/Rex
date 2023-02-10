#include "rex_engine/core_application.h"

#include "rex_engine/memory/memory_manager.h"

#include "rex_std/assert.h"
#include "rex_std/chrono.h"
#include "rex_std/functional.h"
#include "rex_std/math.h"
#include "rex_std/memory.h"
#include "rex_std_extra/memory/memory_size.h"

namespace rex
{
  struct CoreApplication::Internal
  {
  public:
    CoreApplication::Internal()
    {
      mem_manager().initialize(256_kib);
    }

    static CoreApplication* s_instance; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
  };
  
  //-------------------------------------------------------------------------
  CoreApplication* CoreApplication::Internal::s_instance = nullptr; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

  //-------------------------------------------------------------------------
  CoreApplication::CoreApplication()
      : m_internal_ptr(rsl::make_unique<Internal>())
  {
    REX_ASSERT_X(CoreApplication::Internal::s_instance == nullptr, "There can only be one application at the time");
    CoreApplication::Internal::s_instance = this;
  }
  //-------------------------------------------------------------------------
  CoreApplication::~CoreApplication()
  {
    CoreApplication::Internal::s_instance = nullptr;
  }
} // namespace rex