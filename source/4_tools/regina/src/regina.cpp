#include "regina/regina.h"

#include "regina/project.h"
#include "regina/content_manager.h"
#include "regina/project_manager.h"
#include "regina/widgets/create_project_widget.h"
#include "regina/widgets/main_editor_widget.h"

#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/event_system/events/app/quit_app.h"
#include "rex_engine/event_system/event_system.h"

DEFINE_LOG_CATEGORY(LogRegina);

#include "imgui/imgui.h"

namespace regina
{
	Regina::Regina(rsl::unique_ptr<Project> project)
		: m_project(rsl::move(project))
	{
		m_content_manager = rsl::make_unique<ContentManager>();

		if (m_project)
		{
			spawn_main_widget();
		}
		else
		{
			spawn_create_project_widget();
		}
	}
	Regina::~Regina() = default;

	void Regina::update()
	{
		REX_ASSERT_X(m_active_widget, "No active widget in the editor");

		if (m_active_widget->update())
		{
			m_active_widget->close();
		}
	}

	// Initialization
	void Regina::spawn_main_widget()
	{
		REX_ASSERT_X(m_project, "Cannot spawn the main widget if we don't have a project. project is null");

		m_active_widget = rsl::make_unique<MainEditorWidget>();

		m_content_manager->add_content(rex::vfs::project_root());
	}

	// Project management
	void Regina::create_new_project(rsl::string_view projectName)
	{
		REX_INFO(LogRegina, "Creating new project with name \"{}\"", projectName);
		m_project = project_manager::create_new(projectName);
	}
	void Regina::spawn_create_project_widget()
	{
		m_active_widget = rsl::make_unique<CreateProjectWidget>(
			[this](rsl::string_view projectName)
			{
				create_new_project(projectName);
			}
		);

		m_active_widget->set_callback(WidgetEvent::Close, [this]() { spawn_main_widget(); });
	}
}