#include "rex_platform_pch.h"

#include "core/win32window.h"

#include "math/bounds.h"

namespace
{
    const DWORD STYLE = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

    //-------------------------------------------------------------------------
    rex::win32::EventProcessor* getUserData(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        UNREFERENCED_PARAMETER(wParam);

        if (uMsg == WM_CREATE)
        {
            LONG_PTR user_data = (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams;

            SetWindowLongPtr(hWnd, GWLP_USERDATA, user_data);

            return (rex::win32::EventProcessor*)(user_data);
        }

        return (rex::win32::EventProcessor*)(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    //-------------------------------------------------------------------------
    rex::BoundsI createWindowBounds(uint32 width, uint32 height)
    {
        POINT top_left
        {
            GetSystemMetrics(SM_CYSCREEN) / 2 - static_cast<long>(height) / 2,  // top
            GetSystemMetrics(SM_CXSCREEN) / 2 - static_cast<long>(width) / 2    // left
        };
        POINT bottom_right
        {
            top_left.x + static_cast<long>(height),                             // bottom
            top_left.y + static_cast<long>(width)                               // right
        };

        // Bounds order => top - left - bottom - right
        return rex::BoundsI(top_left.x, top_left.y, bottom_right.x, bottom_right.y);
    }
    //-------------------------------------------------------------------------
    WNDCLASS createWindowClass(const std::string& appName, HINSTANCE hInst, WNDPROC windowProcedure)
    {
        WNDCLASS wc;
        memset(&wc, NULL, sizeof(WNDCLASS));

        wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.lpfnWndProc = windowProcedure;
        wc.hInstance = hInst;
        wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
        wc.lpszClassName = appName.c_str();
        wc.style = CS_OWNDC;

        return wc;
    }
}

//-------------------------------------------------------------------------
rex::win32::Window::Window(const WindowProperties& properties)
    :rex::Window()
    ,m_visible(false)
    ,m_hinstance(NULL)
    ,m_hwnd(NULL)
    ,m_wndproc((WNDPROC)wndProc)
    ,m_event_processor(this, properties.event_callback)
    ,m_width(properties.width)
    ,m_height(properties.height)
    ,m_title(properties.title)
{
    RX_INFO("Creating window: {0} ({1}, {2})", m_title, m_width, m_height);

    setupWindowClass();
    setupHwnd();
    setupHdc();
}
//-------------------------------------------------------------------------
rex::win32::Window::~Window()
{
    ReleaseDC(m_hwnd, m_hdc);

    destroyHwnd();
    destroyWindowClass();
}

//-------------------------------------------------------------------------
void rex::win32::Window::show()
{
    if (m_visible)
        return;

    m_visible = true;

    ShowWindow(m_hwnd, SW_SHOWDEFAULT);

    if (!SetForegroundWindow(m_hwnd))
    {
        RX_ERROR("Could not make the window the foreground window");
        return;
    }
    if (SetFocus(m_hwnd) == NULL)
    {
        RX_ERROR("Could not focus the window");
        return;
    }
}
//-------------------------------------------------------------------------
void rex::win32::Window::hide()
{
    if (!m_visible)
        return;

    m_visible = false;

    ShowWindow(m_hwnd, SW_HIDE);
    UpdateWindow(m_hwnd);
}

//-------------------------------------------------------------------------
bool rex::win32::Window::processEvents()
{
    if (m_visible == false)
        return false;

    MSG message;
    if (PeekMessage(&message, NULL, NULL, NULL, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);

        return true;
    }

    return false;
}

//-------------------------------------------------------------------------
void rex::win32::Window::update()
{
    SwapBuffers(m_hdc);
}

//-------------------------------------------------------------------------
unsigned int rex::win32::Window::getWidth() const
{
    return m_width;
}
//-------------------------------------------------------------------------
unsigned int rex::win32::Window::getHeight() const
{
    return m_height;
}

//-------------------------------------------------------------------------
void* rex::win32::Window::getNativeWindow() const
{
    return (void*)m_hwnd;
}

//-------------------------------------------------------------------------
LRESULT CALLBACK rex::win32::Window::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    rex::win32::EventProcessor* event_processor = getUserData(hWnd, uMsg, wParam, lParam);
    if (event_processor != nullptr)
    {
        return event_processor->process(hWnd, { uMsg, wParam, lParam });
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}   

//-------------------------------------------------------------------------
void rex::win32::Window::setupWindowClass()
{
    WNDCLASS wndClass = createWindowClass(m_title, m_hinstance, m_wndproc);

    auto result = RegisterClass(&wndClass);
    if (!result)
    {
        RX_ERROR("Could not register WNDCLASS");
        return;
    }
}
//-------------------------------------------------------------------------
void rex::win32::Window::setupHwnd()
{
    m_event_processor.enableEventProcessing();

    rex::BoundsI bounds = createWindowBounds(getWidth(), getHeight());

    m_hwnd = CreateWindowEx(0,
        m_title.c_str(),
        m_title.c_str(),
        STYLE,
        bounds.left, bounds.top,
        bounds.width(), bounds.height(),
        NULL,
        NULL,
        (HINSTANCE)GetModuleHandle(NULL),
        &m_event_processor);

    if (m_hwnd == NULL)
    {
        RX_ERROR("Failed to create window");
        return;
    }
}
//-------------------------------------------------------------------------
void rex::win32::Window::setupHdc()
{
    m_hdc = GetDC(m_hwnd);
}

//-------------------------------------------------------------------------
void rex::win32::Window::destroyWindowClass()
{
    const int8* wndclass_name = m_title.c_str();

    WNDCLASS wnd_class;
    if (GetClassInfo(m_hinstance, wndclass_name, &wnd_class) != NULL)
    {
        auto result = UnregisterClass(wndclass_name, m_hinstance);
        if (result == NULL)
        {
            RX_ERROR("Failed to unregistger window class!");
        }
    }
}

//-------------------------------------------------------------------------
void rex::win32::Window::destroyHwnd()
{
    RX_ASSERT_X(m_hwnd, "Window is already destroyed!");

    BOOL result = DestroyWindow(m_hwnd);
    if (result == NULL)
    {
        RX_ERROR("Failed to destroy the window");
    }

    m_hwnd = NULL;

    m_event_processor.disableEventProcessing();
}
