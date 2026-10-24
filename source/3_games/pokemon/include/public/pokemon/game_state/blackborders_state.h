#pragma once

#include "pokemon/state_task.h"

namespace pokemon
{
	class BlackBordersState : public StateTask
	{
	public:
		BlackBordersState();

		void begin();
		GameLoopState tick(s32 frameIdx);
		void end();

	private:

	};
}