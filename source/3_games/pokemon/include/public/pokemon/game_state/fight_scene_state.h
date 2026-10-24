#pragma once

#include "pokemon/state_task.h"

namespace pokemon
{
	class FightSceneState : public StateTask
	{
	public:
		FightSceneState();

		void begin();
		GameLoopState tick(s32 frameIdx);
		void end();

	private:

	};
}