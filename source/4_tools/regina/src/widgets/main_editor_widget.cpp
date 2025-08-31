#include "regina/widgets/main_editor_widget.h"

#include "regina/widgets/content_browser_widget.h"

#include "rex_engine/event_system/event_system.h"
#include "rex_engine/event_system/events/app/quit_app.h"

#include "rex_engine/engine/asset_db.h"
#include "rex_engine/gfx/imgui/imgui_utils.h"
#include "rex_engine/gfx/imgui/imgui_scoped_widget.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/directory.h"
#include "rex_engine/text_processing/json.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

DEFINE_LOG_CATEGORY(LogMainEditor);

namespace regina
{
	// Example of how the main editor widget could look
	// +--------------+-------------------------------------------------+-------------+
	// |							| 																								| 					  |
	// |							|  																								| 					  |
	// |							|  																								| 					  |
	// |	properties  |  							viewport													| 	details	  |
	// |							|  																								| 					  |
	// |							|  																								| 					  |
	// |							|  																								| 					  |
	// +----------+---+-------------------------------------------------+-------------+
	// |  folder	|																																	  |
	// | hiearchy |							         content browser															|
	// |				  |																																	  |
	// |				  |																																	  |
	// +----------+-------------------------------------------------------------------+


	MainEditorWidget::MainEditorWidget()
		: m_show_imgui_demo(false)
		, m_show_imgui_style_editor(false)
		, m_active_map(nullptr)
	{
		//ImGuiIO& io = ImGui::GetIO();
		//if (!rex::file::exists(rsl::string_view(io.IniFilename)))
		//{
			rex::scratch_string main_layout_settings = rex::path::join(rex::vfs::instance()->root(), "Regina", "main_editor_layout.ini");
			ImGui::LoadIniSettingsFromDisk(main_layout_settings.data());
		//}

		init_widget_creators();
		add_default_widgets();
	}

	void MainEditorWidget::set_active_map(rex::Map* map)
	{
		if (m_active_map == map)
		{
			return;
		}

		m_active_map = map;
		on_new_active_map();
	}

	bool MainEditorWidget::on_update()
	{
		update_widgets();
	
		return false;
	}
	void MainEditorWidget::on_draw()
	{
		draw_menu_bar();
		draw_docking_backpanel();
		draw_widgets();
		draw_imgui_widgets();
	}

	void MainEditorWidget::init_widget_creators()
	{
		m_widget_create_funcs.emplace(rsl::type_id<ContentBrowserWidget>(), [this]()
			{
				auto content_manager = rsl::make_unique<ContentBrowserWidget>();

				content_manager->on_open_in_editor([this](rsl::string_view fullpath) 
					{
						if (fullpath.contains("maps"))
						{
							rex::Map* new_map = rex::asset_db::instance()->load<rex::Map>(fullpath);
							if (new_map != nullptr)
							{
								m_active_map = new_map;
								on_new_active_map();
							}
						}
					});

				m_widgets.emplace_back(rsl::move(content_manager));
			});
		m_widget_create_funcs.emplace(rsl::type_id<Viewport>(), [this]()
			{
				auto viewport = rsl::make_unique<Viewport>(m_world_composer.tilemap(), nullptr);
				m_viewport = viewport.get();
				m_widgets.emplace_back(rsl::move(viewport));
			});
	}

	void MainEditorWidget::add_default_widgets()
	{
		create_widget<ContentBrowserWidget>();
		create_widget<Viewport>();
	}

	void MainEditorWidget::update_widgets()
	{
		for (auto& widget : m_widgets)
		{
			widget->update();
		}
	}

	void MainEditorWidget::draw_menu_bar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Quit"))
				{
					rex::event_system::instance()->enqueue_event(rex::QuitApp("Quit Pressed In Menu"));
				}
				if (ImGui::MenuItem("Show ImGui Demo", false, &m_show_imgui_demo))
				{
					m_show_imgui_demo = !m_show_imgui_demo;
				}
				if (ImGui::MenuItem("Show ImGui Style Editor", false, &m_show_imgui_style_editor))
				{
					m_show_imgui_style_editor = !m_show_imgui_style_editor;
				}

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
	void MainEditorWidget::draw_docking_backpanel()
	{
		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		{
			window_flags |= ImGuiWindowFlags_NoBackground;
		}

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Docking Backpanel", nullptr, window_flags);
		m_docking_backpanel = ImGui::GetCurrentWindow();
		ImGui::PopStyleVar();

		ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("Regina Dockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		ImGui::End();
	}
	void MainEditorWidget::draw_widgets()
	{
		for (auto& widget : m_widgets)
		{
			widget->draw();
		}

		if (auto widget = rex::imgui::ScopedWidget("Scene Hierachy"))
		{
			ImGui::Text("This is the scene hiearchy");
		}

		if (auto widget = rex::imgui::ScopedWidget("Properties"))
		{
			ImGui::Text("This is the properties panel");
		}
	}
	void MainEditorWidget::draw_imgui_widgets()
	{
		if (m_show_imgui_demo)
		{
			ImGui::ShowDemoWindow();
		}
		if (m_show_imgui_style_editor)
		{
			rex::imgui::ScopedWidget widget("ImGui Style Editor");
			ImGui::ShowStyleEditor();
		}
	}

	void MainEditorWidget::on_new_active_map()
	{
		if (!m_world_composer.is_map_in_tilemap(m_active_map))
		{
			m_world_composer.build_world(m_active_map);
		}

		m_viewport->set_tilemap(m_world_composer.tilemap());
		m_viewport->set_tileset(m_active_map->desc().map_header.blockset->tileset());

		// 3. Move the camera to the active map
		rsl::pointi32 pos_in_tilemap = m_world_composer.map_pos(m_active_map);
		m_viewport->set_camera_pos(pos_in_tilemap);
	}

	void MainEditorWidget::create_widget(rsl::type_id_t typeId)
	{
		if (!m_widget_create_funcs.contains(typeId))
		{
			REX_ERROR(LogMainEditor, "No creation func found for {}", typeId.name());
		}

		m_widget_create_funcs.at(typeId)();
	}

}