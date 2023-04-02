#include "rex_renderer_core/output.h"
#include "rex_renderer_core/display_mode.h"

namespace rex
{
    //-------------------------------------------------------------------------
    rsl::ostream& operator<<(rsl::ostream& os, const Output& obj)
    {
        auto modes = obj.display_modes();

        os << "\n";
        os << "Device Name: " << obj.name() << "\n";
        for (const auto& pair : modes)
        {
            const FormatType format_type = pair.key;
            const rsl::vector<DisplayMode> display_modes = pair.value;

            os << "Format: " << format_type << "\n";

            for (const auto& display : display_modes)
            {
                const auto rr = display.refresh_rate;

                os << "Width: " << display.width << "\n";
                os << "Height: " << display.height << "\n";
                os << "Format: " << display.format << "\n";
                os << "Refresh: " << static_cast<double>(rr.numerator()) / rr.denominator();
            }
        }

        return os;
    }
}