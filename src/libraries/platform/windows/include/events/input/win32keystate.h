#pragma once

#include <Windows.h>

namespace rex
{
	namespace win32
	{
		union KeyState
		{
		public:
			KeyState(LPARAM lparam);

		public:
			size_t data;

// Disabling this warning because it has to be nameless.
#pragma warning (push)
#pragma warning (disable : 4201)
			struct
			{
				unsigned repeat_count : 16;
				unsigned scan_code : 8;
				unsigned extended : 1;
				unsigned reserved : 4; // do not use
				unsigned context : 1;
				unsigned previous_state : 1;
				unsigned transition_state : 1;
			};
#pragma warning (pop)
		};
	}
}
