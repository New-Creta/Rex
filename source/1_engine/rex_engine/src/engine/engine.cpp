#include "rex_engine/engine/engine.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/system/process.h"
#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/engine/globals.h"
#include "rex_engine/text_processing/json.h"

namespace rex
{
	rsl::string init_project_name(rsl::string_view processPath)
	{
		// assign from commandline
		REX_ASSERT_X(rex::cmdline::instance(), "Trying to get project name before commandline is initialized");

		rsl::optional<rsl::string_view> project_name_arg = rex::cmdline::instance()->get_argument("project");

		rsl::string project_name;
		if (project_name_arg)
		{
			project_name.assign(project_name_arg.value());
			return project_name;
		}

		// search for a project name txt file
		// Need to check through the global accessor as it's possible this gets called from the ctor
		scratch_string module_json_path = path::find_in_parent("module.json", path::parent_path(processPath));
		if (!module_json_path.empty())
		{
			memory::Blob blob = file::read_file_abspath(module_json_path);
			json::json module_content = json::parse(blob);
			if (module_content.contains("project_name"))
			{
				project_name.assign(rsl::string_view(module_content["project_name"]));
				return project_name;
			}
		}

		// Get the project name from the exe
		rsl::string_view potential_project_name = path::stem(processPath);
		potential_project_name = potential_project_name.substr(0, potential_project_name.find('_')); // everything before the first underscore
		project_name.assign(potential_project_name);
		return project_name;
	}

	// The engine globals get initialized twice,
	// once as a minimal setup, so we can boot up the engine
	// next time it gets fully initialized
	// To avoid requerying the paths on second initialization
	// we store them as a local static, as they don't change anyway
	EnginePaths init_engine_paths()
	{
		EnginePaths paths;

		scratch_string process_path = current_process::path();

		paths.project_name = init_project_name(process_path);
		paths.data_root.assign(path::find_in_parent("data", path::parent_path(process_path)));
		paths.root.assign(path::parent_path(paths.data_root));
		paths.intermediate_data_root.assign(path::join(paths.root, ".rex"));
		path::join_to(paths.engine_root, paths.data_root, "rex");
		path::join_to(paths.project_root, paths.data_root, paths.project_name);
		path::join_to(paths.sessions_root, paths.data_root, "_sessions");
		path::join_to(paths.project_sessions_root, paths.sessions_root, paths.project_name);
		path::join_to(paths.current_session_root, paths.project_sessions_root, path::timepoint_for_filename(rsl::current_timepoint()));

		if (!directory::exists_abspath(paths.current_session_root))
		{
			directory::create(paths.current_session_root);
		}

		return paths;
	}
	EnginePaths& engine_paths()
	{
		static EnginePaths paths = init_engine_paths();
		return paths;
	}

	EngineGlobals::EngineGlobals(rsl::unique_ptr<ScratchAllocator> scratchAlloc, rsl::unique_ptr<SingleFrameAllocator> tempAlloc)
		: m_scratch_allocator(rsl::move(scratchAlloc))
		, m_single_frame_allocator(rsl::move(tempAlloc))
		, m_frame_info()
	{
	}

	void EngineGlobals::advance_frame()
	{
		m_frame_info.update();
		m_single_frame_allocator->reset();
	}

	void* EngineGlobals::scratch_alloc(s64 size)
	{
		return m_scratch_allocator->allocate(size);
	}
	void* EngineGlobals::scratch_realloc(void* ptr, s64 size)
	{
		return m_scratch_allocator->reallocate(ptr, size);
	}
	void EngineGlobals::scratch_free(void* ptr)
	{
		return m_scratch_allocator->deallocate(ptr);
	}
	bool EngineGlobals::is_scratch_alloc(void* ptr) const
	{
		return m_scratch_allocator->has_allocated_ptr(ptr);
	}
	s64 EngineGlobals::scratch_buffer_size() const
	{
		return m_scratch_allocator->buffer_size();
	}

	void* EngineGlobals::temp_alloc(s64 size)
	{
		return m_single_frame_allocator->allocate(size);
	}
	void* EngineGlobals::temp_realloc(void* ptr, s64 count)
	{
		return m_single_frame_allocator->reallocate(ptr, count);
	}
	void EngineGlobals::temp_free(void* ptr)
	{
		return m_single_frame_allocator->deallocate(ptr);
	}
	bool EngineGlobals::is_temp_alloc(void* ptr) const
	{
		return m_single_frame_allocator->has_allocated_ptr(ptr);
	}
	s64 EngineGlobals::temp_buffer_size() const
	{
		return m_single_frame_allocator->buffer_size();
	}

	const FrameInfo& EngineGlobals::frame_info() const
	{
		return m_frame_info;
	}

	// Returns the current project's name
	rsl::string_view EngineGlobals::project_name() const
	{
		return engine_paths().project_name;
	}

	// Returns the root of all files
	rsl::string_view EngineGlobals::root() const
	{
		return engine_paths().root;
	}

	// Returns the root of all intermediate files, aka the .rex folder
	rsl::string_view EngineGlobals::intermediate_data_root() const
	{
		return engine_paths().intermediate_data_root;
	}

	// Returns the root directory where all data is loaded from
	rsl::string_view EngineGlobals::data_root() const
	{
		return engine_paths().data_root;
	}

	// Returns the root directory of the engine files
	rsl::string_view EngineGlobals::engine_root() const
	{
		return engine_paths().engine_root;
	}

	// Returns the root directory of the current project
	rsl::string_view EngineGlobals::project_root() const
	{
		return engine_paths().project_root;
	}

	// Returns the root for all sessions data
	rsl::string_view EngineGlobals::sessions_root() const
	{
		return engine_paths().sessions_root;
	}

	// Returns the root for all sessions data of this project
	rsl::string_view EngineGlobals::project_sessions_root() const
	{
		return engine_paths().project_sessions_root;
	}

	// Returns the root for all files outputed during this session run (eg. logs)
	rsl::string_view EngineGlobals::current_session_root() const
	{
		return engine_paths().current_session_root;
	}

	namespace engine
	{
		// the engine globals is a special case global
		// if it were a global variable, there's a chance 
		// the init func would gets called before the ctor of the global var
		// which would lead to unwanted results
		// therefore we have to store it as a local static
		globals::GlobalUniquePtr<EngineGlobals>& engine_globals()
		{
			static globals::GlobalUniquePtr<EngineGlobals> globals;
			return globals;
		}

		void init(globals::GlobalUniquePtr<EngineGlobals> globals)
		{
			engine_globals() = rsl::move(globals);
		}
		EngineGlobals* instance()
		{
			return engine_globals().get();
		}
		void shutdown()
		{
			engine_globals().reset();
		}
	}
}