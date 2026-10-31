#pragma once

#include "pokemon/state_task.h"

namespace pokemon
{
	class BlackBordersState : public StateTask
	{
	public:
		BlackBordersState();

		void begin();
		GameState tick(s32 frameIdx);
		void end();

	private:

	};
}