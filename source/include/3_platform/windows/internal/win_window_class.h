//--------------------------------------------------
// File:    win_window_class.h
// Created: 3/24/2022 9:54:46 PM
//
// Created By: nickdb
// 
// Copyright (c) REX
//--------------------------------------------------

#pragma once

#include "win_types.h"

#include <rex_stl/string/string_view.h>

// Be careful when including this header as it'll include Windows.h!!
#define NOMINMAX
#include <Windows.h>

namespace rex
{
    namespace win32
    {
        class WindowClass
        {
        public:
            WindowClass(const rtl::StringView name, WindowProcedureFunc wnd_proc);
            ~WindowClass();

            const rtl::StringView class_name() const;
            HInstance hinstance() const;

        private:
            WNDCLASS m_window_class;
            rtl::StringView m_name;
            HInstance m_hinstance;
        };
    }
}