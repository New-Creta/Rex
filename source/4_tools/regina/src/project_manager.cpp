#include "regina/project_manager.h"

#include "rex_engine/diagnostics/log.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"

#include "rex_std/bonus/utility/version.h"

namespace regina
{
	namespace project_manager
	{
    DEFINE_LOG_CATEGORY(ProjectLoader);

    rsl::version g_current_project_file_version(0, 0, 1);

    rsl::string create_project_filename(rsl::string_view projectName)
    {
      rsl::string project_filename(projectName);
      rsl::string_view project_extension = ".rexproj";

      // don't add the extension if it's already added
      if (!projectName.ends_with(project_extension))
      {
        project_filename += project_extension;
      }

      return project_filename;
    }
    rsl::string create_project_filepath(rsl::string_view projectName)
    {
      rsl::string project_filename = create_project_filename(projectName);
      rsl::string project_filepath(rex::vfs::abs_path(rex::path::join(projectName, project_filename)));

      return project_filepath;
    }

    rsl::unique_ptr<Project> create_new(rsl::string_view projectName)
    {
      if (projectName.empty())
      {
        REX_ERROR(ProjectLoader, "No project name specified");
        return nullptr;
      }

      rsl::string project_filepath = create_project_filepath(projectName);

      if (rex::vfs::exists(project_filepath))
      {
        REX_WARN(ProjectLoader, "{} already exists at {}, cannot create as new project", projectName, project_filepath);
        return load_from_disk(projectName);
      }

      rex::vfs::create_dirs(rex::path::dir_name(project_filepath));
      rex::vfs::create_file(project_filepath);

      rex::json::json new_project_content{};
      new_project_content["name"] = projectName;
      new_project_content["version"] = rsl::to_string(g_current_project_file_version);

      s32 indent = 4;
      rsl::string json_dump = new_project_content.dump(indent);
      rex::vfs::write_to_file(project_filepath, json_dump, rex::vfs::AppendToFile::no);

      return rsl::make_unique<Project>(projectName);
    }
		rsl::unique_ptr<Project> load_from_disk(rsl::string_view projectName)
		{
      if (projectName.empty())
      {
        REX_ERROR(ProjectLoader, "No project name specfied");
        return nullptr;
      }

      rsl::string project_filepath = create_project_filepath(projectName);

      // verify that the project path actually exist
      if (!rex::vfs::is_file(project_filepath))
      {
        REX_ERROR(ProjectLoader, "Project \"{}\" does not exist", project_filepath);
        return nullptr;
      }

      auto project_json = rex::json::read_from_file(project_filepath);

      rsl::version version(project_json["version"]);

      if (version != g_current_project_file_version)
      {
        REX_WARN(ProjectLoader, "project version {} of project with name \"{}\" does not match expected version {}. Some functionality may be impacted", version, projectName, g_current_project_file_version);
      }

      rsl::string_view name = project_json["name"];

      return rsl::make_unique<Project>(name);
		}
	}
}