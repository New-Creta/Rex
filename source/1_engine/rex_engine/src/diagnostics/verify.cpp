#include "rex_engine/diagnostics/verify.h"

#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "tinyfiledialogs.h"

namespace rex
{
  DEFINE_LOG_CATEGORY(LogVerify, LogVerbosity::Error);

  //-------------------------------------------------------------------------
  void rex_verify(rsl::string_view condition, rsl::string_view msg)
  {
    REX_ERROR(LogVerify, "Verification Failed: {}", condition);
    REX_ERROR(LogVerify, msg);

    tinyfd_messageBox("Verification Failed", msg.data(), "ok", "error", 0);

    DEBUG_BREAK();
  }

  //-------------------------------------------------------------------------
  void rex_verify(rsl::string_view msg)
  {
    rex_verify("forced verify", msg);
  }
} // namespace rex