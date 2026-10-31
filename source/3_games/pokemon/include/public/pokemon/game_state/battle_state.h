#pragma once

#include "pokemon/state_task.h"

namespace pokemon
{
	class BattleState : public StateTask
	{
	public:
		BattleState();

		void begin();
		GameState tick(s32 frameIdx);
		void end();

	private:

	};
}