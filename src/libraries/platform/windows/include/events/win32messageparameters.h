#pragma once

namespace rex
{
	namespace win32
	{
		struct MessageParameters
		{
			UINT   msg;

			WPARAM wparam;
			LPARAM lparam;
		};
	}
}