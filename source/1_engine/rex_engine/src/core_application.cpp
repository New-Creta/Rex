#include "rex_engine/core_application.h"

#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_std/assert.h"
#include "rex_std/chrono.h"
#include "rex_std/functional.h"
#include "rex_std/math.h"
#include "rex_std/memory.h"

namespace rex
{
  struct IApplication::Internal
  {
    static IApplication* s_instance; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
  };

  //-------------------------------------------------------------------------
  IApplication* IApplication::Internal::s_instance = nullptr; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

  //-------------------------------------------------------------------------
  IApplication::IApplication()
      : m_internal_ptr(rsl::make_unique<Internal>())
  {
    REX_ASSERT_X(IApplication::Internal::s_instance == nullptr, "There can only be one application at the time");
    IApplication::Internal::s_instance = this;
  }
  //-------------------------------------------------------------------------
  IApplication::~IApplication()
  {
    IApplication::Internal::s_instance = nullptr;
  }
} // namespace rex