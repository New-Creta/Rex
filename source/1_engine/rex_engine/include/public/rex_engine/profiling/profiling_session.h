#pragma once

#include "rex_std/bonus/string.h"
#include "rex_std/chrono.h"
#include "rex_std/thread.h"
#include "rex_std/source_location.h"

#include "rex_engine/engine/globals.h"
#include "rex_engine/engine/types.h"
#include "rex_engine/engine/defines.h"

#include "rex_engine/profiling/timer.h"

namespace rex
{
	using floating_point_nano_seconds = rsl::chrono::duration<f64, rsl::nano>;

	struct ProfilingResult
	{
		rsl::big_stack_string name;

		floating_point_nano_seconds start_since_epoch;
		rsl::chrono::nanoseconds elapsed_time;
		rsl::thread::id thread_id;
		rsl::source_location source_location;
	};

	class ProfilingSession
	{
	public:
		ProfilingSession();
		~ProfilingSession();

		void write_result(const ProfilingResult& result);

	private:
		void write_header();
		void write_footer();
	private:
		rsl::string m_filepath;
	};

	// A profiling timer will track time from its construction until its destruction
	// and report the result to the profiling session
	class ProfilingTimer
	{
	public:
		ProfilingTimer(rsl::string_view name, rsl::source_location sourceLoc = rsl::source_location::current());
		~ProfilingTimer();

	private:
		Timer m_timer;
		rsl::source_location m_source_location;
	};

	namespace profiling_session
	{
		void init(globals::GlobalUniquePtr<ProfilingSession> profilingSession);
		ProfilingSession* instance();
		void shutdown();
	}
}

#define REX_PROFILE_SCOPE(name) const rex::ProfilingTimer REX_ANONYMOUS_VARIABLE(timer)(name)
#define REX_PROFILE_FUNCTION() REX_PROFILE_SCOPE(REX_FUNC_SIG)