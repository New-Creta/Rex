#pragma once

#include "display_mode.h"

#include "gsl/gsl_not_null.h"

namespace rex
{
    using DisplayModes = std::unordered_map<int32, DisplayMode>;

    struct DisplayDescription
    {
        DisplayDescription()
            : name("None")
            , display_index(-1)
            , display_modes()
        {
        }

        const char* name;

        int32 display_index;

        DisplayModes display_modes;
    };

    class Display
    {
    public:
        Display();
        Display(const Display& other);
        Display(Display&& other) noexcept;
        Display(const DisplayDescription& description);
        ~Display();

        Display& operator=(const Display& other);
        Display& operator=(Display&& other) noexcept;

        const char* get_name() const;

        rex::not_null<const DisplayMode*> set_active_mode(int32 index);
        rex::not_null<const DisplayMode*> get_active_mode() const;

        int32 get_display_index() const;
        int32 get_display_mode_count() const;

        std::optional<DisplayMode> get_display_mode(int32 index) const;

    private:
        const DisplayMode* m_active_display_mode;
        DisplayDescription m_description;
    };
}