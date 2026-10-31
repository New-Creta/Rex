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
	GameState BattleState::tick(s32 frameIdx)
	{
		return GameState::Battle;
	}
	void BattleState::end()
	{

	}
}