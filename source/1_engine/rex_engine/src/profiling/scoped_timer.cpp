#include "rex_engine/profiling/scoped_timer.h"

#include "rex_engine/filesystem/path.h"
#include "rex_engine/filesystem/vfs.h"
#include "rex_engine/filesystem/file.h"
#include "rex_std/mutex.h"

#include "rex_engine/memory/memory_tracking.h"
#include "rex_engine/task_system/task_system.h"

#include "rex_engine/diagnostics/log.h"

namespace rex
{
	DEFINE_LOG_CATEGORY(LogProfiler);

	class ProfilingSession
	{
	public:
		ProfilingSession()
		{
			m_filepath = rex::path::join(rex::vfs::current_session_root(), "profile_result.json");
			write_header();
		}

		~ProfilingSession()
		{
			write_footer();
		}

		void write_result(const ProfilingResult& result)
		{	
			static rsl::stringstream ss;
			static rsl::string source_loc;
			static rsl::string result_str;

			ss.str(rsl::string(""));
			result_str.clear();

			source_loc.assign(result.source_location.file_name());
			source_loc.replace("\\", "/");

			ss << ",{";
			ss << "\"cat\":\"function\",";
			ss << "\"dur\":" << (result.elapsed_time.count()) << ',';
			ss << "\"name\":\"" << result.name << "\",";
			ss << "\"ph\":\"X\",";
			ss << "\"pid\":0,";
			ss << "\"tid\":" << result.thread_id << ",";
			ss << "\"ts\":" << result.start_since_epoch.count() << ",";
			ss << "\"args\":";
			ss << "{";
			ss << "\"File\": \"" << source_loc << "\",";
			ss << "\"Line\": " << result.source_location.line();
			ss << "}";
			ss << "}\n";

			result_str = ss.str();

			file::append_line(m_filepath, result_str);
		}

	private:
		void write_header()
		{
			static rsl::string header;
			if (header.empty())
			{
				header += "{\n";
				header += "\"otherData\": {},\n";
				header += "\"displayTimeUnit\": \"us\",\n"; // if the profile timing would change, make sure this is changed too
				header += "\"traceEvents\":[{}\n";
			}

			file::append_line(m_filepath, header);
		}
		void write_footer()
		{
			static rsl::string footer;
			if (footer.empty())
			{
				footer += "]}";
			}

			file::append_line(m_filepath, footer);
		}

	private:
		rsl::string m_filepath;
	};

	void new_profile_result(const ProfilingResult& result)
	{
		static ProfilingSession session{};
		session.write_result(result);
	}

	ScopedTimer::ScopedTimer(rsl::string_view name, rsl::source_location sourceLoc)
		: m_name(name)
		, m_has_stopped(false)
		, m_source_location(sourceLoc)
	{
		m_start_timepoint = rsl::chrono::steady_clock::now();
	}

	ScopedTimer::~ScopedTimer()
	{
		if (!m_has_stopped)
		{
			stop();
		}
	}

	rsl::string_view ScopedTimer::name() const
	{
		return m_name;
	}

	rsl::source_location ScopedTimer::source_location() const
	{
		return m_source_location;
	}

	rsl::chrono::nanoseconds ScopedTimer::stop()
	{
		auto end = rsl::chrono::steady_clock::now();
		auto high_res_start = floating_point_micro_seconds(m_start_timepoint.time_since_epoch());
		auto elapsed_time = rsl::chrono::time_point_cast<rsl::chrono::nanoseconds>(end).time_since_epoch() - rsl::chrono::time_point_cast<rsl::chrono::nanoseconds>(m_start_timepoint).time_since_epoch();

		new_profile_result(
			{
				rsl::big_stack_string(m_name),
				high_res_start,
				elapsed_time,
				rsl::this_thread::get_id(),
				m_source_location
			});

		m_has_stopped = true;
		return elapsed_time;
	}
}