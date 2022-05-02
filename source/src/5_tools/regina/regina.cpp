#include "regina_pch.h"
#include "entrypoint.h"
#include "win32_window.h"
#include "regina.h"

namespace tags
{
    const std::string WINDOW_WIDTH = "xres";
    const std::string WINDOW_HEIGHT = "yres";
}

//-----------------------------------------------------------------------
rex::CoreApplication* rex::create_application(const ApplicationArguments& arguments)
{
    std::string s_window_width = arguments.get_argument_value(tags::WINDOW_WIDTH);
    std::string s_window_height = arguments.get_argument_value(tags::WINDOW_WIDTH);

    rex::ApplicationDescription description;

    description.name = "rex";
    description.window_width = !s_window_width.empty() ? std::stoi(s_window_width) : 1280;
    description.window_height = !s_window_height.empty() ? std::stoi(s_window_height) : 720;

    // R_ASSERT(description.window_width > 16);
    // R_ASSERT(description.window_height > 16);

    return new regina::Regina(description);
}

namespace regina
{
    //-------------------------------------------------------------------------
    Regina::Regina(const rex::ApplicationDescription& description)
        : rex::GraphicalApplication(description)
    {
    }
    //-------------------------------------------------------------------------
    Regina::~Regina() = default;
}