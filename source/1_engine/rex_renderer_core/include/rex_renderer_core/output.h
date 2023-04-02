#pragma once

#include "rex_std/string.h"
#include "rex_std/vector.h"
#include "rex_std/map.h"
#include "rex_std/ostream.h"
#include "rex_renderer_core/format_type.h"

namespace rex
{
    enum class FormatType;
    class Gpu;
    struct DisplayMode;

    class Output
    {
    public:
        Output() = default;
        virtual ~Output() = default;

        virtual const rsl::string& name() const = 0;
        virtual const Gpu* adapter() const = 0;
        virtual const rsl::vector<DisplayMode> display_modes(FormatType type) const = 0;
        virtual const rsl::map<FormatType, rsl::vector<DisplayMode>>& display_modes() const = 0;
    };

    rsl::ostream& operator<<(rsl::ostream& os, const Output& obj);
}