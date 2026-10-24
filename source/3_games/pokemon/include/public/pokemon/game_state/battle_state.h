#pragma once

#include "pokemon/state_task.h"

namespace pokemon
{
	class BattleState : public StateTask
	{
	public:
		BattleState();

		void begin();
		GameLoopState tick(s32 frameIdx);
		void end();

	private:

	};
}