#pragma once

#include "pokemon/state_task.h"

namespace pokemon
{
	class FightSceneState : public StateTask
	{
	public:
		FightSceneState();

		void begin();
		GameState tick(s32 frameIdx);
		void end();

	private:

	};
}