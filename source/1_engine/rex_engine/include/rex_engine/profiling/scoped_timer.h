#pragma once

#include "rex_std/bonus/string.h"
#include "rex_std/chrono.h"
#include "rex_std/thread.h"
#include "rex_std/source_location.h"

#include "rex_engine/engine/types.h"
#include "rex_engine/engine/defines.h"

namespace rex
{
	using floating_point_micro_seconds = rsl::chrono::duration<f64, rsl::micro>;

	struct ProfilingResult
	{
		rsl::big_stack_string name;

		floating_point_micro_seconds start_since_epoch;
		rsl::chrono::nanoseconds elapsed_time;
		rsl::thread::id thread_id;
		rsl::source_location source_location;
	};

	class ScopedTimer
	{
	public:
		ScopedTimer(rsl::string_view name, rsl::source_location sourceLoc = rsl::source_location::current());
		~ScopedTimer();

		rsl::string_view name() const;
		rsl::source_location source_location() const;

		rsl::chrono::nanoseconds stop();

	private:
		rsl::string_view m_name;
		rsl::chrono::time_point<rsl::chrono::steady_clock> m_start_timepoint;
		bool m_has_stopped;
		rsl::source_location m_source_location;
	};
}

#define REX_PROFILE_SCOPE(name) const rex::ScopedTimer REX_ANONYMOUS_VARIABLE(timer)(name)
#define REX_PROFILE_FUNCTION() REX_PROFILE_SCOPE(REX_FUNC_SIG)