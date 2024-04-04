#include "rex_engine/diagnostics/verify.h"

#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_engine/diagnostics/file_dialogs.h"


namespace rex
{
  DEFINE_LOG_CATEGORY(LogVerify);

  //-------------------------------------------------------------------------
  void rex_verify(rsl::string_view condition, rsl::string_view msg)
  {
    REX_ERROR(LogVerify, "Verification Failed: {}", condition);
    REX_ERROR(LogVerify, msg);

    rex::dialog::msg_box("Verification failed", msg, dialog::DialogType::Ok, dialog::IconType::Error, dialog::DefaultButton::Ok);

    DEBUG_BREAK();
  }

  //-------------------------------------------------------------------------
  void rex_verify(rsl::string_view msg)
  {
    rex_verify("forced verify", msg);
  }
} // namespace rex