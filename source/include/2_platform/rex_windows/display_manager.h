#pragma once

#include "gsl/gsl_not_null.h"

namespace rex
{
    class Display;
    class DisplayMode;

    using Displays = std::unordered_map<int32, Display>;

    class DisplayManager
    {
    public:
        DisplayManager();
        ~DisplayManager();

        rex::not_null<const Display*> set_active(int32 displayIndex, int32 modeIndex = 0);
        rex::not_null<const Display*> get_active() const;

        int32 display_count() const;

        int32 display_mode_count() const;
        int32 display_mode_count(int32 displayIndex) const;

    private:
        const Display* m_active_display;
        Displays m_displays;
    };
}