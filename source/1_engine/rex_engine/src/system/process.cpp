#include "rex_engine/system/process.h"
#include "rex_engine/filesystem/path.h"

namespace rex
{
	namespace process
	{
		RunProcessResult run(rsl::string_view cmd)
		{
			rsl::string result;
			FILE* pipe = _popen(cmd.data(), "r");
			if (!pipe)
			{
				return RunProcessResult
				{
					rsl::string(""),
					404 // Return 404 like http error code when something doesn't exist
				};
			}

			char buffer[256]{};
			while (fgets(buffer, sizeof(buffer), pipe))
			{
				result += buffer;
			}
			result.resize(result.find_first_of('\0'));
			s32 return_code = _pclose(pipe);

			// trim trailing newline
			if (!result.empty() && (result.back() == '\n' || result.back() == '\r'))
			{
				result.pop_back();
			}

			return RunProcessResult
			{
				result,
				return_code
			};
		}
	}

	namespace current_process
	{
		scratch_string path()
		{
			path_stack_string filepath;
			path(filepath.data(), filepath.max_size());
			filepath.reset_null_termination_offset();

			return scratch_string(filepath);
		}
	}
}