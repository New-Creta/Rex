#include "rex_engine_pch.h"

#include "core/imguilayer.h"
#include "core/keycodes.h"

#include "events/input/mousedown.h"
#include "events/input/mouseup.h"
#include "events/input/mousescroll.h"
#include "events/input/mousemove.h"
#include "events/input/keydown.h"
#include "events/input/keyup.h"
#include "events/input/keytyped.h"
#include "events/window/windowresize.h"

#include "events/eventdispatcher.h"

// Temp
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl3.h"

#include "events/win32nativeevent.h"
#include "events/win32messageparameters.h"

namespace
{
    float FRAMERATE = 1.0f / 60.0f;

    //-------------------------------------------------------------------------
    int convertToImGuiKeyCode(rex::MouseCode code)
    {
        // A mouse button identifier inside ImGui look like this (0 = left, 1 = right, 2 = middle)

        switch (code)
        {
        case rex::MouseCode::LEFT:      return 0;
        case rex::MouseCode::RIGHT:     return 1;
        case rex::MouseCode::MIDDLE:    return 2;
        case rex::MouseCode::X1:        return 0;   // we will treat this as a left click for now
        case rex::MouseCode::X2:        return 0;   // we will treat this as a left click for now
        }

        RX_ERROR("Invalid mouse code given.");
        return -1;
    }
}

//-------------------------------------------------------------------------
rex::engine::ImGUILayer::ImGUILayer()
    :Layer("ImGui")
    , m_window(nullptr)
    , m_imgui_context_created(false)
{}
//-------------------------------------------------------------------------
rex::engine::ImGUILayer::~ImGUILayer() = default;

//-------------------------------------------------------------------------
void rex::engine::ImGUILayer::onAttach()
{
    BaseApplication* application = static_cast<BaseApplication*>(CoreApplication::getInstance());
    ApplicationWindow* window = static_cast<ApplicationWindow*>(application->getWindow());
    ApplicationContext* context = static_cast<ApplicationContext*>(application->getContext());

    m_window = window;
    m_context = context;

    ImGui_ImplWin32_EnableDpiAwareness();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();

    if (!m_imgui_context_created)
    {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;     // Enable Viewports

        io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;
        io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;
        
        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        ImGui_ImplWin32_Init(m_window->getNativeWindow(), m_context->getNativeContext());
        ImGui_ImplOpenGL3_Init("#version 460");

        m_imgui_context_created = true;
    }
}
//-------------------------------------------------------------------------
void rex::engine::ImGUILayer::onDetach()
{
    if (m_imgui_context_created)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplWin32_Shutdown();

        ImGui::DestroyContext();

        m_imgui_context_created = false;
    }
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//-------------------------------------------------------------------------
void rex::engine::ImGUILayer::onEvent(events::Event& event)
{
    UNUSED_PARAM(event);

    onProcessImGuiEvent(event);
}

//-------------------------------------------------------------------------
void rex::engine::ImGUILayer::onBeginRender()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}
//-------------------------------------------------------------------------
void rex::engine::ImGUILayer::onRender()
{
    ImGui::ShowDemoWindow();
}
//-------------------------------------------------------------------------
void rex::engine::ImGUILayer::onEndRender()
{
    ImGuiIO& io = ImGui::GetIO();

    io.DisplaySize = { (float)m_window->getWidth(), (float)m_window->getHeight() };

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ApplicationContext* current = ApplicationContext::getCurrent();

        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();

        ApplicationContext::makeCurrent(current);
    }
}

//-------------------------------------------------------------------------
bool rex::engine::ImGUILayer::onProcessImGuiEvent(events::Event& event)
{
    const win32::NativeEvent* native_event = dynamic_cast<const win32::NativeEvent*>(event.getNativeEvent());
    if (native_event == nullptr)
        return false;

    const win32::MessageParameters* data = reinterpret_cast<const win32::MessageParameters*>(native_event->getData());

    //
    // ImGui Implementation of the WND PROC
    //
    HWND hwnd = static_cast<HWND>(m_window->getNativeWindow());

    if (ImGui_ImplWin32_WndProcHandler(hwnd, data->msg, data->wparam, data->lparam))
        return true;

    return false;
}