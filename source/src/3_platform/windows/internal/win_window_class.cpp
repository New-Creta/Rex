//--------------------------------------------------
// File:    win_window_class.cpp
// Created: 3/24/2022 9:55:08 PM
//
// Created By: nickdb
//
// Copyright (c) REX
//--------------------------------------------------

#include "internal/win_window_class.h"

#include <rex_stl/diagnostics/win/win_call.h>
#include <rex_stl/diagnostics/logging.h>

rex::win32::WindowClass::WindowClass(const rtl::StringView name, WindowProcedureFunc wnd_proc)
    : m_window_class()
    , m_name(name)
    , m_hinstance(GetModuleHandleA(NULL))
{
    ZeroMemory(&m_window_class, sizeof(m_window_class));

    REX_TODO("Make window style data driven");
    REX_TODO("Make window icon data driven");
    REX_TODO("Make window cursor data driven");

    m_window_class.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    m_window_class.lpfnWndProc = (WNDPROC)wnd_proc;
    m_window_class.hInstance = (HINSTANCE)m_hinstance;
    m_window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    m_window_class.hCursor = LoadCursor(NULL, IDC_ICON);
    m_window_class.lpszClassName = m_name.data();

    if (WIN_FAILED(RegisterClass(&m_window_class)))
    {
        REX_ERROR("Failed to create window class!");
    }
}
rex::win32::WindowClass::~WindowClass()
{
    if (WIN_FAILED(UnregisterClass(m_name.data(), (HINSTANCE)m_hinstance)))
    {
        REX_ERROR("Failed to destroy window class!");
    }
}

const rtl::StringView rex::win32::WindowClass::class_name() const
{
    return m_name;
}

rex::win32::HInstance rex::win32::WindowClass::hinstance() const
{
    return m_hinstance;
}