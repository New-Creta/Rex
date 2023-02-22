#include "rex_engine/entrypoint.h"

#include "rex_engine/diagnostics/logging.h"

#include "rex_linux/gui_application.h"
#include "rex_linux/platform_creation_params.h"

int main(/* int argc, char* argv[] */)
{
    REX_STATIC_TODO("Make use of \"int argc\" and \"char* argv[]\" here");

    rex::CommandLineArguments cmd_args(nullptr);
    rex::PlatformCreationParams creation_params {};
    
    const rex::ApplicationCreationParams app_params = rex::app_entry(rsl::move(creation_params), rsl::move(cmd_args));

    rex::linux::GuiApplication application(app_params);
    s32 result = application.run();

    REX_INFO("Application completed with result: {0}", result);

    return result;
}