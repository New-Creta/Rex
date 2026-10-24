#include "pokemon/game_state/battle_state.h"

namespace pokemon
{
	BattleState::BattleState()
		: StateTask(
			[this]() { begin(); },
			[this](s32 frameIdx) { return tick(frameIdx); },
			[this]() { end(); }
		)
	{

	}

	void BattleState::begin()
	{

	}
	GameLoopState BattleState::tick(s32 frameIdx)
	{

	}
	void BattleState::end()
	{

	}
}