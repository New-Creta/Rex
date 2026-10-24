#pragma once

#include "pokemon/state_task.h"

namespace pokemon
{
	class CopyrightState : public StateTask
	{
	public:
		CopyrightState();

		void begin();
		GameLoopState tick(s32 frameIdx);
		void end();

	private:

	};
}