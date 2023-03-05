#include "rex_engine/core_application.h"

#include "rex_engine/memory/memory_tracking.h"
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
    Internal(const EngineParams& engineParams, const CommandLineArguments& /*cmdArgs*/)
    {
      // load memory config file from disk
      // this file only has high level memory settings
      // eg. how much memory we're max allowed to use
      // it can't split this up in domains as the engine
      // doesn't know which domains you will have
      // this is app specific and is managed by the client
      mem_tracker().initialize(engineParams.max_memory);
    }

    static CoreApplication* s_instance; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
  };

  //-------------------------------------------------------------------------
  CoreApplication* CoreApplication::Internal::s_instance = nullptr; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

  //-------------------------------------------------------------------------
  CoreApplication::CoreApplication(const EngineParams& engineParams, const CommandLineArguments& cmdArgs)
      : m_internal_ptr(rsl::make_unique<Internal>(engineParams, cmdArgs))
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