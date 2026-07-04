#pragma once

#include "rex_engine/engine/defines.h"
#include "rex_engine/engine/types.h"

#include "rex_engine/memory/pointer_math.h"

#include "pokemon/pokedex_constants.h"

struct SpriteStateData1
{
	s8 picture_id;											// - 0: picture ID (fixed, loaded at map init)
	s8 movement_status;									// - 1: movement status (0: uninitialized, 1: ready, 2: delayed, 3: moving)
	s8 image_idx;												// - 2: sprite image index (changed on update, $ff if off screen, includes facing direction, progress in walking animation and a sprite-specific offset)
	s8 y_screen_pos_delta;							// - 3: Y screen position delta (-1,0 or 1; added to Y pixels on each walking animation update)
	s8 y_screen_position;								// - 4: Y screen position (in pixels, always 4 pixels above grid which makes sprites appear to be in the center of a tile)
	s8 x_screen_position_delta;					// - 5: X screen position delta (-1,0 or 1; added to field X pixels on each walking animation update)
	s8 x_screen_position;								// - 6: X screen position (in pixels, snaps to grid if not currently walking)
	s8 intra_animation_frame_counter;		// - 7: intra-animation-frame counter (counting upwards to 4 until animation frame counter is incremented)
	s8 animation_frame_counter;					// - 8: animation frame counter (increased every 4 updates, hold four states (totalling to 16 walking frames)
	s8 face_direction;									// - 9: facing direction ($0: down, $4: up, $8: left, $c: right)
	s8 adjusted_y_coordinate;						// - A: adjusted Y coordinate
	s8 adjusted_x_coordinate;						// - B: adjusted X coordinate
	s8 direction_of_collision;					// - C: direction of collision
	s8 unusued_1;
	s8 unusued_2;
	s8 unusued_3;
};

struct SpriteStateData2
{
	s8 walk_animation_counter;			// - 0: walk animation counter (counting from $10 backwards when moving)
	s8 unused_1;										// - 1:
	s8 y_displacement;							// - 2: Y displacement (initialized at 8, supposed to keep moving sprites from moving too far, but bugged)
	s8 x_displacement;							// - 3: X displacement (initialized at 8, supposed to keep moving sprites from moving too far, but bugged)
	s8 y_position;									// - 4: Y position (in 2x2 tile grid steps, topmost 2x2 tile has value 4)
	s8 x_position;									// - 5: X position (in 2x2 tile grid steps, leftmost 2x2 tile has value 4)
	s8 movement_byte;								// - 6: movement byte 1 (determines whether a sprite can move, $ff:not moving, $fe:random movements, others unknown)
	s8 is_on_grass;									// - 7: (?) (set to $80 when in grass, else $0; may be used to draw grass above the sprite)
	s8 delay_until_next_movement;		// - 8: delay until next movement (counted downwards, movement status is set to ready if reached 0)
	s8 original_facing_direction;		// - 9: original facing direction (backed up by DisplayTextIDInit, restored by CloseTextDisplay)
	s8 unused_2;										// - A
	s8 unused_3;										// - B
	s8 unused_4;										// - C
	s8 picture_id;									// - D: picture ID
	s8 sprite_image_base_offset;		// - E: sprite image base offset (in video ram, player always has value 1, used to compute sprite image index)
	s8 unused_5;										// - F
};

enum class Wd730
{
	// bit 0: NPC sprite being moved by script
	NpcSpriteMovedByScript = BIT(0),
	// bit 5: ignore joypad input
	IgnoreJoypadInput = BIT(5),
	// bit 6: print text with no delay between each letter
	PrintTextWithNoDelayBetweenLetters = BIT(6),
	// bit 7: set if joypad states are being simulated in the overworld or an NPC's movement is being scripted
	SimulateJoypadStates = BIT(7)
};

enum class PlayerMovementFlags : u8
{
	// check if the player is standing on a door and make him walk down a step if so
	IsOnDoor = BIT(0),

	// the player is currently stepping down from a door
	IsSteppingDownFromDoor = BIT(1),

	// standing on a warp
	IsOnWarp = BIT(2),

	// jumping down a ledge / fishing animation
	IsJumpingDownLedgeOrFishing = BIT(6),

	// player sprite spinning due to spin tiles (Rocket hideout / Viridian Gym)
	IsSpinning = BIT(7)
};

enum class Cd60
{
	// bit 0: is player engaged by trainer(to avoid being engaged by multiple trainers simultaneously)
	IsPlayerEngagedByTrainer = BIT(0),
	// bit 1: boulder dust animation(from using Strength) pending
	BoulderDustAnimationPending = BIT(1),
	// bit 3: using generic PC
	UsingGenericPc = BIT(3),
	// bit 5: don't play sound when A or B is pressed in menu
	DontPlaySoundwhenAOrBIsPressedInMenu = BIT(5),
	// bit 6: tried pushing against boulder once(you need to push twice before it will move)
	TriedPushingAgainstBoulderOnce = BIT(6)
};

struct MuteAudioAndPauseMusicData
{
	s8 bit0 : 1;
	s8 bit1 : 1;
	s8 bit2 : 1;
	s8 bit3 : 1;
	s8 bit4 : 1;
	s8 bit5 : 1;
	s8 bit6 : 1;
	s8 mute_all : 1;
};

const s8 NUM_MUSIC_CHANS = 4;
const s8 NUM_NOISE_CHANS = 4;
const s8 NUM_CHANNELS = NUM_MUSIC_CHANS + NUM_NOISE_CHANS;
const s8 NUM_SPRITE_OAM_STRUCTS = 40;
const s8 TILE_WIDTH = 8;
const s8 SCREEN_WIDTH = 20;
const s8 SCREEN_HEIGHT = 18;
const s8 SCREEN_WIDTH_PX = SCREEN_WIDTH * TILE_WIDTH;
const s8 SCREEN_HEIGHT_PX = SCREEN_HEIGHT * TILE_WIDTH;
const s8 LEN_1BPP_TILE = 1 * TILE_WIDTH; // bytes
const s8 LEN_2BPP_TILE = 2 * TILE_WIDTH; // bytes
const s8 BG_MAP_WIDTH = 32;
const s8 BG_MAP_HEIGHT = 32;
const s8 PARTY_LENGTH = 6;
const s8 HOF_MON = 0x10;
const s8 HOF_TEAM = HOF_MON - PARTY_LENGTH;
const s8 HOF_TEAM_CAPACITY = 50;
const s8 NAME_LENGTH = 11;
const s8 ITEM_NAME_LENGTH = 13;
const s8 NUM_BOXES = 12;
const s8 NUM_MOVES = 4;
const s8 NUM_CITY_MAPS = 11;
const s8 NUM_BADGES = 8;
const s8 NUM_STATS = 5;
const s8 NUM_TMS = 50;
const s8 NUM_HMS = 5;
const s8 BAG_ITEM_CAPACITY = 20;
const s8 PC_ITEM_CAPACITY = 50;
const s16 NUM_EVENTS = 0xA00;
const s8 MONS_PER_BOX = 20;

constexpr s32 flag_array_size(s32 input)
{
	return (input + 7) / 8;
}

struct AudioRam
{
	s8 wUnusedC000;

	s8 wSoundID;

	MuteAudioAndPauseMusicData wMuteAudioAndPauseMusic;

	s8 wDisableChannelOutputWhenSfxEnds;
	s8 wStereoPanning;
	s8 wSavedVolume;
	s8 wChannelCommandPointers[NUM_CHANNELS * 2];
	s8 wChannelReturnAddresses[NUM_CHANNELS * 2];

	s8 wChannelSoundIDs[NUM_CHANNELS];

	s8 wChannelFlags1[NUM_CHANNELS];
	s8 wChannelFlags2[NUM_CHANNELS];
	s8 wChannelDutyCycles[NUM_CHANNELS];
	s8 wChannelDutyCyclePatterns[NUM_CHANNELS];

	// reloaded at the beginning of a note.counts down until the vibrato begins.
	s8 wChannelVibratoDelayCounters[NUM_CHANNELS];
	s8 wChannelVibratoExtents[NUM_CHANNELS];

	// high nybble is rate(counter reload value) and low nybble is counter.
	// time between applications of vibrato.
	s8 wChannelVibratoRates[NUM_CHANNELS];
	s8 wChannelFrequencyLowBytes[NUM_CHANNELS];

	// delay of the beginning of the vibrato from the start of the note
	s8 wChannelVibratoDelayCounterReloadValues[NUM_CHANNELS];
	s8 wChannelPitchSlideLengthModifiers[NUM_CHANNELS];
	s8 wChannelPitchSlideFrequencySteps[NUM_CHANNELS];
	s8 wChannelPitchSlideFrequencyStepsFractionalPart[NUM_CHANNELS];
	s8 wChannelPitchSlideCurrentFrequencyFractionalPart[NUM_CHANNELS];
	s8 wChannelPitchSlideCurrentFrequencyHighBytes[NUM_CHANNELS];
	s8 wChannelPitchSlideCurrentFrequencyLowBytes[NUM_CHANNELS];
	s8 wChannelPitchSlideTargetFrequencyHighBytes[NUM_CHANNELS];
	s8 wChannelPitchSlideTargetFrequencyLowBytes[NUM_CHANNELS];

	// Note delays are stored as 16 - bit fixed - point numbers where the integer part
	// is 8 bits and the fractional part is 8 bits.
	s8 wChannelNoteDelayCounters[NUM_CHANNELS];
	s8 wChannelLoopCounters[NUM_CHANNELS];
	s8 wChannelNoteSpeeds[NUM_CHANNELS];
	s8 wChannelNoteDelayCountersFractionalPart[NUM_CHANNELS];
	s8 wChannelOctaves[NUM_CHANNELS];

	// also includes fade for hardware channels that support it
	s8 wChannelVolumes[NUM_CHANNELS];

	s8 wMusicWaveInstrument;
	s8 wSfxWaveInstrument;
	s16 wMusicTempo;
	s16 wSfxTempo;
	s16 wSfxHeaderPointer;
	s8 wNewSoundID;
	s8 wAudioROMBank;
	s8 wAudioSavedROMBank;
	s8 wFrequencyModifier;
	s8 wTempoModifier;

	s8 padding[13];
};

struct SpriteStateData
{
	SpriteStateData1 player_sprite_data1;
	SpriteStateData2 player_sprite_data2;

	SpriteStateData1 sprite_data1[16];
	SpriteStateData2 sprite_data2[16];
};

struct OAMStruct
{
	s8 y_coord;
	s8 x_coord;
	s8 tile_id;
	s8 attributes;
};

struct OAMBuffer
{
	OAMStruct wShadowOAMSprite[NUM_SPRITE_OAM_STRUCTS];
};

struct Tilemap
{
	// buffer for tiles that are visible on screen (20 columns by 18 rows)
	s8 wTileMap[SCREEN_WIDTH * SCREEN_HEIGHT];

	union
	{
		// buffer for temporarily saving and restoring current screen's tiles
		// (e.g. if menus are drawn on top)
		s8 wTileMapBackup[SCREEN_WIDTH * SCREEN_HEIGHT];

		struct
		{
			// list of indexes to patch with SERIAL_NO_DATA_BYTE after transfer
			s8 wSerialPartyMonsPatchList[200];

			// list of indexes to patch with SERIAL_NO_DATA_BYTE after transfer
			s8 wSerialEnemyMonsPatchList[200];
		};
	};

	s8 padding[80];
};

struct OverworldMap
{
	union
	{
		s8 wOverworldMap[1300];
		s8 wTempPic[7 * 7 * LEN_2BPP_TILE];
	};
};

struct BoxStruct
{
	s8 species;
	s16 hp;
	s8 box_level;
	s8 status;
	union
	{
		s8 type;
		s8 type1;
	};
	s8 type2;
	s8 catch_rate;
	s8 moves[NUM_MOVES];
	s16 OTID;
	s8 exp[3];
	s16 hpexp;
	s16 attackexp;
	s16 defenseexp;
	s16 speedexp;
	s16 specialexp;
	s16 dvs;
	s8 pp[NUM_MOVES];

};

struct PartyStruct
{
	BoxStruct box_struct;
	s8 level;
	union
	{
		s16 stats;
		s16 max_hp;
	};
	s16 attack;
	s16 defense;
	s16 speed;
	s16 special;
};

struct BattleStruct
{
	s8 species;
	s16 hp;
	union
	{
		s8 party_pos;
		s8 box_level;
	};
	s8 status;
	union
	{
		s8 type;
		s8 type1;
	};
	s8 type2;
	s8 catch_rate;
	s8 moves[NUM_MOVES];
	s16 dvs;
	s8 level;
	union
	{
		s16 stats;
		s16 maxhp;
	};
	s16 attack;
	s16 defense;
	s16 speed;
	s16 special;
	s16 pp[NUM_MOVES];
};

struct MapConnectionStruct
{
	s8 connected_map;
	s16 connection_strip_src;
	s16 connection_strip_dest;
	s8 connection_strip_length;
	s8 connected_map_width;
	s8 connected_map_y_alignment;
	s8 connected_map_x_alignment;
	s16 connected_map_view_pointer;
};

struct WramData
{
	// the tiles of the row or column to be redrawn by RedrawRowOrColumn
	s8 wRedrawRowOrColumnSrcTiles[SCREEN_WIDTH * 2];

	// coordinates of the position of the cursor for the top menu item (id 0)
	s8 wTopMenuItemY;
	s8 wTopMenuItemX;

	// the id of the currently selected menu item
	// the top item has id 0, the one below that has id 1, etc.
	// note that the "top item" means the top item currently visible on the screen
	// add this value to [wListScrollOffset] to get the item's position within the list
	s8 wCurrentMenuItem;

	// the tile that was behind the menu cursor's current location
	s8 wTileBehindCursor;

	// id of the bottom menu item
	s8 wMaxMenuItem;

	// bit mask of keys that the menu will respond to
	s8 wMenuWatchedKeys;

	// id of previously selected menu item
	s8 wLastMenuItem;

	// It is mainly used by the party menu to remember the cursor position while the
	// menu isn't active.
	// It is also used to remember the cursor position of mon lists (for the
	// withdraw/deposit/release actions) in Bill's PC so that it doesn't get lost
	// when you choose a mon from the list and a sub-menu is shown. It's reset when
	// you return to the main Bill's PC menu.
	s8 wPartyAndBillsPCSavedMenuItem;

	// It is used by the bag list to remember the cursor position while the menu
	// isn't active.
	s8 wBagSavedMenuItem;

	// It is used by the start menu to remember the cursor position while the menu
	// isn't active.
	// The battle menu uses it so that the cursor position doesn't get lost when
	// a sub-menu is shown. It's reset at the start of each battle.
	s8 wBattleAndStartSavedMenuItem;

	s8 wPlayerMoveListIndex;

	// index in party of currently battling mon
	s8 wPlayerMonNumber;

	// the address of the menu cursor's current location within wTileMap
	s16 wMenuCursorLocation;

	s8 padding1[2];

	// how many times should HandleMenuInput poll the joypad state before it returns?
	s8 wMenuJoypadPollCount;

	// id of menu item selected for swapping (counts from 1) (0 means that no menu item has been selected for swapping)
	s8 wMenuItemToSwap;

	// offset of the current top menu item from the beginning of the list
	// keeps track of what section of the list is on screen
	s8 wListScrollOffset;

	// If non-zero, then when wrapping is disabled and the player tries to go past
	// the top or bottom of the menu, return from HandleMenuInput. This is useful for
	// menus that have too many items to display at once on the screen because it
	// allows the caller to scroll the entire menu up or down when this happens.
	s8 wMenuWatchMovingOutOfBounds;

	s8 wTradeCenterPointerTableIndex;

	s8 padding2[1];

	// destination pointer for text output
	// this variable is written to, but is never read from
	s16 wTextDest;

	// if non-zero, skip waiting for a button press after displaying text in DisplayTextID
	s8 wDoNotWaitForButtonPressAfterDisplayingText;

	union
	{
		struct
		{
			// the received menu selection is stored twice
			s16 wLinkMenuSelectionReceiveBuffer;
			s8 padding1[3];
			// the menu selection byte is stored twice before sending
			s16 wLinkMenuSelectionSendBuffer;
			s8 padding2[3];

			union
			{
				s8 wEnteringCableClub;
				s8 wLinkTimeoutCounter;
			};
		};
		struct
		{
			union
			{
				// temporary nybble used by Serial_ExchangeNybble
				s8 wSerialExchangeNybbleTempReceiveData;
				// the final received nybble is stored here by Serial_SyncAndExchangeNybble
				s8 wSerialSyncAndExchangeNybbleReceiveData;
			};
			// the final received nybble is stored here by Serial_ExchangeNybble
			s8 wSerialExchangeNybbleReceiveData;
			s8 padding1[3];
			// this nybble is sent when using Serial_SyncAndExchangeNybble or Serial_ExchangeNybble
			s8 wSerialExchangeNybbleSendData;
			s8 padding1[4];
			s16 wUnknownSerialCounter;
		};
	};

	union
	{
		// $00 = player mons
		// $01 = enemy mons
		s8 wWhichTradeMonSelectionMenu;
		// 0 = player's party
		// 1 = enemy party
		// 2 = current box
		// 3 = daycare
		// 4 = in-battle mon
		//
		// AddPartyMon uses it slightly differently.
		// If the lower nybble is 0, the mon is added to the player's party, else the enemy's.
		// If the entire value is 0, then the player is allowed to name the mon.
		s8 wMonDataLocation;
	};

	// set to 1 if you can go from the bottom to the top or top to bottom of a menu
	// set to 0 if you can't go past the top or bottom of the menu
	s8 wMenuWrappingEnabled;

	// whether to check for 180-degree turn (0 = don't, 1 = do)
	s8 wCheckFor180DegreeTurn;
	s8 padding3[1];

	s8 wMissableObjectIndex;

	s8 wPredefID;
	s8 wPredefHL;
	s8 wPredefDE;
	s8 wPredefBC;

	s8 wTrainerHeaderFlagBit;
	s8 padding4[1];

	// which NPC movement script pointer is being used
	// 0 if an NPC movement script is not running
	s8 wNPCMovementScriptPointerTableNum;

	// ROM bank of current NPC movement script
	s8 wNPCMovementScriptBank;

	s8 padding5[2];

	// This union spans 180 bytes.
	union Union1
	{
		s8 wVermilionDockTileMapBuffer[5 * BG_MAP_WIDTH + SCREEN_WIDTH];
		s8 wOaksAideRewardItemName[ITEM_NAME_LENGTH];
		s8 wElevatorWarpMaps[11 * 2];
		// List of bag items that has been filtered to a certain type of items,
		// such as drinks or fossils.
		s8 wFilteredBagItems[4];
		// Saved copy of OAM for the first frame of the animation to make it easy to
		// flip back from the second frame.
		s8 wMonPartySpritesSavedOAM[0x60];
		s8 wTrainerCardBlkPacket[0x40];
		s8 wHallOfFame[HOF_TEAM];
		s8 wNPCMovementDirections[180];

		struct
		{
			s8 wDexRatingNumMonsSeen;
			s8 wDexRatingNumMonsOwned;
			s8 wDexRatingText;
		};

		struct
		{
			// If a random number greater than this value is generated, then the player is
			// allowed to have three 7 symbols or bar symbols line up.
			// So, this value is actually the chance of NOT entering that mode.
			// If the slot is lucky, it equals 250, giving a 5/256 (~2%) chance.
			// Otherwise, it equals 253, giving a 2/256 (~0.8%) chance.
			s8 wSlotMachineSevenAndBarModeChance;
			s8 padding[2];
			// ROM back to return to when the player is done with the slot machine
			s8 wSlotMachineSavedROMBank;
			s8 padding1[166];
			s8 wLuckySlotHiddenObjectIndex;
		};

		struct
		{
			s8 wAnimationType;
			s8 padding1[29];
			s8 wAnimPalette;
		};

		struct
		{
			s8 padding1[60];

			// temporary buffer when swapping party mon data
			s8 wSwitchPartyMonTempBuffer[44]; // party_struct size
		};

		struct
		{
			s8 padding1[120];
			// this is the end of the joypad states
			// the list starts above this address and extends downwards in memory until here
			// overloaded with below labels
		};

		struct MiscBattleData
		{
			union
			{
				s8 wBoostExpByExpAll;
				s8 wUnusedCC5B;
			};

			s8 padding1[59];
			s8 wNPCMovementDirections2[10];
			// used in Pallet Town scripted movement
			s8 wNumStepsToTake;
			s8 padding1[48];
			s8 wRLEByteCount;

			union
			{
				s8 wParentMenuItem;
				// 0 = not added
				// 1 = added
				s8 wAddedToParty;
				// 1 flag for each party member indicating whether it can evolve
				// The purpose of these flags is to track which mons levelled up during the
				// current battle at the end of the battle when evolution occurs.
				// Other methods of evolution simply set it by calling TryEvolvingMon.
				s8 wCanEvolveFlags;
			};

			s8 wForceEvolution;
			// if [wAILayer2Encouragement] != 1, the second AI layer is not applied
			s8 wAILayer2Encouragement;
			s8 padding2[1];

			// current HP of player and enemy substitutes
			s8 wPlayerSubstituteHP;
			s8 wEnemySubstituteHP;

			// used for TestBattle (unused in non-debug builds)
			s8 wTestBattlePlayerSelectedMove;
			s8 padding3[1];

			// 0=regular, 1=mimic, 2=above message box (relearn, heal pp..)
			s8 wMoveMenuType;

			s8 wPlayerSelectedMove;
			s8 wEnemySelectedMove;
			s8 wLinkBattleRandomNumberListIndex;

			// number of times remaining that AI action can occur
			s8 wAICount;

			s8 padding4[2];

			s8 wEnemyMoveListIndex;

			// The enemy mon's HP when it was switched in or when the current player mon
			// was switched in, which was more recent.
			// It's used to determine the message to print when switching out the player mon.
			s8 wLastSwitchInEnemyMonHP;

			// total amount of money made using Pay Day during the current battle
			s8 wTotalPayDayMoney[3];

			s8 wSafariEscapeFactor;
			s8 wSafariBaitFactor;
			s8 padding5[1];

			s16 wTransformedEnemyMonOriginalDVs;
			s8 wMonIsDisobedient;
			s8 wPlayerDisabledMoveNumber;
			s8 wEnemyDisabledMoveNumber;

			s8 wInHandlePlayerMonFainted;
			s8 wPlayerUsedMove;
			s8 wEnemyUsedMove;

			s8 wEnemyMonMinimized;
			s8 wMoveDidntMiss;

			// flags that indicate which party members have fought the current enemy mon
			s8 wPartyFoughtCurrentEnemyFlags;

			// Whether the low health alarm has been disabled due to the player winning the
			// battle.
			s8 wLowHealthAlarmDisabled;

			s8 wPlayerMonMinimized;

			s8 padding6[13];

			union
			{
				// number of hits by enemy in attacks like Double Slap, etc.
				s8 wEnemyNumHits;
				// the amount of damage accumulated by the enemy while biding
				s16 wEnemyBideAccumulatedDamage;
			};

			s8 padding7[8];
		};
	};

	static_assert(sizeof(Union1) == 180, "Union1 should be 180 bytes in size");

	union
	{
		struct
		{
			s8 wInGameTradeGiveMonSpecies;
			s16 wInGameTradeTextPointerTablePointer;
			s8 wInGameTradeTextPointerTableIndex;
			s8 wInGameTradeGiveMonName[NAME_LENGTH];
			s8 wInGameTradeReceiveMonName[NAME_LENGTH];
			s8 wInGameTradeMonNick[NAME_LENGTH];
			s8 wInGameTradeReceiveMonSpecies;
		};

		struct
		{
			s8 wPlayerMonUnmodifiedLevel;
			s16 wPlayerMonUnmodifiedMaxHP;
			s16 wPlayerMonUnmodifiedAttack;
			s16 wPlayerMonUnmodifiedDefense;
			s16 wPlayerMonUnmodifiedSpeed;
			s16 wPlayerMonUnmodifiedSpecial;

			union
			{
				s8 wPlayerMonStatMods;
				s8 wPlayerMonAttackMod;
			};
			s8 wPlayerMonDefenseMod;
			s8 wPlayerMonSpeedMod;
			s8 wPlayerMonSpecialMod;
			s8 wPlayerMonAccuracyMod;
			s8 wPlayerMonEvasionMod;

			s8 padding1[2];
			s8 padding2[1];

			s8 wEnemyMonUnmodifiedLevel;
			s16 wEnemyMonUnmodifiedMaxHP;
			s16 wEnemyMonUnmodifiedAttack;
			s16 wEnemyMonUnmodifiedDefense;
			s16 wEnemyMonUnmodifiedSpeed;
			s16 wEnemyMonUnmodifiedSpecial;

			// stat modifiers for the enemy's current pokemon
			// value can range from 1 - 13 ($1 to $D)
			// 7 is normal
			s8 wEnemyMonAttackMod;
			s8 wEnemyMonDefenseMod;
			s8 wEnemyMonSpeedMod;
			s8 wEnemyMonSpecialMod;
			s8 wEnemyMonAccuracyMod;
			s8 wEnemyMonEvasionMod;
			s8 padding3[2];
		};

		struct
		{
			s8 padding1[30];
			s8 wEngagedTrainerClass;
			s8 wEngagedTrainerSet;
		};
	};

	s8 padding6[1];

	union
	{
		s8 wNPCMovementDirections2Index;
		s8 wUnusedCD37;

		// number of items in wFilteredBagItems list
		s8 wFilteredBagItemsCount;
	};

	// the next simulated joypad state is at wSimulatedJoypadStatesEnd plus this value minus 1
	// 0 if the joypad state is not being simulated
	s8 wSimulatedJoypadStatesIndex;
	// written to but nothing ever reads it
	s8 wUnusedCD39;
	// written to but nothing ever reads it
	s8 wUnusedCD3A;

	// mask indicating which real button presses can override simulated ones
	// XXX is it ever not 0?
	s8 wOverrideSimulatedJoypadStatesMask;

	s8 padding7[1];

	// This union spans 30 bytes.
	union Union2
	{
		// This struct is technically 30 bytes of data but due to alignment
		// it's bumped to 32 bytes
		struct
		{
			s8 wTradedPlayerMonSpecies;
			s8 wTradedEnemyMonSpecies;
			s8 padding[2];
			s8 wTradedPlayerMonOT[NAME_LENGTH];
			s16 wTradedPlayerMonOTID;
			s8 wTradedEnemyMonOT[NAME_LENGTH];
			s16 wTradedEnemyMonOTID;
		};

		struct
		{
			s8 wTradingWhichPlayerMon;
			s8 wTradingWhichEnemyMon;
			s8 wNameOfPlayerMonToBeTraded[NAME_LENGTH];
		};

		struct
		{
			// one byte for each falling object
			s8 wFallingObjectsMovementData[20];
		};

		struct
		{
			// array of the number of mons in each box
			s8 wBoxMonCounts[NUM_BOXES];
		};

		struct
		{
			s8 wPriceTemp[3]; // BCD
		};

		struct
		{
			// the current mon's field moves
			s8 wFieldMoves[NUM_MOVES];
			s8 wNumFieldMoves;
			s8 wFieldMovesLeftmostXCoord;
			s8 wLastFieldMoveID; // unused
		};

		struct
		{
			s8 wBoxNumString[3];
		};

		struct
		{
			// 0 = upper half (Y < 9)
			// 1 = lower half (Y >= 9)
			s8 wBattleTransitionCircleScreenQuadrantY;
			s8 wBattleTransitionCircleScreenQuadrantX;
		};

		struct
		{
			// after 1 row/column has been copied, the offset to the next one to copy from
			s16 wBattleTransitionCopyTilesOffset;
		};

		struct
		{
			// counts down from 7 so that every time 7 more tiles of the spiral have been
			// placed, the tile map buffer is copied to VRAM so that progress is visible
			s8 wInwardSpiralUpdateScreenCounter;
			s8 padding[9];
			// 0 = outward, 1 = inward
			s8 wBattleTransitionSpiralDirection;
		};

		struct
		{
			// multiplied by 16 to get the number of times to go right by 2 pixels
			s8 wSSAnneSmokeDriftAmount;

			// 0 = left half (X < 10)
			// 1 = right half (X >= 10)
			s8 wSSAnneSmokeX;
		};

		struct
		{
			union
			{
				s8 wHoFMonSpecies;
				s8 wHoFTeamIndex;
			};

			s8 wHoFPartyMonIndex;
			s8 wHoFMonLevel;
			// 0 = mon, 1 = player
			s8 wHoFMonOrPlayer;
			s8 wHoFTeamIndex2;
			s8 wHoFTeamNo;
		};

		struct
		{
			s8 wRivalStarterTemp;
			s8 wRivalStarterBallSpriteIndex;
		};

		struct
		{
			s8 wFlyAnimUsingCoordList;
			// $ff sentinel values at each end
			s8 wFlyLocationsList[NUM_CITY_MAPS + 2];
		};

		struct
		{
			s8 wWhichTownMapLocation;
			s8 wFlyAnimCounter;
			s8 wFlyAnimBirdSpriteImageIndex;
		};

		struct
		{
			s8 padding[1];
			// difference in X between the next ball and the current one
			s8 wHUDPokeballGfxOffsetX;
			s8 wHUDGraphicsTiles[3];
		};

		struct
		{
			//  the level of the mon at the time it entered day care
			s8 wDayCareStartLevel;
			s8 wDayCareNumLevelsGrown;
			s16 wDayCareTotalCost; // BCD
			s16 wDayCarePerLevelCost; // BCD (always $100)
		};

		struct
		{
			// which wheel the player is trying to stop
			// 0 = none, 1 = wheel 1, 2 = wheel 2, 3 or greater = wheel 3
			s8 wStoppingWhichSlotMachineWheel;

			s8 wSlotMachineWheel1Offset;
			s8 wSlotMachineWheel2Offset;
			s8 wSlotMachineWheel3Offset;

			// the OAM tile number of the upper left corner of the winning symbol minus 2
			s8 wSlotMachineWinningSymbol;
			s8 wSlotMachineWheel1BottomTile;
			s8 wSlotMachineWheel1MiddleTile;
			s8 wSlotMachineWheel1TopTile;
			s8 wSlotMachineWheel2BottomTile;
			s8 wSlotMachineWheel2MiddleTile;
			s8 wSlotMachineWheel2TopTile;
			s8 wSlotMachineWheel3BottomTile;
			s8 wSlotMachineWheel3MiddleTile;
			s8 wSlotMachineWheel3TopTile;
			s16 wPayoutCoins;

			// These flags are set randomly and control when the wheels stop.
			// bit 6: allow the player to win in general
			// bit 7: allow the player to win with 7 or bar (plus the effect of bit 6)
			s8 wSlotMachineFlags;
			// wheel 1 can "slip" while this is non - zero
			s8 wSlotMachineWheel1SlipCounter;
			// wheel 2 can "slip" while this is non - zero
			s8 wSlotMachineWheel2SlipCounter;
			// The remaining number of times wheel 3 will roll down a symbol until a match is
			// found, when winning is enabled.It's initialized to 4 each bet.
			s8 wSlotMachineRerollCounter;
			// how many coins the player bet on the slot machine (1 to 3)
			s8 wSlotMachineBet;
		};

		struct
		{
			s8 wCanPlaySlots;
			s8 padding1[8];

			// temporary variable used to add payout amount to the player's coins
			s8 wTempCoins1;
			s8 padding2[2];

			// temporary variable used to subtract the bet amount from the player's coins
			s8 wTempCoins2;
		};

		struct
		{
			s8 wHiddenObjectFunctionArgument;
			s8 wHiddenObjectFunctionRomBank;
			s8 wHiddenObjectIndex;
			s8 wHiddenObjectY;
			s8 wHiddenItemOrCoinsIndex;
			s8 wHiddenObjectX;
		};

		struct
		{
			s8 wPlayerSpinInPlaceAnimFrameDelay;
			s8 wPlayerSpinInPlaceAnimFrameDelayDelta;
			s8 wPlayerSpinInPlaceAnimFrameDelayEndValue;
			s8 wPlayerSpinInPlaceAnimSoundID;
			s8 padding1[6];
			// temporary space used when wFacingDirectionList is rotated
			// used when spinning the player's sprite
			s8 tempSpaceWhenFacingDirectionListIsRotated;
			s8 wFacingDirectionList[4];
			s8 padding2[3];
			s8 wSavedPlayerScreenY;
			s8 wSavedPlayerFacingDirection;
		};

		struct
		{
			s8 wPlayerSpinWhileMovingUpOrDownAnimDeltaY;
			s8 wPlayerSpinWhileMovingUpOrDownAnimMaxY;
			s8 wPlayerSpinWhileMovingUpOrDownAnimFrameDelay;
		};

		struct
		{
			s8 wTrainerSpriteOffset;
			s8 wTrainerEngageDistance;
			s8 wTrainerFacingDirection;
			s8 wTrainerScreenY;
			s8 wTrainerScreenX;
		};

		struct
		{
			s8 wTrainerInfoTextBoxWidthPlus1;
			s8 wTrainerInfoTextBoxWidth;
			s8 wTrainerInfoTextBoxNextRowOffset;
		};

		struct
		{
			s8 wOptionsTextSpeedCursorX;
			s8 wOptionsBattleAnimCursorX;
			s8 wOptionsBattleStyleCursorX;
			s8 wOptionsCancelCursorX;
		};

		struct
		{
			// tile ID of the badge number being drawn
			s8 wBadgeNumberTile;
			// first tile ID of the name being drawn
			s8 wBadgeNameTile;
			// a list of the first tile IDs of each badge or face (depending on whether the
			// badge is owned) to be drawn on the trainer screen
			// the byte after the list gets read when shifting back one byte
			s8 wBadgeOrFaceTiles[NUM_BADGES + 1];
			s8 padding[1];
			// temporary list created when displaying the badges on the trainer screen
			// one byte for each badge; 0 = not obtained, 1 = obtained
			s8 wTempObtainedBadgesBooleans[NUM_BADGES];
		};

		struct
		{
			s8 wUnusedCD3D;

			// the number of credits mons that have been displayed so far
			s8 wNumCreditsMonsDisplayed;
		};

		struct
		{
			s8 padding[1];
			s8 tempWhenwJigglypuffFacingDirectionsIsRotated;
			s8 wJigglypuffFacingDirections[4];
		};

		struct
		{
			s8 padding1[16];
			// $3d = tree tile, $52 = grass tile
			s8 wCutTile;
			s8 padding2[2];
			// 0 = cut animation, 1 = boulder dust animation
			s8 wWhichAnimationOffsets;
		};

		struct
		{
			s8 padding1[18];
			// the index of the sprite the emotion bubble is to be displayed above
			s8 wEmotionBubbleSpriteIndex;
			s8 wWhichEmotionBubble;
		};

		struct
		{
			s16 wChangeBoxSavedMapTextPointer;
		};

		union
		{
			s8 wSavedY;
			s8 wTempSCX;
			// which entry from TradeMons to select
			s8 wWhichTrade;
			s8 wDexMaxSeenMon;
			s8 wPPRestoreItem;
			s8 wWereAnyMonsAsleep;
			s8 wNumShakes;
			s8 wWhichBadge;
			s8 wTitleMonSpecies;
			s8 wPlayerCharacterOAMTile;
			// the number of small stars OAM entries to move down
			s8 wMoveDownSmallStarsOAMCount;
			s8 wChargeMoveNum;
			s8 wCoordIndex;
			s8 wSwappedMenuItem;
			// 0 = no bite
			// 1 = bite
			// 2 = no fish on map
			s8 wRodResponse;
		};
	};

	union
	{
		// 0 = neither
		// 1 = warp pad
		// 2 = hole
		s8 wStandingOnWarpPadOrHole;
		s8 wOAMBaseTile;
		s8 wGymTrashCanIndex;
	};

	s8 wSymmetricSpriteOAMAttributes;

	s8 wMonPartySpriteSpecies;

	// in the trade animation, the mon that leaves the left gameboy
	s8 wLeftGBMonSpecies;

	// in the trade animation, the mon that leaves the right gameboy
	s8 wRightGBMonSpecies;

	// bit 0: is player engaged by trainer(to avoid being engaged by multiple trainers simultaneously)
	// bit 1: boulder dust animation(from using Strength) pending
	// bit 3: using generic PC
	// bit 5: don't play sound when A or B is pressed in menu
	// bit 6: tried pushing against boulder once(you need to push twice before it will move)
	s8 wFlags_0xcd60;

	s8 padding8[9];

	// This has overlapping related uses.
	// When the player tries to use an item or use certain field moves, 0 is stored
	// when the attempt fails and 1 is stored when the attempt succeeds.
	// In addition, some items store 2 for certain types of failures, but this
	// cannot happen in battle.
	// In battle, a non - zero value indicates the player has taken their turn using
	// something other than a move(e.g. using an item or switching pokemon).
	// So, when an item is successfully used in battle, this value becomes non - zero
	// and the player is not allowed to make a move and the two uses are compatible.
	s8 wActionResultOrTookBattleTurn;

	//  Set buttons are ignored.
	s8 wJoyIgnore;

	union
	{
		// size of downscaled mon pic used in pokeball entering / exiting animation
		// $00 = 5×5
		// $01 = 3×3
		s8 wDownscaledMonSize;
		// FormatMovesString stores the number of moves minus one here
		s8 wNumMovesMinusOne;
	};

	union
	{
		// buffer for various data
		s8 wcd6d[20];

		struct
		{
			s8 padding1[4];
			// temp variable used to print a move's current PP on the status screen
			s8 wStatusScreenCurrentPP;
			s8 padding1[6];
			// list of normal max PP (without PP up) values
			s8 wNormalMaxPPList;
			s8 padding1[5];
		};
	};

	union
	{
		// buffer for transferring the random number list generated by the other gameboy
		s8 wSerialOtherGameboyRandomNumberListBlock[0x11];
		s8 wTileMapBackup2[SCREEN_WIDTH * SCREEN_HEIGHT];
	};

	union
	{
		s8 wBuffer[30];
		struct
		{
			s8 wEvoOldSpecies;
			s8 wEvoNewSpecies;
			s8 wEvoMonTileOffset;
			s8 wEvoCancelled;
		};

		struct
		{
			s8 wNamingScreenNameLength;
			// non-zero when the player has chosen to submit the name
			s8 wNamingScreenSubmitName;
			// 0 = upper case
			// 1 = lower case
			s8 wAlphabetCase;
			s8 padding[1];
			s8 wNamingScreenLetter;
		};

		struct
		{
			s8 wChangeMonPicEnemyTurnSpecies;
			s8 wChangeMonPicPlayerTurnSpecies;
		};

		struct
		{
			s16 wHPBarMaxHP;
			s16 wHPBarOldHP;
			s16 wHPBarNewHP;
			s8 wHPBarDelta;
			s16 wHPBarTempHP;
			s8 padding[11];
			s16 wHPBarHPDifference;
		};

		struct
		{
			// lower nybble is x, upper nybble is y
			s8 wTownMapCoords;
			// whether WriteMonMoves is being used to make a mon learn moves from day care
			// non - zero if so
			s8 wLearningMovesFromDayCare;

			s8 padding[27];

			// the item that the AI used
			s8 wAIItem;
			s8 wUsedItemOnWhichPokemon;
		};
	};

	// sound ID during battle animations
	s8 wAnimSoundID;

	// used as a storage value for the bank to return to after a BankswitchHome (bankswitch in homebank)
	s8 wBankswitchHomeSavedROMBank;

	// used as a temp storage value for the bank to switch to
	s8 wBankswitchHomeTemp;

	// 0 = nothing bought or sold in pokemart
	// 1 = bought or sold something in pokemart
	// this value is not used for anything
	s8 wBoughtOrSoldItemInMart;

	// $00 - win
	// $01 - lose
	// $02 - draw
	s8 wBattleResult;

	// bit 0: if set, DisplayTextID automatically draws a text box
	s8 wAutoTextBoxDrawingControl;

	union
	{
		// used in some overworld scripts to vary scripted movement
		s8 wSavedCoordIndex;
		s8 wOakWalkedToPlayer;
		s8 wNextSafariZoneGateScript;
	};

	// used in CheckForTilePairCollisions2 to store the tile the player is on
	s8 wTilePlayerStandingOn;

	s8 wNPCNumScriptedSteps;

	// which script function within the pointer table indicated by
	// wNPCMovementScriptPointerTableNum
	s8 wNPCMovementScriptFunctionNum;

	// bit 0: set when printing a text predef so that DisplayTextID doesn't switch
	//        to the current map's bank
	s8 wTextPredefFlag;

	s8 wPredefParentBank;
	s8 wSpriteIndex;
	// movement byte 2 of current sprite
	s8 wCurSpriteMovement2;
	s8 padding9[2];

	// sprite offset of sprite being controlled by NPC movement script
	s8 wNPCMovementScriptSpriteOffset;
	s8 wScriptedNPCWalkCounter;
	s8 padding10[1];
	s8 wGBC;

	// if running on SGB, it's 1, else it's 0
	s8 wOnSGB;

	s8 wDefaultPaletteCommand;

	union
	{
		struct
		{
			s8 wPlayerHPBarColor;
		};

		struct
		{
			// species of the mon whose palette is used for the whole screen
			s8 wWholeScreenPaletteMonSpecies;
		};
	};

	s8 wEnemyHPBarColor;

	// 0: green
	// 1: yellow
	// 2: red
	s8 wPartyMenuHPBarColors[PARTY_LENGTH];

	s8 wStatusScreenHPBarColor;
	s8 padding11[7];

	union
	{
		s8 wCopyingSGBTileData;
		s8 wWhichPartyMenuHPBar;
		s8 wPalPacket;
	};

	union
	{
		s8 wPartyMenuBlkPacket[0x30];

		struct
		{
			s8 padding[29];
			// storage buffer for various strings
			s8 wStringBuffer[20];
		};

		struct
		{
			s8 padding[29];
			// the total amount of exp a mon gained
			s16 wExpAmountGained;
			s8 wGainBoostedExp;
		};
	};

	s8 wGymCityName[17];
	s8 wGymLeaderName[NAME_LENGTH];
	s8 wItemList[16];
	s16 wListPointer;
	// used to store pointers, but never read
	s16 wUnusedCF8D;
	s16 wItemPrices;
	// used with a lot of things (too much to list here)
	s8 wcf91;
	// which pokemon you selected
	s8 wWhichPokemon;
	// if non-zero, then print item prices when displaying lists
	s8 wPrintItemPrices;
	// type of HP bar
	// $00 = enemy HUD in battle
	// $01 = player HUD in battle / status screen
	// $02 = party menu
	s8 wHPBarType;
	// D used by DisplayListMenuID
	s8 wListMenuID;

	union
	{
		// if non - zero, RemovePokemon will remove the mon from the current box,
		// else it will remove the mon from the party
		s8 wRemoveMonFromBox;
		// 0 = move from box to party
		// 1 = move from party to box
		// 2 = move from daycare to party
		// 3 = move from party to daycare
		s8 wMoveMonType;
	};

	s8 wItemQuantity;
	s8 wMaxItemQuantity;

	// LoadMonData copies mon data here
	PartyStruct loaded_mon;

	// bit 0: The space in VRAM that is used to store walk animation tile patterns
	//        for the player and NPCs is in use for font tile patterns.
	//        This means that NPC movement must be disabled.
	// The other bits are unused.
	s8 wFontLoaded;

	// walk animation counter
	s8 wWalkCounter;

	// background tile number in front of the player (either 1 or 2 steps ahead)
	s8 wTileInFrontOfPlayer;

	// The desired fade counter reload value is stored here prior to calling
	// PlaySound in order to cause the current music to fade out before the new
	// music begins playing. Storing 0 causes no fade out to occur and the new music
	// to begin immediately.
	// This variable has another use related to fade-out, as well. PlaySound stores
	// the sound ID of the music that should be played after the fade-out is finished
	// in this variable. FadeOutAudio checks if it's non-zero every V-Blank and
	// fades out the current audio if it is. Once it has finished fading out the
	// audio, it zeroes this variable and starts playing the sound ID stored in it.
	s8 wAudioFadeOutControl;

	s8 wAudioFadeOutCounterReloadValue;

	s8 wAudioFadeOutCounter;

	// This is used to determine whether the default music is already playing when
	// attempting to play the default music (in order to avoid restarting the same
	// music) and whether the music has already been stopped when attempting to
	// fade out the current music (so that the new music can be begin immediately
	// instead of waiting).
	// It sometimes contains the sound ID of the last music played, but it may also
	// contain $ff (if the music has been stopped) or 0 (because some routines zero
	// it in order to prevent assumptions from being made about the current state of
	// the music).
	s8 wLastMusicSoundID;

	// $00 = causes sprites to be hidden and the value to change to $ff
	// $01 = enabled
	// $ff = disabled
	// other values aren't used
	s8 wUpdateSpritesEnabled;

	s8 wEnemyMoveNum;
	s8 wEnemyMoveEffect;
	s8 wEnemyMovePower;
	s8 wEnemyMoveType;
	s8 wEnemyMoveAccuracy;
	s8 wEnemyMoveMaxPP;
	s8 wPlayerMoveNum;
	s8 wPlayerMoveEffect;
	s8 wPlayerMovePower;
	s8 wPlayerMoveType;
	s8 wPlayerMoveAccuracy;
	s8 wPlayerMoveMaxPP;
	s8 wEnemyMonSpecies2;
	s8 wBattleMonSpecies2;

	s8 wEnemyMonNick[NAME_LENGTH];

	BattleStruct wEnemyMon;

	s8 wEnemyMonBaseStats[NUM_STATS];
	s8 wEnemyMonActualCatchRate;
	s8 wEnemyMonBaseExp;

	s8 wBattleMonNick[NAME_LENGTH];
	BattleStruct wBattleMon;

	s8 wTrainerClass;
	s8 padding12[1];
	s16 wTrainerPicPointer;
	s8 padding13[1];

	union
	{
		s8 wTempMoveNameBuffer[14];
		// The name of the mon that is learning a move.
		s8 wLearnMoveMonName[NAME_LENGTH];
	};

	s8 padding14[2];

	// money received after battle = base money × level of last enemy mon
	s16 wTrainerBaseMoney; // BCD
	s8 wMissableObjectCounter;
	s8 padding15[1];

	// 13 bytes for the letters of the opposing trainer
	// the name is terminated with $50 with possible
	// unused trailing letters
	s8 wTrainerName[13];

	// lost battle, this is -1
	// no battle, this is 0
	// wild battle, this is 1
	// trainer battle, this is 2
	s8 wIsInBattle;

	// flags that indicate which party members should be be given exp when GainExperience is called
	s8 wPartyGainExpFlags;

	// in a wild battle, this is the species of pokemon
	// in a trainer battle, this is the trainer class + OPP_ID_OFFSET
	s8 wCurOpponent;

	// in normal battle, this is 0
	// in old man battle, this is 1
	// in safari battle, this is 2
	s8 wBattleType;

	// bits 0-6: Effectiveness
	//  $0 = immune
	//  $5 = not very effective
	//  $a = neutral
	//  $14 = super-effective
	// bit 7: STAB
	s8 wDamageMultipliers;

	// which entry in LoneAttacks to use
	// it's actually the same thing as ^
	union
	{
		s8 wLoneAttackNo;
		s8 wGymLeaderNo;
	};
	// which instance of [youngster, lass, etc] is this?
	s8 wTrainerNo;

	// $00 = normal attack
	// $01 = critical hit
	// $02 = successful OHKO
	// $ff = failed OHKO
	s8 wCriticalHitOrOHKO;

	s8 wMoveMissed;

	// always 0
	s8 wPlayerStatsToDouble;

	// always 0
	s8 wPlayerStatsToHalve;

	// bit 0 - bide
	// bit 1 - thrash / petal dance
	// bit 2 - attacking multiple times(e.g. double kick)
	// bit 3 - flinch
	// bit 4 - charging up for attack
	// bit 5 - using multi - turn move(e.g.wrap)
	// bit 6 - invulnerable to normal attack(using fly / dig)
	// bit 7 - confusion
	s8 wPlayerBattleStatus1;

	// bit 0 - X Accuracy effect
	// bit 1 - protected by "mist"
	// bit 2 - focus energy effect
	// bit 4 - has a substitute
	// bit 5 - need to recharge
	// bit 6 - rage
	// bit 7 - leech seeded
	s8 wPlayerBattleStatus2;

	// bit 0 - toxic
	// bit 1 - light screen
	// bit 2 - reflect
	// bit 3 - transformed
	s8 wPlayerBattleStatus3;

	// always 0
	s8 wEnemyStatsToDouble;

	// always 0
	s8 wEnemyStatsToHalve;

	s8 wEnemyBattleStatus1;
	s8 wEnemyBattleStatus2;
	s8 wEnemyBattleStatus3;

	// when the player is attacking multiple times, the number of attacks left
	s8 wPlayerNumAttacksLeft;

	s8 wPlayerConfusedCounter;
	s8 wPlayerToxicCounter;

	// high nibble : which move is disabled(1 - 4)
	// low nibble : disable turns left
	s8 wPlayerDisabledMove;
	s8 padding14[1];

	// when the enemy is attacking multiple times, the number of attacks left
	s8 wEnemyNumAttacksLeft;
	s8 wEnemyConfusedCounter;
	s8 wEnemyToxicCounter;

	// high nibble : which move is disabled(1 - 4)
	// low nibble : disable turns left
	s8 wEnemyDisabledMove;
	s8 padding15[1];

	union
	{
		// the amount of damage accumulated by the player while biding
		s16 wPlayerBideAccumulatedDamage;
		s16 wUnknownSerialCounter2;
		// number of hits by player in attacks like Double Slap, etc.
		s8 wPlayerNumHits;
	};

	s8 padding16[2];
	// non-zero when an item or move that allows escape from battle was used
	s8 wEscapedFromBattle;

	union
	{
		s8 wAmountMoneyWon[3];
		struct
		{
			s8 wObjectToHide;
			s8 wObjectToShow;
		};
	};

	union
	{
		// the map you will start at when the debug bit is set
		s8 wDefaultMap;
		s8 wMenuItemOffset;
		//  ID number of the current battle animation
		s8 wAnimationID;
	};

	union
	{
		s8 wNamingScreenType;
		s8 wPartyMenuTypeOrMessageID;
		// temporary storage for the number of tiles in a tileset
		s8 wTempTilesetNumTiles;
	};

	// used by the pokemart code to save the existing value of wListScrollOffset
	// so that it can be restored when the player is done with the pokemart NPC
	s8 wSavedListScrollOffset;
	s8 padding17[2];

	// base coordinates of frame block
	s8 wBaseCoordX;
	s8 wBaseCoordY;

	// low health alarm counter / enable
	// high bit = enable, others = timer to cycle frequencies
	s8 wLowHealthAlarm;

	// counts how many tiles of the current frame block have been drawn
	s8 wFBTileCounter;

	s8 wMovingBGTilesCounter2;

	// duration of each frame of the current subanimation in terms of screen refreshes
	s8 wSubAnimFrameDelay;
	// counts the number of subentries left in the current subanimation
	s8 wSubAnimCounter;

	// 1 = no save file or save file is corrupted
	// 2 = save file exists and no corruption has been detected
	s8 wSaveFileStatus;

	// number of tiles in current battle animation frame block
	s8 wNumFBTiles;

	union
	{
		struct
		{
			s8 wSpiralBallsBaseY;
			s8 wSpiralBallsBaseX;
		};

		struct
		{
			// bits 0 - 6: index into FallingObjects_DeltaXs array(0 - 8)
			// bit 7: direction; 0 = right, 1 = left
			s8 wFallingObjectMovementByte;
			s8 wNumFallingObjects;
		};

		struct
		{
			union
			{
				s8 wFlashScreenLongCounter;
				s8 wNumShootingBalls;
				// 01 if mon is moving from left gameboy to right gameboy; $00 if vice versa
				s8 wTradedMonMovingRight;
				s8 wOptionsInitialized;
				s8 wNewSlotMachineBallTile;
				// how much to add to the X/Y coord
				s8 wCoordAdjustmentAmount;
				s8 wUnusedD08A;
			};

			union
			{
				s8 wSlideMonDelay;
				// generic counter variable for various animations
				s8 wAnimCounter;
				// controls what transformations are applied to the subanimation
				// 01: flip horizontally and vertically
				// 02: flip horizontally and translate downwards 40 pixels
				// 03: translate base coordinates of frame blocks, but don't change their internal coordinates or flip their tiles
				// 04: reverse the subanimation
				s8 wSubAnimTransform;
			};
		};

	};

	s16 wEndBattleWinTextPointer;
	s16 wEndBattleLoseTextPointer;
	s8 padding18[2];
	s8 wEndBattleTextRomBank;
	s8 padding19[2];
	// the address _of the address_ of the current subanimation entry
	s16 wSubAnimAddrPtr;

	union
	{
		// the address of the current subentry of the current subanimation
		s16 wSubAnimSubEntryAddr;
		// If non - zero, the allow matches flag is always set.
		// There is a 1 / 256 (~0.4 %) chance that this value will be set to 60, which is
		// the only way it can increase.Winning certain payout amounts will decrement it
		// or zero it.
		s8 wSlotMachineAllowMatchesCounter;
	};

	s8 padding20[2];

	s8 wOutwardSpiralTileMapPointer;

	union
	{
		s8 wPartyMenuAnimMonEnabled;
		// non - zero when enabled.causes nest locations to blink on and off.
		// the town selection cursor will blink regardless of what this value is
		s8 wTownMapSpriteBlinkingEnabled;
		s8 wUnusedD09B;
	};

	// current destination address in OAM for frame blocks (big endian)
	s16 wFBDestAddr;

	// controls how the frame blocks are put together to form frames
	// specifically, after finishing drawing the frame block, the frame block's mode determines what happens
	// 00: clean OAM buffer and delay
	// 02: move onto the next frame block with no delay and no cleaning OAM buffer
	// 03: delay, but don't clean OAM buffer
	// 04: delay, without cleaning OAM buffer, and do not advance[wFBDestAddr], so that the next frame block will overwrite this one
	s8 wFBMode;


	union
	{
		// 0 = small
		// 1 = big
		s8 wLinkCableAnimBulgeToggle;
		s8 wIntroNidorinoBaseTile;
		s8 wOutwardSpiralCurrentDirection;
		s8 wDropletTile;
		s8 wNewTileBlockID;
		s8 wWhichBattleAnimTileset;
		// 0 = left
		// 1 = right
		s8 wSquishMonCurrentDirection;
		s8 wSlideMonUpBottomRowLeftTile;
	};

	s8 wDisableVBlankWYUpdate;
	s8 wSpriteCurPosX;
	s8 wSpriteCurPosY;
	s8 wSpriteWidth;
	s8 wSpriteHeight;
	// current input byte
	s8 wSpriteInputCurByte;
	// bit offset of last read input bit
	s8 wSpriteInputBitCounter;

	// determines where in the output byte the two bits are placed.Each byte contains four columns(2bpp data)
	// 3->XX000000   1st column
	// 2 -> 00XX0000   2nd column
	// 1 -> 0000XX00   3rd column
	// 0 -> 000000XX   4th column
	s8 wSpriteOutputBitOffset;

	// bit 0 determines used buffer(0->sSpriteBuffer1, 1->sSpriteBuffer2)
	// bit 1 loading last sprite chunk ? (there are at most 2 chunks per load operation)
	s8 wSpriteLoadFlags;
	s8 wSpriteUnpackMode;
	s8 wSpriteFlipped;

	// pointer to next input byte
	s16 wSpriteInputPtr;
	// pointer to current output byte
	s16 wSpriteOutputPtr;
	// used to revert pointer for different bit offsets
	s16 wSpriteOutputPtrCached;
	// pointer to differential decoding table (assuming initial value 0)
	s16 wSpriteDecodeTable0Ptr;
	// pointer to differential decoding table (assuming initial value 1)
	s16 wSpriteDecodeTable1Ptr;

	// used as a temp storage area for Pokemon Species, and other Pokemon/Battle related things
	s8 wd0b5;

	s8 wNameListType;
	s8 wPredefBank;
	s8 wMonHeader;

	// In the ROM base stats data structure, this is the dex number, but it is
	// overwritten with the internal index number after the header is copied to WRAM.
	s8 wMonHIndex;
	union
	{
		s8 wMonHBaseStats;
		s8 wMonHBaseHP;
	};
	s8 wMonHBaseAttack;
	s8 wMonHBaseDefense;
	s8 wMonHBaseSpeed;
	s8 wMonHBaseSpecial;
	union
	{
		s8 wMonHTypes;
		s8 wMonHTypes1;
	};
	s8 wMonHTypes2;
	s8 wMonHCatchRate;
	s8 wMonHBaseEXP;
	s8 wMonHSpriteDim;
	s8 wMonHFrontSprite;
	s8 wMonHBackSprite;
	s8 wMonHMoves[NUM_MOVES];
	s8 wMonHGrowthRate;
	s8 wMonHLearnset[rex::align(NUM_TMS + NUM_HMS, 8) / 8];
	s8 padding21[1];
	// saved at the start of a battle and then written back at the end of the battle
	s8 wSavedTileAnimations;
	s8 padding22[2];
	s16 wDamage;
	s8 padding23[2];
	s8 wRepelRemainingSteps;
	// list of moves for FormatMovesString
	s8 wMoves[NUM_MOVES];
	s8 wMoveNum;
	s8 wMovesString[56];
	s8 wUnusedD119;

	// wWalkBikeSurfState is sometimes copied here, but it doesn't seem to be used for anything
	s8 wWalkBikeSurfStateCopy;

	// the type of list for InitList to init
	s8 wInitListType;

	// 0 if no mon was captured
	s8 wCapturedMonSpecies;

	// Non - zero when the first player mon and enemy mon haven't been sent out yet.
	// It prevents the game from asking if the player wants to choose another mon
	// when the enemy sends out their first mon and suppresses the "no will to fight"
	// message when the game searches for the first non - fainted mon in the party,
	// which will be the first mon sent out.
	s8 wFirstMonsNotOutYet;

	union
	{
		s8 wPokeBallCaptureCalcTemp;
		// lower nybble : number of shakes
		// upper nybble : number of animations to play
		s8 wPokeBallAnimData;
		s8 wUsingPPUp;
		s8 wMaxPP;
		// 0 for player, non-zero for enemy
		s8 wCalculateWhoseStats;
		s8 wTypeEffectiveness;
		s8 wMoveType;
		// used as a Pokemon and Item storage value. Also used as an output value for CountSetBits
		s8 wNumSetBits;
		s8 wd11e;
	};

	// When this value is non - zero, the player isn't allowed to exit the party menu
	// by pressing B and not choosing a mon.
	s8 wForcePlayerToChooseMon;

	// number of times the player has tried to run from battle
	s8 wNumRunAttempts;

	s8 wEvolutionOccurred;

	s8 wVBlankSavedROMBank;

	s8 padding24[1];

	s8 wIsKeyItem;

	s8 wTextBoxID;
	// not exactly sure what this is used for, but it seems to be used as a multipurpose temp flag value
	s8 wCurrentMapScriptFlags;
	s8 wCurEnemyLVL;
	// pointer to list of items terminated by $FF
	s16 wItemListPointer;
	// number of entries in a list
	s8 wListCount;
	s8 wLinkState;
	s8 wTwoOptionMenuID;

	union
	{
		// the id of the menu item the player ultimately chose
		s8 wChosenMenuItem;
		// non-zero when the whole party has fainted due to out-of-battle poison damage
		s8 wOutOfBattleBlackout;
	};

	// the way the user exited a menu
	// for list menus and the buy/sell/quit menu:
	// $01 = the user pressed A to choose a menu item
	// $02 = the user pressed B to cancel
	// for two-option menus:
	// $01 = the user pressed A with the first menu item selected
	// $02 = the user pressed B or pressed A with the second menu item selected
	s8 wMenuExitMethod;

	union
	{
		// the size is always 6, so they didn't need a variable in RAM for this
		s8 wDungeonWarpDataEntrySize;
		// 0 = museum guy
		// 1 = gym guy
		s8 wWhichPewterGuy;
		// there are 3 windows, from 0 to 2
		s8 wWhichPrizeWindow;
		// a horizontal or vertical gate block
		s8 wGymGateTileBlock;
	};

	s8 wSavedSpriteScreenY;
	s8 wSavedSpriteScreenX;
	s8 wSavedSpriteMapY;
	s8 wSavedSpriteMapX;

	s8 padding25[5];

	s8 wWhichPrize;

	// counts downward each frame
	// when it hits 0, bit 5 (ignore input bit) of wd730 is reset
	s8 wIgnoreInputCounter;

	// counts down once every step
	s8 wStepCounter;

	// after a battle, you have at least 3 steps before a random battle can occur
	s8 wNumberOfNoRandomBattleStepsLeft;

	s8 wPrize1;
	s8 wPrize2;
	s8 wPrize3;

	s8 padding26[1];

	union
	{
		s8 wSerialRandomNumberListBlock[11];

		struct
		{
			s16 wPrize1Price;
			s16 wPrize2Price;
			s16 wPrize3Price;
			s8 padding[1];
			// hared list of 9 random numbers, indexed by wLinkBattleRandomNumberListIndex
			s8 wLinkBattleRandomNumberList[10];
		};
	};

	union
	{
		s8 wSerialPlayerDataBlock;
		// When a real item is being used, this is 0.
		// When a move is acting as an item, this is the ID of the item it's acting as.
		// For example, out - of - battle Dig is executed using a fake Escape Rope item.In
		// that case, this would be ESCAPE_ROPE.
		s8 wPseudoItemID;
	};

	s8 wUnusedD153;
	s8 padding27[2];
	s8 wEvoStoneItemID;
	s8 wSavedNPCMovementDirections2Index;
	s8 wPlayerName[NAME_LENGTH];
};

struct PartyData
{
	s8 wPartyCount;
	s8 wPartySpecies[PARTY_LENGTH + 1];
	PartyStruct wPartyMons[PARTY_LENGTH];
	PartyStruct wPartyMonOT[PARTY_LENGTH];
	PartyStruct wPartyMonNicks[PARTY_LENGTH];
};

struct MainData
{
	s8 wPokedexOwned[flag_array_size(NUM_POKEMON)];
	s8 wPokedexSeen[flag_array_size(NUM_POKEMON)];
	s8 wNumBagItems;

	// item, quantity
	s8 wBagItems[BAG_ITEM_CAPACITY * 2 + 1];

	s8 wPlayerMoney[3]; // BCD

	s8 wRivalName[NAME_LENGTH];

	// bit 7 = battle animation
	// 0: On
	// 1: Off
	// bit 6 = battle style
	// 0: Shift
	// 1: Set
	// bits 0 - 3 = text speed(number of frames to delay after printing a letter)
	// 1: Fast
	// 3: Medium
	// 5: Slow
	s8 wOptions;

	s8 wObtainedBadges[flag_array_size(NUM_BADGES)];

	s8 padding1[1];

	// bit 0: If 0, limit the delay to 1 frame.Note that this has no effect if
	//        the delay has been disabled entirely through bit 1 of this variable
	// or bit 6 of wd730.
	// bit 1: If 0, no delay.
	s8 wLetterPrintingDelayFlags;

	s16 wPlayerID;

	s8 wMapMusicSoundID;
	s8 wMapMusicROMBank;

	// offset subtracted from FadePal4 to get the background and object palettes for the current map
	// normally, it is 0. it is 6 when Flash is needed, causing FadePal2 to be used instead of FadePal4
	s8 wMapPalOffset;

	s8 wCurMap;

	// pointer to the upper left corner of the current view in the tile block map
	s16 wCurrentTileBlockMapViewPointer;

	// player's position on the current map
	// nick comment : (I believe this is expressed in squares)
	s8 wYCoord;
	s8 wXCoord;

	// player's position (by block)
	s8 wYBlockCoord;
	s8 wXBlockCoord;

	s8 wLastMap;

	s8 wUnusedD366;

	union
	{
		s8 wCurMapHeader;
		s8 wCurMapTileset;
	};

	s8 wCurMapHeight;
	s8 wCurMapWidth;
	s16 wCurMapDataPtr;
	s16 wCurMapTextPtr;
	s16 wCurMapScriptPtr;
	s8 wCurMapConnections;

	MapConnectionStruct wNorth;
	MapConnectionStruct wSouth;
	MapConnectionStruct wWest;
	MapConnectionStruct wEast;

	// sprite set for the current map (11 sprite picture ID's)
	s8 wSpriteSet[11];

	// sprite set ID for the current map
	s8 wSpriteSetID;

	s16 wObjectDataPointerTemp;
	s8 padding2[2];

	// the tile shown outside the boundaries of the map
	s8 wMapBackgroundTile;

	// number of warps in current map (up to 32)
	s8 wNumberOfWarps;

	// current map warp entries
	s8 wWarpEntries[32 * 4]; // Y, X, warp ID, map ID

	// if $ff, the player's coordinates are not updated when entering the map
	s8 wDestinationWarpID;

	s8 padding3[128];

	// number of signs in the current map (up to 16)
	s8 wNumSigns;

	// Y, X
	s8 wSignCoords[16 * 2];
	s8 wSignTextIDs[16];

	// number of sprites on the current map (up to 16)
	s8 wNumSprites;

	// these two variables track the X and Y offset in blocks from the last special warp used
	// they don't seem to be used for anything
	s8 wYOffsetSinceLastSpecialWarp;
	s8 wXOffsetSinceLastSpecialWarp;

	// movement byte 2, text ID
	s8 wMapSpriteData[16 * 2];
	// trainer class/item ID, trainer set ID
	s8 wMapSpriteExtraData[16 * 2];

	// map height in 2x2 meta-tiles
	s8 wCurrentMapHeight2;

	// map width in 2x2 meta-tiles
	s8 wCurrentMapWidth2;

	// the address of the upper left corner of the visible portion of the BG tile map in VRAM
	s16 wMapViewVRAMPointer;

	// In the comments for the player direction variables below, "moving" refers to
	// both walking and changing facing direction without taking a step.
	//
	// if the player is moving, the current direction
	// if the player is not moving, zero
	// map scripts write to this in order to change the player's facing direction
	s8 wPlayerMovingDirection;

	// the direction in which the player was moving before the player last stopped
	s8 wPlayerLastStopDirection;

	// if the player is moving, the current direction
	// if the player is not moving, the last the direction in which the player moved
	s8 wPlayerDirection;

	s8 wTilesetBank;

	// maps blocks (4x4 tiles) to tiles
	s16 wTilesetBlocksPtr;

	s16 wTilesetGfxPtr;

	// list of all walkable tiles
	s16 wTilesetCollisionPtr;

	s8 wTilesetTalkingOverTiles[3];
	s8 wGrassTile;
	s8 padding4[4];

	s8 wNumBoxItems;

	// item, quantity
	s8 wBoxItems[PC_ITEM_CAPACITY * 2 + 1];

	// bits 0 - 6: box number
	// bit 7: whether the player has changed boxes before
	s8 wCurrentBoxNum;

	s8 padding5[1];

	// number of HOF teams
	s8 wNumHoFTeams;

	s8 wUnusedD5A3;

	s16 wPlayerCoins; // BCD

	s8 wMissableObjectFlags[flag_array_size(0x100)];
	s8 padding6[7];

	s8 wSavedSpriteImageIndex;
	s8 wMissableObjectList[16 * 2 + 1];
	s8 padding7[1];

	union
	{
		s8 wGameProgressFlags;
		s8 wOaksLabCurScript;
	};

	s8 wPalletTownCurScript;
	s8 padding8[1];
	s8 wBluesHouseCurScript;
	s8 padding9[1];
	s8 wViridianCityCurScript;
	s8 padding10[2];
	s8 wPewterCityCurScript;
	s8 wRoute3CurScript;
	s8 wRoute4CurScript;
	s8 padding11[1];
	s8 wViridianGymCurScript;
	s8 wPewterGymCurScript;
	s8 wCeruleanGymCurScript;
	s8 wVermilionGymCurScript;
	s8 wCeladonGymCurScript;
	s8 wRoute6CurScript;
	s8 wRoute8CurScript;
	s8 wRoute24CurScript;
	s8 wRoute25CurScript;
	s8 wRoute9CurScript;
	s8 wRoute10CurScript;
	s8 wMtMoon1FCurScript;
	s8 wMtMoonB2FCurScript;
	s8 wSSAnne1FRoomsCurScript;
	s8 wSSAnne2FRoomsCurScript;
	s8 wRoute22CurScript;
	s8 padding12[1];
	s8 wRedsHouse2FCurScript;
	s8 wViridianMartCurScript;
	s8 wRoute22GateCurScript;
	s8 wCeruleanCityCurScript;
	s8 padding13[7];
	s8 wSSAnneBowCurScript;
	s8 wViridianForestCurScript;
	s8 wMuseum1FCurScript;
	s8 wRoute13CurScript;
	s8 wRoute14CurScript;
	s8 wRoute17CurScript;
	s8 wRoute19CurScript;
	s8 wRoute21CurScript;
	s8 wSafariZoneGateCurScript;
	s8 wRockTunnelB1FCurScript;
	s8 wRockTunnel1FCurScript;
	s8 padding14[1];
	s8 wRoute11CurScript;
	s8 wRoute12CurScript;
	s8 wRoute15CurScript;
	s8 wRoute16CurScript;
	s8 wRoute18CurScript;
	s8 wRoute20CurScript;
	s8 wSSAnneB1FRoomsCurScript;
	s8 wVermilionCityCurScript;
	s8 wPokemonTower2FCurScript;
	s8 wPokemonTower3FCurScript;
	s8 wPokemonTower4FCurScript;
	s8 wPokemonTower5FCurScript;
	s8 wPokemonTower6FCurScript;
	s8 wPokemonTower7FCurScript;
	s8 wRocketHideoutB1FCurScript;
	s8 wRocketHideoutB2FCurScript;
	s8 wRocketHideoutB3FCurScript;
	s8 wRocketHideoutB4FCurScript;
	s8 padding15[1];
	s8 wRoute6GateCurScript;
	s8 wRoute8GateCurScript;
	s8 padding16[1];
	s8 wCinnabarIslandCurScript;
	s8 wPokemonMansion1FCurScript;
	s8 padding17[1];
	s8 wPokemonMansion2FCurScript;
	s8 wPokemonMansion3FCurScript;
	s8 wPokemonMansionB1FCurScript;
	s8 wVictoryRoad2FCurScript;
	s8 wVictoryRoad3FCurScript;
	s8 padding18[1];
	s8 wFightingDojoCurScript;
	s8 wSilphCo2FCurScript;
	s8 wSilphCo3FCurScript;
	s8 wSilphCo4FCurScript;
	s8 wSilphCo5FCurScript;
	s8 wSilphCo6FCurScript;
	s8 wSilphCo7FCurScript;
	s8 wSilphCo8FCurScript;
	s8 wSilphCo9FCurScript;
	s8 wHallOfFameCurScript;
	s8 wChampionsRoomCurScript;
	s8 wLoreleisRoomCurScript;
	s8 wBrunosRoomCurScript;
	s8 wAgathasRoomCurScript;
	s8 wCeruleanCaveB1FCurScript;
	s8 wVictoryRoad1FCurScript;
	s8 padding19[1];
	s8 wLancesRoomCurScript;
	s8 padding20[4];
	s8 wSilphCo10FCurScript;
	s8 wSilphCo11FCurScript;
	s8 padding21[1];
	s8 wFuchsiaGymCurScript;
	s8 wSaffronGymCurScript;
	s8 padding22[1];
	s8 wCinnabarGymCurScript;
	s8 wGameCornerCurScript;
	s8 wRoute16Gate1FCurScript;
	s8 wBillsHouseCurScript;
	s8 wRoute5GateCurScript;
	s8 wPowerPlantCurScript;
	s8 wRoute7GateCurScript;
	s8 padding23[1];
	s8 wSSAnne2FCurScript;
	s8 wSeafoamIslandsB3FCurScript;
	s8 wRoute23CurScript;
	s8 wSeafoamIslandsB4FCurScript;
	s8 wRoute18Gate1FCurScript;
	s8 padding24[78];
	s8 padding25[56];

	s8 wObtainedHiddenItemsFlags[flag_array_size(112)];
	s8 wObtainedHiddenCoinsFlags[flag_array_size(16)];

	// $00 = walking
	// $01 = biking
	// $02 = surfing
	s8 wWalkBikeSurfState;
	s8 padding26[10];

	s8 wTownVisitedFlag[NUM_CITY_MAPS];
	// starts at 502
	s16 wSafariSteps;

	// item given to cinnabar lab
	s8 wFossilItem;
	// mon that will result from the item
	s8 wFossilMon;
	s8 padding27[2];

	// trainer classes start at OPP_ID_OFFSET
	s8 wEnemyMonOrTrainerClass;

	s8 wPlayerJumpingYScreenCoordsIndex;
	s8 wRivalStarter;
	s8 padding28[1];

	s8 wPlayerStarter;

	// sprite index of the boulder the player is trying to push
	s8 wBoulderSpriteIndex;

	s8 wLastBlackoutMap;

	// destination map (for certain types of special warps, not ordinary walking)
	s8 wDestinationMap;

	s8 wUnusedD71B;

	// used to store the tile in front of the boulder when trying to push a boulder
	// also used to store the result of the collision check($ff for a collision and $00 for no collision)
	s8 wTileInFrontOfBoulderAndBoulderCollisionResult;

	// destination map for dungeon warps
	s8 wDungeonWarpDestinationMap;

	// which dungeon warp within the source map was used
	s8 wWhichDungeonWarp;

	s8 wUnusedD71F;

	s8 padding29[8];

	// bit 0: using Strength outside of battle
	// bit 1: set by IsSurfingAllowed when surfing's allowed, but the caller resets it after checking the result
	// bit 3: received Old Rod
	// bit 4: received Good Rod
	// bit 5: received Super Rod
	// bit 6: gave one of the Saffron guards a drink
	// bit 7: set by ItemUseCardKey, which is leftover code from a previous implementation of the Card Key
	s8 wd728;
	s8 padding30[1];

	// redundant because it matches wObtainedBadges
	// used to determine whether to show name on statue and in two NPC text scripts
	s8 wBeatGymFlags;
	s8 padding31[1];

	// bit 0: if not set, the 3 minimum steps between random battles have passed
	// bit 1: prevent audio fade out
	s8 wd72c;

	// This variable is used for temporary flags and as the destination map when
	// warping to the Trade Center or Colosseum.
	// bit 0: sprite facing directions have been initialised in the Trade Center
	// bit 3: do scripted warp(used to warp back to Lavender Town from the top of the pokemon tower)
	// bit 4: on a dungeon warp
	// bit 5: don't make NPCs face the player when spoken to
	// Bits 6 and 7 are set by scripts when starting major battles in the storyline,
	// but they do not appear to affect anything.Bit 6 is reset after all battles
	// and bit 7 is reset after trainer battles(but it's only set before trainer
	// battles anyway).
	s8 wd72d;

	// bit 0: the player has received Lapras in the Silph Co.building
	// bit 1: set in various places, but doesn't appear to have an effect
	// bit 2: the player has healed pokemon at a pokemon center at least once
	// bit 3: the player has a received a pokemon from Prof.Oak
	// bit 4: disable battles
	// bit 5: set when a battle ends and when the player blacks out in the overworld due to poison
	// bit 6: using the link feature
	// bit 7: set if scripted NPC movement has been initialised
	s8 wd72e;
	s8 padding32[1];

	// bit 0: NPC sprite being moved by script
	// bit 5: ignore joypad input
	// bit 6: print text with no delay between each letter
	// bit 7: set if joypad states are being simulated in the overworld or an NPC's movement is being scripted
	s8 wd730;
	s8 padding33[1];

	// bit 0: play time being counted
	// bit 1: debug mode(unused and incomplete in non - debug builds)
	// bit 2: the target warp is a fly warp(bit 3 set or blacked out) or a dungeon warp(bit 4 set)
	// bit 3: used warp pad, escape rope, dig, teleport, or fly, so the target warp is a "fly warp"
	// bit 4: jumped into hole(Pokemon Mansion, Seafoam Islands, Victory Road) or went down waterfall(Seafoam Islands), so the target warp is a "dungeon warp"
	// bit 5: currently being forced to ride bike(cycling road)
	// bit 6: map destination is[wLastBlackoutMap](usually the last used pokemon center, but could be the player's house)
	s8 wd732;

	// bit 0: running a test battle(unused in non - debug builds)
	// bit 1: prevent music from changing when entering new map
	// bit 2: skip the joypad check in CheckWarpsNoCollision(used for the forced warp down the waterfall in the Seafoam Islands)
	// bit 3: trainer wants to battle
	// bit 4: use variable[wCurMapScript] instead of the provided index for next frame's map script (used to start battle when talking to trainers)
	// bit 7: used fly out of battle
	s8 wFlags_D733;

	// bit 1: set when you beat Lorelei and reset in Indigo Plateau lobby
	// the game uses this to tell when Elite 4 events need to be reset
	s8 wBeatLorelei;
	s8 padding34[1];

	// bit 0: check if the player is standing on a door and make him walk down a step if so
	// bit 1: the player is currently stepping down from a door
	// bit 2: standing on a warp
	// bit 6: jumping down a ledge / fishing animation
	// bit 7: player sprite spinning due to spin tiles(Rocket hideout / Viridian Gym)
	PlayerMovementFlags wd736;

	s16 wCompletedInGameTradeFlags;
	s8 padding35[2];

	s8 wWarpedFromWhichWarp;
	s8 wWarpedFromWhichMap;
	s8 padding36[2];

	s8 wCardKeyDoorY;
	s8 wCardKeyDoorX;
	s8 padding37[2];

	s8 wFirstLockTrashCanIndex;
	s8 wSecondLockTrashCanIndex;
	s8 padding38[2];

	s8 wEventFlags[flag_array_size(NUM_EVENTS)];

	union
	{
		struct
		{
			s8 wGrassRate;
			s8 wGrassMons[10 * 2];
			s8 padding[8];

			s8 wWaterRate;
			s8 wWaterMons[10 * 2];
		};

		struct
		{
			s8 wLinkEnemyTrainerName[NAME_LENGTH];
			s8 padding1[1];
			s8 wSerialEnemyDataBlock[9]; // This acts as padding
			s8 wEnemyPartyCount;
			s8 wEnemyPartySpecies[PARTY_LENGTH + 1];

			PartyStruct wEnemyMon[PARTY_LENGTH];
			PartyStruct wEnemyMonNicks[PARTY_LENGTH];
		};
	};

	s16 wTrainerHeaderPtr;
	s8 padding39[6];

	union
	{
		// the trainer the player must face after getting a wrong answer in the Cinnabar
		// gym quiz
		s8 wOpponentAfterWrongAnswer;
		s8 wUnusedDA38;
	};

	// index of current map script, mostly used as index for function pointer array
	// mostly copied from map - specific map script pointer and written back later
	s8 wCurMapScript;
	s8 padding40[7];

	s8 wPlayTimeHours;
	s8 wPlayTimeMaxed;
	s8 wPlayTimeMinutes;
	s8 wPlayTimeSeconds;
	s8 wPlayTimeFrames;
	s8 wSafariZoneGameOver;
	s8 wNumSafariBalls;
	// 0 if no pokemon is in the daycare
	// 1 if pokemon is in the daycare
	s8 wDayCareInUse;
	s8 wDayCareMonName[NAME_LENGTH];
	s8 wDayCareMonOT[NAME_LENGTH];
	BoxStruct wDayCareMon;

};

struct CurrentBoxData
{
	s8 wBoxCount;
	s8 wBoxSpecies[MONS_PER_BOX + 1];

	BoxStruct wBoxMons[MONS_PER_BOX];
	BoxStruct wBoxMonOT[MONS_PER_BOX];
	BoxStruct wBoxMonNicks[MONS_PER_BOX];
};

struct Stack
{
	s8 wStake[0x100 - 1];
};

struct Wram
{
	AudioRam audio_data;
	SpriteStateData sprite_state_data;
	OAMBuffer oam_buffer;
	Tilemap tilemap;
	OverworldMap overworld_map;
	WramData data;
	PartyData party_data;
	MainData main_data;
	CurrentBoxData current_box_data;
	Stack stack;
};

extern Wram wram;
