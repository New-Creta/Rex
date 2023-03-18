#include "rex_engine/core_application.h"

#include "rex_engine/memory/memory_tracking.h"
#include "rex_engine/filesystem/filesystem.h"
#include "rex_std/assert.h"
#include "rex_std/chrono.h"
#include "rex_std/functional.h"
#include "rex_std/math.h"
#include "rex_std/memory.h"
#include "rex_std_extra/memory/memory_size.h"

namespace rex
{
  //-------------------------------------------------------------------------
  CoreApplication::CoreApplication(const EngineParams& engineParams, const CommandLineArguments& /*cmdArgs*/)
  {
    vfs::init();
    mem_tracker().initialize(engineParams.max_memory);
  }
  //-------------------------------------------------------------------------
  CoreApplication::~CoreApplication() = default;
} // namespace rex