#pragma once

#include "pokemon/state_task.h"

namespace pokemon
{
	class StartMenuState : public StateTask
	{
	public:
		StartMenuState();

		void begin();
		GameLoopState tick(s32 frameIdx);
		void end();

	private:

	};
}