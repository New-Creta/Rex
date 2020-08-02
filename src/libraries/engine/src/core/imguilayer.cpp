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

#include "imgui_impl_win32.h"
#include "imgui_impl_opengl3.h"

namespace
{
    float FRAMERATE = 1.0f / 60.0f;
}

//-------------------------------------------------------------------------
rex::engine::ImGUILayer::ImGUILayer()
    :Layer("ImGui")
    , m_window(nullptr)
    , m_imgui_context_created(false)
{}
//-------------------------------------------------------------------------
rex::engine::ImGUILayer::~ImGUILayer()
{
    if (m_imgui_context_created)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext();

        ImGui_ImplWin32_Shutdown();
    }
}

//-------------------------------------------------------------------------
void rex::engine::ImGUILayer::onAttach()
{
    BaseApplication* application = static_cast<BaseApplication*>(CoreApplication::getInstance());
    ApplicationWindow* window = static_cast<ApplicationWindow*>(application->getWindow());

    m_window = window;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();

    if (!m_imgui_context_created)
    {
        ImGui::CreateContext();

        ImGui_ImplWin32_Init(m_window->getHandle());
        ImGui_ImplOpenGL3_Init("#version 460");

        ImGui::StyleColorsDark();

        m_imgui_context_created = true;
    }
}
//-------------------------------------------------------------------------
void rex::engine::ImGUILayer::onDetach()
{}

//-------------------------------------------------------------------------
void rex::engine::ImGUILayer::onUpdate()
{
    ImGuiIO& io = ImGui::GetIO();

    io.DeltaTime = FRAMERATE;
    io.DisplaySize = { (float)m_window->getWidth(), (float)m_window->getHeight() };

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
//-------------------------------------------------------------------------
void rex::engine::ImGUILayer::onEvent(events::Event& event)
{
    events::EventDispatcher dispatcher(event);

    dispatcher.dispatch<events::MouseDown>(std::bind(&rex::engine::ImGUILayer::onMouseButtonPressedEvent, this, std::placeholders::_1));
    dispatcher.dispatch<events::MouseUp>(std::bind(&rex::engine::ImGUILayer::onMouseButtonReleasedEvent, this, std::placeholders::_1));
    dispatcher.dispatch<events::MouseMove>(std::bind(&rex::engine::ImGUILayer::onMouseMovedEvent, this, std::placeholders::_1));
    dispatcher.dispatch<events::MouseScroll>(std::bind(&rex::engine::ImGUILayer::onMouseScrolledEvent, this, std::placeholders::_1));

    dispatcher.dispatch<events::KeyDown>(std::bind(&rex::engine::ImGUILayer::onKeyPressedEvent, this, std::placeholders::_1));
    dispatcher.dispatch<events::KeyUp>(std::bind(&rex::engine::ImGUILayer::onKeyReleasedEvent, this, std::placeholders::_1));
    dispatcher.dispatch<events::KeyTyped>(std::bind(&rex::engine::ImGUILayer::onKeyTypedEvent, this, std::placeholders::_1));

    dispatcher.dispatch<events::WindowResize>(std::bind(&rex::engine::ImGUILayer::onWindowResizedEvent, this, std::placeholders::_1));
}

//-------------------------------------------------------------------------
bool rex::engine::ImGUILayer::onMouseButtonPressedEvent(events::MouseDown& e)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[(int)e.getButton()] = true;

    return false;
}
//-------------------------------------------------------------------------
bool rex::engine::ImGUILayer::onMouseButtonReleasedEvent(events::MouseUp& e)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[(int)e.getButton()] = false;

    return false;
}
//-------------------------------------------------------------------------
bool rex::engine::ImGUILayer::onMouseMovedEvent(events::MouseMove& e)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = { (float)e.getScreenPosition().x, (float)e.getScreenPosition().y };

    return false;
}
//-------------------------------------------------------------------------
bool rex::engine::ImGUILayer::onMouseScrolledEvent(events::MouseScroll& e)
{
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel += e.getScrollDelta();

    return false;
}

//-------------------------------------------------------------------------
bool rex::engine::ImGUILayer::onKeyPressedEvent(events::KeyDown& e)
{
    ImGuiIO& io = ImGui::GetIO();

    RX_TODO("Change to REX keycode!");
    const int8 c = *e.getKey();

    io.KeysDown[c] = true;

    io.KeyCtrl = io.KeysDown[RX_KEY_LEFTCONTROL] || io.KeysDown[RX_KEY_RIGHTCONTROL];
    io.KeyShift = io.KeysDown[RX_KEY_LEFTSHIFT] || io.KeysDown[RX_KEY_RIGHTSHIFT];
    io.KeyAlt = io.KeysDown[RX_KEY_LEFTALT] || io.KeysDown[RX_KEY_RIGHTALT];
    io.KeySuper = io.KeysDown[RX_KEY_LEFTSUPER] || io.KeysDown[RX_KEY_RIGHTSUPER];
    return false;
}
//-------------------------------------------------------------------------
bool rex::engine::ImGUILayer::onKeyReleasedEvent(events::KeyUp& e)
{
    ImGuiIO& io = ImGui::GetIO();

    RX_TODO("Change to REX keycode!");
    const int8 c = *e.getKey(); // This is a WIN32 key code.

    io.KeysDown[c] = false;

    return false;
}
//-------------------------------------------------------------------------
bool rex::engine::ImGUILayer::onKeyTypedEvent(events::KeyTyped& e)
{
    ImGuiIO& io = ImGui::GetIO();

    const int8 c = *e.getKey();

    if (c > 0 && c < 0x10000)
        io.AddInputCharacterUTF16((unsigned short)c);

    return false;
}

//-------------------------------------------------------------------------
bool rex::engine::ImGUILayer::onWindowResizedEvent(events::WindowResize& e)
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = { (float)e.getWidth(), (float)e.getHeight() };
    io.DisplayFramebufferScale = { 1.0f, 1.0f };

#ifdef _OPENGL

    glViewport(0, 0, e.getWidth(), e.getHeight());

#endif

    return false;
}

