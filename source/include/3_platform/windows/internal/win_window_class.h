#pragma once

#include "win_types.h"

#include <rex_stl/string/string_view.h>
#include <rex_stl/memory/unique_ptr.h>

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
            rtl::UniquePtr<Internal> m_internal_ptr;
        };
    }
}