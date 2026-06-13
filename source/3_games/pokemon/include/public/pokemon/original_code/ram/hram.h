#pragma once

#include "rex_engine/engine/types.h"

struct Hram
{
	// Initialized to 16.
	// Decremented each input iteration if the player
	// presses the reset sequence(A + B + SEL + START).
	// Soft reset when 0 is reached.
	s8 hSoftReset;

	union
	{
		struct
		{
			union
			{
				// base tile ID to which offsets are added
				s8 hBaseTileID;
				s8 hDexWeight;
				s8 hWarpDestinationMap;
				s8 hOAMTile;
				s8 hROMBankTemp;
				s8 hPreviousTileset;
				s8 hRLEByteValue;
			};

			union
			{
				// DisplayTextID's argument
				s8 hSpriteIndexOrTextID;
				s8 hPartyMonIndex;
			};

			s8 hVRAMSlot;

			union
			{
				s8 hFourTileSpriteCount;
				s8 hHalveItemPrices;
			};
		};

		// BCD number
		s8 hItemPrice[3];
		struct
		{
			s8 hSlideAmount;
			// the total number of tiles being shifted each time the pic slides by one tile
			s8 hSlidingRegionSize;
			// -1 = left
			//  0 = right
			s8 hSlideDirection;
		};

		struct
		{
			union
			{
				s8 hSpriteInterlaceCounter;
				s8 hSpriteWidth;
			};
			s8 hSpriteHeight; // in tiles
			s8 hSpriteOffset;	// in tiles
		};

		struct
		{
			s8 hDownArrowBlinkCount1;
			s8 hDownArrowBlinkCount2;
		};

		struct
		{
			union
			{
				s8 hMapStride;
				s8 hEastWestConnectedMapWidth;
				s8 hNorthSouthConnectionStripWidth;
			};

			union
			{
				s8 hMapWidth;
				s8 hNorthSouthConnectedMapWidth;
			};
		};

		struct
		{
			s8 hSpriteDataOffset;
			s8 hSpriteIndex;

			union
			{
				s8 hSpriteImageIndex;
				s8 hSpriteFacingDirection;
				s8 hSpriteMovementByte2;
			};
		};

		struct
		{
			s8 hLoadSpriteTemp1;
			s8 hLoadSpriteTemp2;
		};

		struct
		{
			s8 padding[2];
			s16 hEnemySpeed;
		};
	};

	union
	{
		struct
		{
			s8 hSpriteOffset2;
			s8 hOAMBufferOffset;
			s8 hSpriteScreenX;
			s8 hSpriteScreenY;
		};

		struct
		{
			s8 hCollidingSpriteOffset;
			s8 hCollidingSpriteTempYValue;
			s8 hCollidingSpriteTempXValue;
			s8 hCollidingSpriteAdjustedDistance;
		};
	};

	s8 hTilePlayerStandingOn;
	s8 hSpritePriority;

	union
	{
		union
		{
			struct
			{
				s8 padding1[1];
				s8 hMultiplicand[3];
				s8 hMultiplier;
				s8 padding2[1];
				s8 hMultiplyBuffer[4];
			};
			struct
			{
				s8 hProduct[4];
			};
			struct
			{
				s8 hDividend[4];
				s8 hDivisor;
				s8 hDivideBuffer[5];
			};
			struct
			{
				s8 hQuotient[4];
				s8 hRemainder;
			};
		};

		struct
		{
			// PrintNumber (big endian).
			s8 hPastLeadingZeros; // last char printed
			s8 hNumToPrint[3];
			s8 hPowerOf10[3];
			s8 hSavedNumToPrint[3];
		};

		struct
		{
			union
			{
				s8 hNPCMovementDirections2Index;
				s8 hNPCSpriteOffset;
				// distance in steps between NPC and player
				s8 hNPCPlayerYDistance;
			};
			s8 hNPCPlayerXDistance;
			s8 hFindPathNumSteps;

			// bit 0: set when the end of the path's Y coordinate matches the target's
			// bit 1: set when the end of the path's X coordinate matches the target's
			// When both bits are set, the end of the path is at the target's position
			// (i.e.the path has been found).
			s8 hFindPathFlags;
			s8 hFindPathYProgress;
			s8 hFindPathXProgress;

			// 0 = from player to NPC
			// 1 = from NPC to player
			s8 hNPCPlayerRelativePosPerspective;
			s8 padding[1];

			// bit 0:
			// 0 = target is to the south or aligned
			// 1 = target is to the north
			// bit 1:
			// 0 = target is to the east or aligned
			// 1 = target is to the west
			s8 hNPCPlayerRelativePosFlags;
		};

		struct
		{
			s8 hSwapItemID;
			s8 hSwapItemQuantity;
		};

		struct
		{
			s8 hSignCoordPointer;
		};

		struct
		{
			s8 padding;
			s8 hMutateWY;
			s8 hMutateWX;
		};

		struct
		{
			s8 hSwapTemp;
			s8 hExperience[3];
		};
	};

	union
	{
		struct
		{
			s8 hMoney[3]; // BCD number
		};

		struct
		{
			s8 hUnusedCoinsByte;
			s16 hCoins; // BCD number
		};
	};

	union
	{
		s8 hDivideBCDDivisor[3];
		s8 hDivideBCDQuotient[3]; // BCD number
	};

	s8 hDivideBCDBuffer[3]; // BCD number
	s8 padding[1];

	s8 hSerialReceivedNewData;

	// $01 = using external clock
	// $02 = using internal clock
	// $ff = establishing connection
	s8 hSerialConnectionStatus;
	s8 hSerialIgnoringInitialData;
	s8 hSerialSendData;
	s8 hSerialReceiveData;

	// these values are copied to rSCX, rSCY, and rWY during V-blank
	s8 hSCX;
	s8 hSCY;
	s8 hWY;

	s8 hJoyLast;
	s8 hJoyReleased;
	s8 hJoyPressed;
	s8 hJoyHeld;
	s8 hJoy5;
	s8 hJoy6;
	s8 hJoy7;

	s8 hLoadedROMBank;
	s8 hSavedROMBank;

	// is automatic background transfer during V - blank enabled ?
	// if nonzero, yes
	// if zero, no
	s8 hAutoBGTransferEnabled;

	// 00 = top third of background
	// 01 = middle third of background
	// 02 = bottom third of background
	s8 hAutoBGTransferPortion;

	// the destination address of the automatic background transfer
	s16 hAutoBGTransferDest;

	s8 hRedrawMapViewRowOffset;

	// temporary storage for stack pointer during memory transfers that use pop
	// to increase speed
	s16 hSPTemp;

	// source address for VBlankCopyBgMap function
	// the first byte doubles as the byte that enabled the transfer.
	// if it is 0, the transfer is disabled
	// if it is not 0, the transfer is enabled
	// this means that XX00 is not a valid source address
	s16 hVBlankCopyBGSource;

	// destination address for VBlankCopyBgMap function
	s16 hVBlankCopyBGDest;

	// number of rows for VBlankCopyBgMap to copy
	s8 hVBlankCopyBGNumRows;

	// size of VBlankCopy transfer in 16-byte units
	s8 hVBlankCopySize;

	// source address for VBlankCopy function
	s16 hVBlankCopySource;

	// destination address for VBlankCopy function
	s16 hVBlankCopyDest;

	// size of source data for VBlankCopyDouble in 8-byte units
	s8 hVBlankCopyDoubleSize;

	// source address for VBlankCopyDouble function
	s16 hVBlankCopyDoubleSource;

	// destination address for VBlankCopyDouble function
	s16 hVBlankCopyDoubleDest;

	// controls whether a row or column of 2x2 tile blocks is redrawn in V - blank
	// 00 = no redraw
	// 01 = redraw column
	// 02 = redraw row
	s8 hRedrawRowOrColumnMode;

	s16 hRedrawRowOrColumnDest;

	s8 hRandomAdd;
	s8 hRandomSub;

	// decremented every V-blank (used for delays)
	s8 hFrameCounter;

	// V - blank sets this to 0 each time it runs.
	// So, by setting it to a nonzero value and waiting for it to become 0 again,
	// you can detect that the V - blank handler has run since then.
	s8 hVBlankOccurred;

	// Controls which tiles are animated.
	// 0 = no animations(breaks Surf)
	// 1 = water tile $14 is animated
	// 2 = water tile $14 and flower tile $03 are animated
	s8 hTileAnimations;

	s8 hMovingBGTilesCounter1;

	s8 padding2[1];

	// multiple of 0x10
	s8 hCurrentSpriteOffset;

	union
	{
		struct
		{
			s8 hPlayerFacing;
			s8 hPlayerYCoord;
			s8 hPlayerXCoord;
		};

		struct
		{
			// $00 = bag full
			// $01 = got item
			// $80 = didn't meet required number of owned mons
			// $FF = player cancelled
			union
			{
				s8 hOaksAideResult;
				// required number of owned mons
				s8 hOaksAideRequirement;
			};

			s8 hOaksAideRewardItem;
			s8 hOaksAideNumMonsOwned;
		};

		struct
		{
			s8 hVendingMachineItem;
			s8 hVendingMachinePrice[3]; // BCD number
		};

		struct
		{
			s8 hGymGateIndex;
			s8 hGymGateAnswer;
		};

		struct
		{
			s8 hDexRatingNumMonsSeen;
			s8 hDexRatingNumMonsOwned;
		};

		struct
		{
			s8 hItemToRemoveID;
			s8 hItemToRemoveIndex;
		};

		struct
		{
			union
			{
				s8 hItemCounter;
				s8 hSavedCoordIndex;
				s8 hMissableObjectIndex;
				s8 hGymTrashCanRandNumMask;
				s8 hInteractedWithBookshelf;
			};
		};
	};

	s8 padding3[1];

	union
	{
		s8 hBackupGymGateIndex;
		s8 hUnlockedSilphCoDoors;
	};

	s8 hStartTileID;
	s8 padding4[2];

	s8 hNewPartyLength;

	union
	{
		struct
		{
			s8 hDividend2;
			s8 hDivisor2;
			s8 hQuotient2;
		};

		struct
		{
			s8 hIsHiddenMissableObject;
		};
	};

	s8 hMapROMBank;
	s8 hSpriteVRAMSlotAndFacing;

	union
	{
		s8 hCoordsInFrontOfPlayerMatch;
		s8 hSpriteAnimFrameCounter;
	};

	union
	{
		struct
		{
			s8 hSpriteScreenYCoord;
			s8 hSpriteScreenXCoord;
			s8 hSpriteMapYCoord;
			s8 hSpriteMapXCoord;
		};

		struct
		{
			s8 hItemAlreadyFound;
			s8 padding[2];
			s8 hDidntFindAnyHiddenObject;
		};

		struct
		{
			s8 padding1[1];
			s16 hSavedMapTextPtr;
			s8 padding2[1];
		};
	};

	s8 padding5[4];

	// ; 0 on player's turn, 1 on enemy's turn
	s8 hWhoseTurn;
	s8 hClearLetterPrintingDelayFlags;
	s8 padding6[1];

	// bit 0: draw HP fraction to the right of bar instead of below(for party menu)
	// bit 1: menu is double spaced
	s8 hUILayoutFlags;

	s8 hFieldMoveMonMenuTopMenuItemX;
	s8 hJoyInput;
	s8 hDisableJoypadPolling;

	s8 padding7[5];
};

extern Hram hram;
