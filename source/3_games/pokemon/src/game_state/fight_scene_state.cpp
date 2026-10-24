#include "pokemon/game_state/fight_scene_state.h"

namespace pokemon
{
	FightSceneState::FightSceneState()
		: StateTask(
			[this]() { begin(); },
			[this](s32 frameIdx) { return tick(frameIdx); },
			[this]() { end(); }
		)
	{

	}

	void FightSceneState::begin()
	{

	}
	GameLoopState FightSceneState::tick(s32 frameIdx)
	{

	}
	void FightSceneState::end()
	{

	}
}