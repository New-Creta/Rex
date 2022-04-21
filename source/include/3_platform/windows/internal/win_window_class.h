#pragma once

#include "win_types.h"

#include <rex_stl/string/string_view.h>

namespace rex
{
    namespace win32
    {
        class WindowClass
        {
        public:
            WindowClass(const rtl::StringView name, WindowProcedureFunc wnd_proc);

            const rtl::StringView class_name() const;
            HInstance hinstance() const;

        private:
            struct Internal;
            std::unique_ptr<Internal> m_internal_ptr;
        };
    }
}