#include "rex_engine/platform/win/filesystem/win_vfs.h"

#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_engine/engine/state_controller.h"
#include "rex_engine/engine/casting.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/directory.h"
#include "rex_engine/platform/win/diagnostics/win_call.h"
#include "rex_std/bonus/atomic/atomic.h"
#include "rex_std/bonus/hashtable.h"
#include "rex_std/bonus/memory.h"
#include "rex_std/bonus/platform.h"
#include "rex_std/bonus/string.h"
#include "rex_std/bonus/utility/enum_reflection.h"
#include "rex_std/ctype.h"
#include "rex_std/mutex.h"
#include "rex_std/thread.h"
#include "rex_std/unordered_map.h"
#include "rex_std/vector.h"

#include <Windows.h>
#include <processenv.h>
#include <cstddef>

DEFINE_LOG_CATEGORY(LogFileSystem);

// NOLINTBEGIN(modernize-use-nullptr)

// For documentation on how the vfs works, see vfs.cpp

namespace rex
{
  namespace vfs
  {


  } // namespace vfs
} // namespace rex

// NOLINTEND(modernize-use-nullptr)
