#pragma once

#include "win_types.h"

#include "rex_std/string_view.h"
#include "rex_std/memory.h"

namespace rex
{
    namespace win32
    {
        class WindowClass
        {
        public:
            WindowClass(const rsl::string_view name, WindowProcedureFunc wnd_proc);

            const rsl::string_view class_name() const;
            HInstance hinstance() const;

        private:
            struct Internal;
            rsl::unique_ptr<Internal> m_internal_ptr;
        };
    }
}