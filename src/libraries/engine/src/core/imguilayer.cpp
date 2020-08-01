#include "rex_engine_pch.h"

#include "core/imguilayer.h"
#include "core/keycodes.h"

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
    UNUSED_PARAM(event);
}

