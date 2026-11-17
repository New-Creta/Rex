#pragma once

#include "rex_engine/frameinfo/frameinfo.h"
#include "rex_engine/engine/globals.h"

#include "rex_engine/memory/global_allocators/global_allocator.h"
#include "rex_engine/memory/allocators/circular_allocator.h"
#include "rex_engine/memory/allocators/stack_allocator.h"

#include "rex_std/bonus/string.h"

namespace rex
{
	// For documentation, see: ~/_docs/src/documentation/rex/rex_globals.md
	struct EnginePaths
	{
		rsl::string project_name;
		rsl::string root;
		rsl::string intermediate_data_root;
		rsl::string data_root;
		rsl::string engine_root;
		rsl::string project_root;
		rsl::string sessions_root;
		rsl::string project_sessions_root;
		rsl::string current_session_root;
	};

	class EngineGlobals
	{
	public:
		using ScratchAllocator = TCircularAllocator<GlobalAllocator>;
		using SingleFrameAllocator = TStackAllocator<GlobalAllocator>;

		EngineGlobals(rsl::unique_ptr<ScratchAllocator> scratchAlloc, rsl::unique_ptr<SingleFrameAllocator> tempAlloc);

		void advance_frame();

		template <typename T>
		T* scratch_alloc()
		{
			return m_scratch_allocator->allocate<T>();
		}
		void* scratch_alloc(s64 size);
		void* scratch_realloc(void* ptr, s64 size);
		void scratch_free(void* ptr);
		bool is_scratch_alloc(void* ptr) const;
		s64 scratch_buffer_size() const;

		template <typename T>
		T* temp_alloc()
		{
			return m_single_frame_allocator->allocate<T>();
		}
		void* temp_alloc(s64 size);
		void* temp_realloc(void* ptr, s64 size);
		void temp_free(void* ptr);
		bool is_temp_alloc(void* ptr) const;
		s64 temp_buffer_size() const;

		const FrameInfo& frame_info() const;

		// Returns the current project's name
		rsl::string_view project_name() const;

		// Returns the root of all files
		rsl::string_view root() const;

		// Returns the root of all intermediate files, aka the .rex folder
		rsl::string_view intermediate_data_root() const;

		// Returns the root directory where all data is loaded from
		rsl::string_view data_root() const;

		// Returns the root directory of the engine files
		rsl::string_view engine_root() const;

		// Returns the root directory of the current project
		rsl::string_view project_root() const;

		// Returns the root for all sessions data
		rsl::string_view sessions_root() const;

		// Returns the root for all sessions data of this project
		rsl::string_view project_sessions_root() const;

		// Returns the root for all files outputed during this session run (eg. logs)
		rsl::string_view current_session_root() const;

	private:
		// An allocator used for temp memory. deallocation isn't tracked. Memory may or may not last more than 1 frame
		rsl::unique_ptr<ScratchAllocator> m_scratch_allocator;

		// An allocator used for memory that's used within a single frame. It gets reset at the beginning of the frame
		rsl::unique_ptr<SingleFrameAllocator> m_single_frame_allocator;

		FrameInfo m_frame_info;
	};

	namespace engine
	{
		void init(globals::GlobalUniquePtr<EngineGlobals> globals);
		EngineGlobals* instance();
		void shutdown();
	}
}