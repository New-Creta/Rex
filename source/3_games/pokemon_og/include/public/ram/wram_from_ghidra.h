#pragma once

typedef unsigned char   undefined;

typedef unsigned char    byte;
typedef unsigned long    dword;
typedef unsigned char    undefined1;
typedef unsigned int    undefined2;
typedef unsigned int    word;
typedef struct FallingObjectData FallingObjectData, * PFallingObjectData;

struct FallingObjectData {
  byte wFallingObjectMovementByte;
  byte wNumFallingObjects;
};

typedef union MiscUnion MiscUnion, * PMiscUnion;

union MiscUnion {
  byte field0;
  byte field1;
  byte field2;
  byte wSavedY;
  byte wTempSCX;
  byte wWhichTrade;
  byte wDexMaxSeenMon;
  byte wPPRestoreItem;
  byte wWereAnyMonsAsleep;
  byte wNumShakes;
  byte wWhichBadge;
  byte wTitleMonSpecies;
  byte wPlayerCharacterOAMTile;
  byte wMoveDownSmallStarsOAMCount;
  byte wChargeMoveNum;
  byte wCoordIndex;
  byte wSwappedMenuItem;
  byte wRodResponse;
};

typedef struct MiscData6 MiscData6, * PMiscData6;

typedef union MiscUnion4 MiscUnion4, * PMiscUnion4;

typedef union MiscUnion5 MiscUnion5, * PMiscUnion5;

typedef union MiscUnion6 MiscUnion6, * PMiscUnion6;

union MiscUnion4 {
  byte hBaseTileID;
  byte hDexWeight;
  byte hWarpDestinationMap;
  byte hOAMTile;
  byte hROMBankTemp;
  byte hPreviousTileset;
  byte hRLEByteValue;
};

union MiscUnion5 {
  byte hSpriteIndexOrTextID;
  byte hPartyMonIndex;
};

union MiscUnion6 {
  byte hFourTileSpriteCount;
  byte hHalveItemPrices;
};

struct MiscData6 {
  union MiscUnion4 field0_0x0;
  union MiscUnion5 field1_0x1;
  byte hVRAMSlot;
  union MiscUnion6 field3_0x3;
};

typedef struct EmotionData EmotionData, * PEmotionData;

struct EmotionData {
  char Padding[18];
  byte wEmotionBubbleSpriteIndex;
  byte wWhichEmotionBubble;
};

typedef union Union9 Union9, * PUnion9;

union Union9 {
  byte wTownMapCoords;
  byte wLearningMovesFromDayCare;
};

typedef union Union8 Union8, * PUnion8;

typedef struct EvoData EvoData, * PEvoData;

typedef struct wNamingScreen wNamingScreen, * PwNamingScreen;

typedef struct ChangeMonPic ChangeMonPic, * PChangeMonPic;

typedef struct HPBarData HPBarData, * PHPBarData;

typedef struct MiscData2 MiscData2, * PMiscData2;

struct HPBarData {
  word wHPBarMaxHP;
  word wHPBarOldHP;
  word wHPBarNewHP;
  byte wHPBarDelta;
  word wHPBarTempHP;
  char Padding[11];
  word wHPBarHPDifference;
};

struct wNamingScreen {
  byte wNamingScreenNameLength;
  byte wNamingScreenSubmitName;
  byte wAlphabetCase;
  byte Padding;
  byte wNamingScreenLetter;
};

struct ChangeMonPic {
  byte wChangeMonPicEnemyTurnSpecies;
  byte wChangeMonPicPlayerTurnSpecies;
};

struct EvoData {
  byte wEvoOldSpecies;
  byte wEvoNewSpecies;
  byte wEvoMonTileOffset;
  byte wEvoCancelled;
};

struct MiscData2 {
  union Union9 field0_0x0;
  char field1_0x1[27];
  byte wAIItem;
  byte wUsedItemOnWhichPokemon;
};

union Union8 {
  char field0[30];
  struct EvoData field1;
  struct wNamingScreen field2;
  struct ChangeMonPic field3;
  struct HPBarData field4;
  struct MiscData2 field5;
};

typedef union Union7 Union7, * PUnion7;

union Union7 {
  char wSerialOtherGameboyRandomNumberListBlock[17];
  char wTileMapBackup2[360];
};

typedef struct MiscData3 MiscData3, * PMiscData3;

struct MiscData3 {
  union MiscUnion4 field0_0x0;
  union MiscUnion5 field1_0x1;
  byte hVRAMSlot;
  union MiscUnion6 field3_0x3;
};

typedef struct MiscData5 MiscData5, * PMiscData5;

struct MiscData5 {
  byte hSwapTemp;
  char hExperience[3];
};

typedef union MiscData4 MiscData4, * PMiscData4;

union MiscData4 {
  byte hPastLeadingZeros;
  char hNumToPrint[3];
  char hPowerOf10[3];
  char hSavedNumToPrint[3];
};

typedef union Union2 Union2, * PUnion2;

typedef struct InGameTradeData InGameTradeData, * PInGameTradeData;

typedef struct PokemonStatData PokemonStatData, * PPokemonStatData;

typedef struct EngagedTrainerData EngagedTrainerData, * PEngagedTrainerData;

typedef struct PlayerMonUnmodifiedData PlayerMonUnmodifiedData, * PPlayerMonUnmodifiedData;

typedef struct PlayerMonData PlayerMonData, * PPlayerMonData;

typedef struct EnemyMonUnmodified EnemyMonUnmodified, * PEnemyMonUnmodified;

typedef struct EnemyMonData EnemyMonData, * PEnemyMonData;

struct PlayerMonUnmodifiedData {
  byte Level;
  word MaxHP;
  word Attack;
  word Defense;
  word Speed;
  word Special;
};

struct EnemyMonData {
  byte StatMods;
  byte AttackMod;
  byte DefenseMod;
  byte SpeedMod;
  byte SpecialMod;
  byte AccuracyMod;
  char field6_0x6[2];
};

struct PlayerMonData {
  byte AttackMod;
  byte DefenseMod;
  byte SpeedMod;
  byte SpecialMod;
  byte AccuracyMod;
  byte EvasionMod;
  char field6_0x6[2];
};

struct EnemyMonUnmodified {
  byte Level;
  word MaxHP;
  word Attack;
  word Defense;
  word Speed;
  word Special;
};

struct PokemonStatData {
  struct PlayerMonUnmodifiedData PlayerUnmodifiedStats;
  struct PlayerMonData PlayerModifiedStats;
  byte Padding;
  struct EnemyMonUnmodified EnemyUnmodifiedStats;
  struct EnemyMonData EnemyModifiedStats;
};

struct EngagedTrainerData {
  char Padding[30];
  byte Class;
  byte Set;
};

struct InGameTradeData {
  byte GiveMonSpecies;
  word TextPointerTablePointer;
  byte TextPointerTableIndex;
  char GiveMonName[11];
  char ReceiveMonName[11];
  char MonNick[11];
  byte ReceiveMonSpecies;
};

union Union2 {
  struct InGameTradeData InGameTradeData;
  struct PokemonStatData PokemonStats;
  struct EngagedTrainerData EngagedTrainerData;
};

typedef union Union1 Union1, * PUnion1;

typedef struct DexRating DexRating, * PDexRating;

typedef struct SlotMachineData SlotMachineData, * PSlotMachineData;

typedef struct AnimData AnimData, * PAnimData;

typedef struct SwitchPartyTempData SwitchPartyTempData, * PSwitchPartyTempData;

typedef struct MiscBattleData MiscBattleData, * PMiscBattleData;

struct SwitchPartyTempData {
  char padding[60];
  char wSwitchPartyMonTempBuffer[44];
};

struct SlotMachineData {
  byte wSlotMachineSevenAndBarModeChance;
  char padding1[2];
  byte wSlotMachineSavedROMBank;
  char padding2[166];
  byte wLuckySlotHiddenObjectIndex;
};

struct DexRating {
  byte wDexRatingNumMonsSeen;
  byte wDexRatingNumMonsOwned;
  byte wDexRatingText;
};

struct MiscBattleData {
  byte wUnusedCC5B;
  char Padding1[59];
  char wNPCMovementDirections2[10];
  byte wNumStepsToTake;
  char Padding2[48];
  byte wRLEByteCount;
  byte wCanEvolveFlags;
  byte wForceEvolution;
  byte wAILayer2Encouragement;
  byte Padding3;
  byte wPlayerSubstituteHP;
  byte wEnemySubstituteHP;
  byte wTestBattlePlayerSelectedMove;
  byte Padding4;
  byte wMoveMenuType;
  byte wPlayerSelectedMove;
  byte wEnemySelectedMove;
  byte wLinkBattleRandomNumberListIndex;
  byte wAICount;
  char Padding5[2];
  byte wEnemyMoveListIndex;
  word wLastSwitchInEnemyMonHP;
  char wTotalPayDayMoney[3];
  byte wSafariEscapeFactor;
  byte wSafariBaitFactor;
  byte Padding6;
  word wTransformedEnemyMonOriginalDVs;
  byte wMonIsDisobedient;
  byte wPlayerDisabledMoveNumber;
  byte wEnemyDisabledMoveNumber;
  byte wInHandlePlayerMonFainted;
  byte wPlayerUsedMove;
  byte wEnemyUsedMove;
  byte wEnemyMonMinimized;
  byte wMoveDidntMiss;
  byte wPartyFoughtCurrentEnemyFlags;
  byte wLowHealthAlarmDisabled;
  byte wPlayerMonMinimized;
  char Padding7[13];
  word wEnemyBideAccumulatedDamage;
  char Padding8[8];
};

struct AnimData {
  byte wAnimationType;
  char padding[29];
  byte wAnimPalette;
};

union Union1 {
  char wVermilionDockTileMapBuffer[180];
  char wOaksAideRewardItemName[13];
  char wElevatorWarpMaps[22];
  char wFilteredBagItems[4];
  char wMonPartySpritesSavedOAM[96];
  char wTrainerCardBlkPacket[64];
  char wHallOfFame[96];
  char wNPCMovementDirections[180];
  struct DexRating DexRating;
  struct SlotMachineData SlotMachineData;
  struct AnimData AnimData;
  struct SwitchPartyTempData SwitchPartyMonTemp;
  char SimulatedJoypadStatesBuffer[120];
  struct MiscBattleData MiscBattleData;
};

typedef union Union6 Union6, * PUnion6;

typedef struct PPData PPData, * PPPData;

struct PPData {
  char Padding1[4];
  byte wStatusScreenCurrentPP;
  char Padding2[6];
  char wNormalMaxPPList[4];
  char Padding3[5];
};

union Union6 {
  char wcd6d[20];
  struct PPData field1;
};

typedef union Union5 Union5, * PUnion5;

union Union5 {
  byte wDownscaledMonSize;
  byte wNumMovesMinusOne;
};

typedef union Union4 Union4, * PUnion4;

union Union4 {
  byte wStandingOnWarpPadOrHole;
  byte wOAMBaseTile;
  byte wGymTrashCanIndex;
};

typedef union Union3 Union3, * PUnion3;

typedef struct TradedStats TradedStats, * PTradedStats;

typedef struct TradingInfo TradingInfo, * PTradingInfo;

typedef struct FieldMoveData FieldMoveData, * PFieldMoveData;

typedef struct BattleTransitionCircleData BattleTransitionCircleData, * PBattleTransitionCircleData;

typedef struct BattletransitionSpiralData BattletransitionSpiralData, * PBattletransitionSpiralData;

typedef struct SSAnneData SSAnneData, * PSSAnneData;

typedef struct HOFData HOFData, * PHOFData;

typedef struct RivalInfo RivalInfo, * PRivalInfo;

typedef struct FlyData FlyData, * PFlyData;

typedef struct FlyData2 FlyData2, * PFlyData2;

typedef struct HudData HudData, * PHudData;

typedef struct DayCareData DayCareData, * PDayCareData;

typedef struct SlotMachineData2 SlotMachineData2, * PSlotMachineData2;

typedef struct MiscSlotMachineData MiscSlotMachineData, * PMiscSlotMachineData;

typedef struct HiddenObjectData HiddenObjectData, * PHiddenObjectData;

typedef struct PlayerSpinInPlaceAnimData PlayerSpinInPlaceAnimData, * PPlayerSpinInPlaceAnimData;

typedef struct wPlayerSpinWhileMovingUpOrDownData wPlayerSpinWhileMovingUpOrDownData, * PwPlayerSpinWhileMovingUpOrDownData;

typedef struct TrainerGfxData TrainerGfxData, * PTrainerGfxData;

typedef struct TrainerInfoTextBoxData TrainerInfoTextBoxData, * PTrainerInfoTextBoxData;

typedef struct TextOptionsData TextOptionsData, * PTextOptionsData;

typedef struct BadgeGfxData BadgeGfxData, * PBadgeGfxData;

typedef struct CreditsData CreditsData, * PCreditsData;

typedef struct JigglypuffFacingData JigglypuffFacingData, * PJigglypuffFacingData;

typedef struct OverworldAnimData OverworldAnimData, * POverworldAnimData;

struct SlotMachineData2 {
  byte wStoppingWhichSlotMachineWheel;
  byte wSlotMachineWheel1Offset;
  byte wSlotMachineWheel2Offset;
  byte wSlotMachineWheel3Offset;
  byte wSlotMachineWheel1BottomTile;
  byte wSlotMachineWheel1MiddleTile;
  byte wSlotMachineWheel1TopTile;
  byte wSlotMachineWheel2BottomTile;
  byte wSlotMachineWheel2MiddleTile::_db;
  byte wSlotMachineWheel2TopTile;
  byte wSlotMachineWheel3BottomTile;
  byte wSlotMachineWheel3MiddleTile;
  byte wSlotMachineWheel3TopTile;
  word wPayoutCoins;
  byte wSlotMachineFlags;
  byte wSlotMachineWheel1SlipCounter;
  byte wSlotMachineWheel2SlipCounter;
  byte wSlotMachineRerollCounter;
  byte wSlotMachineBet;
};

struct BattletransitionSpiralData {
  byte wInwardSpiralUpdateScreenCounter;
  char Padding[9];
  byte wBattleTransitionSpiralDirection;
};

struct SSAnneData {
  byte wSSAnneSmokeDriftAmount;
  byte wSSAnneSmokeX;
};

struct TrainerInfoTextBoxData {
  byte wTrainerInfoTextBoxWidthPlus1;
  byte wTrainerInfoTextBoxWidth;
  byte wTrainerInfoTextBoxNextRowOffset;
};

struct FlyData {
  byte wFlyAnimUsingCoordList;
  char wFlyLocationsList[13];
};

struct TrainerGfxData {
  byte wTrainerSpriteOffset;
  byte wTrainerEngageDistance;
  byte wTrainerFacingDirection;
  byte wTrainerScreenY;
  byte wTrainerScreenX;
};

struct TradingInfo {
  byte wTradingWhichPlayerMon;
  byte wTradingWhichEnemyMon;
  char wNameOfPlayerMonToBeTraded[11];
};

struct CreditsData {
  byte Unused;
  byte wNumCreditsMonsDisplayed;
};

struct JigglypuffFacingData {
  byte Padding;
  byte Temp;
  char wJigglypuffFacingDirections[4];
};

struct wPlayerSpinWhileMovingUpOrDownData {
  byte wPlayerSpinWhileMovingUpOrDownAnimDeltaY;
  byte wPlayerSpinWhileMovingUpOrDownAnimMaxY;
  byte wPlayerSpinWhileMovingUpOrDownAnimFrameDelay;
};

struct BattleTransitionCircleData {
  byte wBattleTransitionCircleScreenQuadrantY;
  byte wBattleTransitionCircleScreenQuadrantX;
};

struct HiddenObjectData {
  byte wHiddenObjectFunctionArgument;
  byte wHiddenObjectFunctionRomBank;
  byte wHiddenObjectIndex;
  byte wHiddenObjectY;
  byte wHiddenObjectX;
};

struct TextOptionsData {
  byte wOptionsTextSpeedCursorX;
  byte wOptionsBattleAnimCursorX;
  byte wOptionsBattleStyleCursorX;
  byte wOptionsCancelCursorX;
};

struct HOFData {
  byte wHoFTeamIndex;
  byte wHoFPartyMonIndex;
  byte wHoFMonLevel;
  byte wHoFMonOrPlayer;
  byte wHoFTeamIndex2;
  byte wHoFTeamNo;
};

struct HudData {
  byte Padding;
  byte wHUDPokeballGfxOffsetX;
  char wHUDGraphicsTiles[3];
};

struct BadgeGfxData {
  byte wBadgeNumberTile;
  byte wBadgeNameTile;
  char wBadgeOrFaceTiles[9];
  byte padding;
  char wTempObtainedBadgesBooleans[8];
};

struct FlyData2 {
  byte wWhichTownMapLocation;
  byte wFlyAnimCounter;
  byte wFlyAnimBirdSpriteImageIndex;
};

struct FieldMoveData {
  char wFieldMoves[4];
  byte wNumFieldMoves;
  byte wFieldMovesLeftmostXCoord;
  byte wLastFieldMoveID;
};

struct TradedStats {
  byte wTradedPlayerMonSpecies;
  byte wTradedEnemyMonSpecies;
  char Padding[2];
  char wTradedPlayerMonOT[11];
  word wTradedPlayerMonOTID;
  char wTradedEnemyMonOT[11];
  word wTradedEnemyMonOTID;
};

struct RivalInfo {
  byte wRivalStarterTemp;
  byte wRivalStarterBallSpriteIndex;
};

struct PlayerSpinInPlaceAnimData {
  byte wPlayerSpinInPlaceAnimFrameDelay;
  byte wPlayerSpinInPlaceAnimFrameDelayDelta;
  byte wPlayerSpinInPlaceAnimFrameDelayEndValue;
  byte wPlayerSpinInPlaceAnimSoundID;
  char Padding1[6];
  byte Padding2;
  char wFacingDirectionList[4];
  char Padding3[3];
  byte wSavedPlayerScreenY;
  byte wSavedPlayerFacingDirection;
};

struct OverworldAnimData {
  char Padding1[16];
  byte wCutTile;
  char Padding2[2];
  byte wWhichAnimationOffsets;
};

struct MiscSlotMachineData {
  byte wCanPlaySlots;
  char Padding[1];
  word wTempCoins1;
  char Padding2[2];
  word wTempCoins2;
};

struct DayCareData {
  byte wDayCareStartLevel;
  byte wDayCareNumLevelsGrown;
  word wDayCareTotalCost;
  word wDayCarePerLevelCost;
};

union Union3 {
  struct TradedStats field0;
  struct TradingInfo field1;
  char wFallingObjectsMovementData[20];
  char wBoxMonCounts[12];
  char wPriceTemp[3];
  struct FieldMoveData field5;
  char wBoxNumString[3];
  struct BattleTransitionCircleData field7;
  word wBattleTransitionCopyTilesOffset;
  struct BattletransitionSpiralData field9;
  struct SSAnneData field10;
  struct HOFData field11;
  struct RivalInfo field12;
  struct FlyData field13;
  struct FlyData2 field14;
  struct HudData field15;
  struct DayCareData field16;
  struct SlotMachineData2 field17;
  struct MiscSlotMachineData field18;
  struct HiddenObjectData field19;
  struct PlayerSpinInPlaceAnimData field20;
  struct wPlayerSpinWhileMovingUpOrDownData field21;
  struct TrainerGfxData field22;
  struct TrainerInfoTextBoxData field23;
  struct TextOptionsData field24;
  struct BadgeGfxData field25;
  struct CreditsData field26;
  struct JigglypuffFacingData field27;
  struct OverworldAnimData field28;
  struct EmotionData field29;
  word wChangeBoxSavedMapTextPointer;
  union MiscUnion field31;
};

typedef struct MiscData MiscData, * PMiscData;

typedef union MiscUnion2 MiscUnion2, * PMiscUnion2;

typedef union MiscUnion3 MiscUnion3, * PMiscUnion3;

union MiscUnion2 {
  byte wFlashScreenLongCounter;
  byte wNumShootingBalls;
  byte wTradedMonMovingRight;
  byte wOptionsInitialized;
  byte wNewSlotMachineBallTile;
  byte wCoordAdjustmentAmount;
  byte wUnusedD08A;
};

union MiscUnion3 {
  byte wSlideMonDelay;
  byte wAnimCounter;
  byte wSubAnimTransform;
};

struct MiscData {
  union MiscUnion2 field0_0x0;
  union MiscUnion3 field1_0x1;
};

typedef struct OakAideData OakAideData, * POakAideData;

typedef union OaksAide OaksAide, * POaksAide;

union OaksAide {
  byte hOaksAideResult;
  byte hOaksAideRequirement;
};

struct OakAideData {
  union OaksAide field0_0x0;
  byte hOaksAideRewardItem;
  byte hOaksAideNumMonsOwned;
};

typedef struct TextFar TextFar, * PTextFar;

struct TextFar {
  byte TXT_FAR;
  word Address;
  byte Bank;
  byte TXT_END;
};

typedef union PlayerUnion PlayerUnion, * PPlayerUnion;

union PlayerUnion {
  byte hPlayerFacing;
  byte hPlayerYCoord;
  byte hPlayerXCoord;
};

typedef struct SpritateStateData1 SpritateStateData1, * PSpritateStateData1;

struct SpritateStateData1 {
  byte PictureID;
  byte MovementStatus;
  byte ImageIndex;
  byte YStepVector;
  byte YPixels;
  byte XStepVector;
  byte XPixels;
  byte IntraAnimFrameCounter;
  byte AnimFrameCounte;
  byte FacingDirection;
  byte YAdjusted;
  byte XAdjusted;
  byte CollisionData;
  char field13_0xd[3];
};

typedef struct GymGateData GymGateData, * PGymGateData;

struct GymGateData {
  byte hGymGateIndex;
  byte hGymGateAnswer;
};

typedef union OverworldMapUnion OverworldMapUnion, * POverworldMapUnion;

union OverworldMapUnion {
  char wOverworldMap[1300];
  char wTempPic[310];
};

typedef struct FoundItemData FoundItemData, * PFoundItemData;

struct FoundItemData {
  byte hItemAlreadyFound;
  char Padding[2];
  byte hDidntFindAnyHiddenObject;
};

typedef struct BoxData BoxData, * PBoxData;

struct BoxData {
  byte Species;
  word HP;
  byte BoxLevel;
  byte Status;
  byte Type1;
  byte Type2;
  byte CatchRate;
  char Moves[4];
  word OTID;
  char Exp[3];
  word HPExp;
  word AttackExp;
  word DefenseExp;
  word SpeedExp;
  word SpecialExp;
  word DVs;
  char PP[4];
};

typedef struct MonPatchList MonPatchList, * PMonPatchList;

struct MonPatchList {
  char wSerialPartyMonsPatchList[200];
  char wSerialEnemyMonsPatchList[200];
};

typedef struct RemainderData RemainderData, * PRemainderData;

struct RemainderData {
  char hQuotient[4];
  byte hRemainder;
};

typedef struct SwapItemData SwapItemData, * PSwapItemData;

struct SwapItemData {
  byte hSwapItemID;
  byte hSwapItemQuantity;
};

typedef struct FlyWarpSpec FlyWarpSpec, * PFlyWarpSpec;

struct FlyWarpSpec {
  byte MapID;
  byte Padding;
  word Address;
};

typedef struct SpecialWarpSpec SpecialWarpSpec, * PSpecialWarpSpec;

typedef struct FlyWrap FlyWrap, * PFlyWrap;

struct FlyWrap {
  word MapWidth;
  byte YBlocks;
  byte XBlocks;
  byte SubBlockY;
  byte SubBlockX;
};

struct SpecialWarpSpec {
  byte MapID;
  struct FlyWrap FlyWarp;
  byte Tileset;
};

typedef struct NpcData NpcData, * PNpcData;

typedef union MiscUnion12 MiscUnion12, * PMiscUnion12;

union MiscUnion12 {
  byte hNPCMovementDirections2Index;
  byte hNPCSpriteOffset;
  byte hNPCPlayerYDistance;
};

struct NpcData {
  union MiscUnion12 field0_0x0;
  byte hNPCPlayerXDistance;
  byte hFindPathNumSteps;
  byte hFindPathFlags;
  byte hFindPathYProgress;
  byte hFindPathXProgress;
  byte hNPCPlayerRelativePosPerspective;
  byte Padding;
  byte hNPCPlayerRelativePosFlags;
};

typedef struct CoinData CoinData, * PCoinData;

struct CoinData {
  byte hUnusedCoinsByte;
  word hCoins;
};

typedef struct ExpData.conflict ExpData.conflict, * PExpData.conflict;

struct ExpData.conflict{
    word wExpAmountGained;
    byte wGainBoostedExp;
};

typedef struct DivisionData DivisionData, * PDivisionData;

struct DivisionData {
  char hDividend[4];
  byte hDivisor;
  char hDivideBuffer[5];
};

typedef struct DexRatingNumMons DexRatingNumMons, * PDexRatingNumMons;

struct DexRatingNumMons {
  byte hDexRatingNumMonsSeen;
  byte hDexRatingNumMonsOwned;
};

typedef struct SpriteStateData2 SpriteStateData2, * PSpriteStateData2;

struct SpriteStateData2 {
  byte WalkAnimationCounter;
  byte Padding1;
  byte YDisplacement;
  byte XDisplacement;
  byte MapY;
  byte MapX;
  byte MovementByte1;
  byte GrassPriority;
  byte MovementDelay;
  byte OrigFacingDirection;
  char Padding2[3];
  byte PictureID;
  byte ImageBaseOffset;
  byte Padding3;
};

typedef struct VermilionDockExtraData VermilionDockExtraData, * PVermilionDockExtraData;

struct VermilionDockExtraData {
  char field0_0x0[3200];
};

typedef struct PrizeData PrizeData, * PPrizeData;

struct PrizeData {
  word wPrize1Price;
  word wPrize2Price;
  word wPrize3Price;
  byte Padding;
  char wLinkBattleRandomNumberList[10];
};

typedef union MiscUnion17 MiscUnion17, * PMiscUnion17;

typedef struct MultiplyData MultiplyData, * PMultiplyData;

struct MultiplyData {
  byte Padding1;
  char hMultiplicand[3];
  byte hMultiplier;
  byte Padding2;
  char hMultiplyBuffer[4];
};

union MiscUnion17 {
  struct MultiplyData field0;
  char hProduct[4];
  struct DivisionData field2;
  struct RemainderData field3;
};

typedef union MiscUnion16 MiscUnion16, * PMiscUnion16;

typedef struct SpriteScreenData SpriteScreenData, * PSpriteScreenData;

typedef struct CollidingSprite CollidingSprite, * PCollidingSprite;

struct CollidingSprite {
  byte hCollidingSpriteOffset;
  byte hCollidingSpriteTempYValue;
  byte hCollidingSpriteTempXValue;
  byte hCollidingSpriteAdjustedDistance;
};

struct SpriteScreenData {
  byte hSpriteOffset2;
  byte hOAMBufferOffset;
  byte hSpriteScreenX;
  byte hSpriteScreenY;
};

union MiscUnion16 {
  struct SpriteScreenData field0;
  struct CollidingSprite field1;
};

typedef union MiscUnion19 MiscUnion19, * PMiscUnion19;

union MiscUnion19 {
  char hMoney[3];
  struct CoinData field1;
};

typedef union MiscUnion18 MiscUnion18, * PMiscUnion18;

typedef struct MutateData MutateData, * PMutateData;

struct MutateData {
  byte Padding;
  byte hMutateWY;
  byte hMutateWX;
};

union MiscUnion18 {
  union MiscUnion17 field0;
  union MiscData4 field1;
  struct NpcData field2;
  struct SwapItemData field3;
  word hSignCoordPointer;
  struct MutateData field5;
  struct MiscData5 field6;
};

typedef union MiscUnion13 MiscUnion13, * PMiscUnion13;

union MiscUnion13 {
  byte hItemCounter;
  byte hSavedCoordIndex;
  byte hMissableObjectIndex;
  byte hGymTrashCanRandNumMask;
  byte hInteractedWithBookshelf;
};

typedef union MiscUnion15 MiscUnion15, * PMiscUnion15;

typedef struct SlidingData SlidingData, * PSlidingData;

typedef struct SpriteData SpriteData, * PSpriteData;

typedef struct DownArrowBlink DownArrowBlink, * PDownArrowBlink;

typedef struct MapWidths MapWidths, * PMapWidths;

typedef struct SpriteGfxData SpriteGfxData, * PSpriteGfxData;

typedef struct LoadSpriteTemp LoadSpriteTemp, * PLoadSpriteTemp;

typedef struct EnemySpeedData EnemySpeedData, * PEnemySpeedData;

typedef union MapEastNorthData MapEastNorthData, * PMapEastNorthData;

typedef union MapNorthSouthWidth MapNorthSouthWidth, * PMapNorthSouthWidth;

union MapNorthSouthWidth {
  byte hMapWidth;
  byte hNorthSouthConnectedMapWidth;
};

union MapEastNorthData {
  byte hMapStride;
  byte hEastWestConnectedMapWidth;
  byte hNorthSouthConnectionStripWidth;
};

struct MapWidths {
  union MapEastNorthData field0_0x0;
  union MapNorthSouthWidth field1_0x1;
};

struct DownArrowBlink {
  byte hDownArrowBlinkCount1;
  byte hDownArrowBlinkCount2;
};

struct EnemySpeedData {
  char field0_0x0[2];
  word hEnemySpeed;
};

struct SlidingData {
  byte hSlideAmount;
  byte hSlidingRegionSize;
  byte hSlideDirection;
};

struct SpriteData {
  byte hSpriteWidth;
  byte hSpriteHeight;
  byte hSpriteOffset;
};

struct SpriteGfxData {
  byte hSpriteScreenYCoord;
  byte hSpriteScreenXCoord;
  byte hSpriteMapYCoord;
  byte hSpriteMapXCoord;
};

struct LoadSpriteTemp {
  char field0_0x0[2];
  byte hLoadSpriteTemp1;
  byte hLoadSpriteTemp2;
};

union MiscUnion15 {
  struct MiscData3 field0;
  char hItemPrice[3];
  struct SlidingData field2;
  struct SpriteData field3;
  struct DownArrowBlink field4;
  struct MapWidths field5;
  struct SpriteGfxData field6;
  struct LoadSpriteTemp field7;
  struct EnemySpeedData field8;
};

typedef union MiscUnion14 MiscUnion14, * PMiscUnion14;

union MiscUnion14 {
  byte hBackupGymGateIndex;
  byte hUnlockedSilphCoDoors;
};

typedef union MiscUnion11 MiscUnion11, * PMiscUnion11;

union MiscUnion11 {
  byte hCollidingSpriteOffset;
  byte hCollidingSpriteTempYValue;
  byte hCollidingSpriteTempXValue;
  byte hCollidingSpriteAdjustedDistance::_db;
};

typedef union MiscUnion10 MiscUnion10, * PMiscUnion10;

union MiscUnion10 {
  byte hMapWidth;
  byte hNorthSouthConnectedMapWidth;
};

typedef struct GrassAndWaterData GrassAndWaterData, * PGrassAndWaterData;

struct GrassAndWaterData {
  byte wGrassRate;
  char wGrassMons[20];
  char Padding[8];
  byte wWaterRate;
  char wWaterMons[20];
};

typedef struct LinkMenuData LinkMenuData, * PLinkMenuData;

struct LinkMenuData {
  word wLinkMenuSelectionReceiveBuffer;
  char Padding1[3];
  word wLinkMenuSelectionSendBuffer;
  char Padding2[3];
  byte wLinkTimeoutCounter; // Also used for wEnteringCableClub
};

typedef union MiscUnion20 MiscUnion20, * PMiscUnion20;

typedef struct VendingMachineData VendingMachineData, * PVendingMachineData;

typedef struct ItemToRemoveData ItemToRemoveData, * PItemToRemoveData;

struct VendingMachineData {
  byte hVendingMachineItem;
  char hVendingMachinePrice[3];
};

struct ItemToRemoveData {
  byte hItemToRemoveID;
  byte hItemToRemoveIndex;
};

union MiscUnion20 {
  union PlayerUnion field0;
  struct OakAideData field1;
  struct VendingMachineData field2;
  struct GymGateData field3;
  struct DexRatingNumMons field4;
  struct ItemToRemoveData field5;
  union MiscUnion13 field6;
};

typedef union MiscUnion22 MiscUnion22, * PMiscUnion22;

typedef struct SavedMapTextData SavedMapTextData, * PSavedMapTextData;

struct SavedMapTextData {
  byte field0_0x0;
  word hSavedMapTextPtr;
  byte field2_0x3;
};

union MiscUnion22 {
  struct SpriteGfxData field0;
  struct FoundItemData field1;
  struct SavedMapTextData field2;
};

typedef union MiscUnion21 MiscUnion21, * PMiscUnion21;

typedef struct DivideData DivideData, * PDivideData;

struct DivideData {
  byte hDividend2;
  byte hDivisor2;
  byte hQuotient2;
};

union MiscUnion21 {
  struct DivideData field0;
  byte hIsHiddenMissableObject;
};

typedef struct BattleData BattleData, * PBattleData;

struct BattleData {
  byte Species;
  word HP;
  byte PartyPos;
  byte Status;
  byte Type1;
  byte Type2;
  byte CatchRate;
  char Moves[4];
  word DVs;
  byte Level;
  word MaxHP;
  word Attack;
  word Defense;
  word Speed;
  word Special;
  char PP[4];
};

typedef struct ObjectsToShowOrHideData ObjectsToShowOrHideData, * PObjectsToShowOrHideData;

struct ObjectsToShowOrHideData {
  byte wObjectToHide;
  byte wObjectToShow;
};

typedef union LinkAndSerialExchangeUnion LinkAndSerialExchangeUnion, * PLinkAndSerialExchangeUnion;

typedef struct SerialExchangeData SerialExchangeData, * PSerialExchangeData;

struct SerialExchangeData {
  byte wSerialSyncAndExchangeNybbleReceiveData;
  byte wSerialExchangeNybbleReceiveData;
  char Padding1[3];
  byte wSerialExchangeNybbleSendData;
  char Padding2[4];
  word wUnknownSerialCounter;
};

union LinkAndSerialExchangeUnion {
  struct LinkMenuData LinkMenu;
  struct SerialExchangeData SerialExchange;
};

typedef struct PredefPointer PredefPointer, * PPredefPointer;

struct PredefPointer {
  byte Bank;
  word Address;
};

typedef struct ExpData ExpData, * PExpData;

struct ExpData {
  char Padding[29];
  word wExpAmountGained;
  byte wGainBoostedExp;
};

typedef struct StringBuffer StringBuffer, * PStringBuffer;

struct StringBuffer {
  char field0_0x0[29];
  char wStringBuffer[20];
};

typedef struct DungeanWarp DungeanWarp, * PDungeanWarp;

struct DungeanWarp {
  byte TargetMapID;
  byte SourceMapWarpID;
};

typedef struct PalletTown_ScriptPointers PalletTown_ScriptPointers, * PPalletTown_ScriptPointers;

struct PalletTown_ScriptPointers {
  pointer PalletTownDefaultScript;
  pointer PalletTownOakHeyWaitScript;
  pointer PalletTownOakWalksToPlayerScript;
  pointer PalletTownOakNotSafeComeWithMeScript;
  pointer PalletTownPlayerFollowsOakScript;
  pointer PalletTownDaisyScript;
  pointer PalletTownNoopScript;
};

typedef struct Name Name, * PName;

struct Name {
  char field0_0x0[11];
};

typedef struct TextSpeedOption TextSpeedOption, * PTextSpeedOption;

struct TextSpeedOption {
  byte XCoordInMenu;
  byte FrameDelayAfterPrint;
};

typedef union Union21 Union21, * PUnion21;

union Union21 {
  byte wNamingScreenType;
  byte wPartyMenuTypeOrMessageID;
  byte wTempTilesetNumTiles;
};

typedef union Union22 Union22, * PUnion22;

typedef struct SpiralBallsBaseData SpiralBallsBaseData, * PSpiralBallsBaseData;

struct SpiralBallsBaseData {
  byte wSpiralBallsBaseY;
  byte wSpiralBallsBaseX;
};

union Union22 {
  struct SpiralBallsBaseData field0;
  struct FallingObjectData field1;
  struct MiscData field2;
};

typedef union Union20 Union20, * PUnion20;

union Union20 {
  byte wDefaultMap;
  byte wMenuItemOffset;
  byte wAnimationID;
};

typedef union Union25 Union25, * PUnion25;

union Union25 {
  byte wLinkCableAnimBulgeToggle::;
  byte wIntroNidorinoBaseTile;
  byte wOutwardSpiralCurrentDirection;
  byte wDropletTile;
  byte wNewTileBlockID;
  byte wWhichBattleAnimTileset;
  byte wSquishMonCurrentDirection;
  byte wSlideMonUpBottomRowLeftTile;
};

typedef union Union26 Union26, * PUnion26;

union Union26 {
  byte wPokeBallCaptureCalcTemp;
  byte wPokeBallAnimData;
  byte wUsingPPUp;
  byte wMaxPP;
  byte wCalculateWhoseStats;
  byte wTypeEffectiveness;
  byte wMoveType;
  byte wNumSetBits;
  byte wd11e;
};

typedef union Union23 Union23, * PUnion23;

union Union23 {
  word wSubAnimSubEntryAddr;
  byte wSlotMachineAllowMatchesCounter;
};

typedef union Union24 Union24, * PUnion24;

union Union24 {
  byte wPartyMenuAnimMonEnabled;
  byte wTownMapSpriteBlinkingEnabled;
  byte wUnusedD09B;
};

typedef union Union29 Union29, * PUnion29;

typedef struct EnemyData EnemyData, * PEnemyData;

typedef struct PartyData PartyData, * PPartyData;

struct PartyData {
  struct BoxData field0_0x0;
  byte Level;
  word MaxHP;
  word Attack;
  word Defense;
  word Speed;
  word Special;
};

struct EnemyData {
  struct Name field0_0x0;
  byte field1_0xb;
  char field2_0xc[9];
  byte field3_0x15;
  char field4_0x16[7];
  struct PartyData field5_0x1d;
  struct PartyData field6_0x49;
  struct PartyData field7_0x75;
  struct PartyData field8_0xa1;
  struct PartyData field9_0xcd;
  struct PartyData field10_0xf9;
  struct Name field11_0x125;
  struct Name field12_0x130;
  struct Name field13_0x13b;
  struct Name field14_0x146;
  struct Name field15_0x151;
  struct Name field16_0x15c;
  struct Name field17_0x167;
  struct Name field18_0x172;
  struct Name field19_0x17d;
  struct Name field20_0x188;
  struct Name field21_0x193;
  struct Name field22_0x19e;
};

union Union29 {
  struct GrassAndWaterData field0;
  struct EnemyData field1;
};

typedef union Union27 Union27, * PUnion27;

union Union27 {
  byte wDungeonWarpDataEntrySize;
  byte wWhichPewterGuy;
  byte wWhichPrizeWindow;
  byte wGymGateTileBlock;
};

typedef union Union28 Union28, * PUnion28;

union Union28 {
  char wSerialRandomNumberListBlock[17];
  struct PrizeData field1;
};

typedef struct MapWidthData MapWidthData, * PMapWidthData;

struct MapWidthData {
  byte hMapStride;
  byte hEastWestConnectedMapWidth;
  byte hNorthSouthConnectionStripWidth;
};

typedef union Union11 Union11, * PUnion11;

union Union11 {
  byte wSavedCoordIndex;
  byte wOakWalkedToPlayer;
  byte wNextSafariZoneGateScript;
};

typedef union Union14 Union14, * PUnion14;

union Union14 {
  char field0[48];
  struct StringBuffer field1;
  struct ExpData field2;
};

typedef union Union15 Union15, * PUnion15;

union Union15 {
  byte wHPBarType;
  byte wListMenuID;
};

typedef union Union12 Union12, * PUnion12;

union Union12 {
  byte wPlayerHPBarColor;
  byte wWholeScreenPaletteMonSpecies;
};

typedef union Union13 Union13, * PUnion13;

union Union13 {
  byte wCopyingSGBTileData::;
  byte wWhichPartyMenuHPBar;
  byte wPalPacket;
};

typedef union Union18 Union18, * PUnion18;

union Union18 {
  word wPlayerBideAccumulatedDamage;
  word wUnknownSerialCounter2;
  byte wPlayerNumHits;
};

typedef union Union19 Union19, * PUnion19;

union Union19 {
  char wAmountMoneyWon[3];
  struct ObjectsToShowOrHideData field1;
};

typedef union Union16 Union16, * PUnion16;

union Union16 {
  byte wRemoveMonFromBox;
  byte wMoveMonType;
};

typedef union Union17 Union17, * PUnion17;

union Union17 {
  char wTempMoveNameBuffer[14];
  char wLearnMoveMonName[11];
};

typedef struct SpriteOAM SpriteOAM, * PSpriteOAM;

struct SpriteOAM {
  byte YCoord;
  byte XCoord;
  byte TileID;
  byte Attributes;
};

typedef union TilemapUnion TilemapUnion, * PTilemapUnion;

union TilemapUnion {
  struct MonPatchList field0;
  char field1[360];
};

typedef union MiscUnion7 MiscUnion7, * PMiscUnion7;

union MiscUnion7 {
  byte hMapStride;
  byte hEastWestConnectedMapWidth;
  byte hNorthSouthConnectionStripWidth;
};

typedef union MiscUnion8 MiscUnion8, * PMiscUnion8;

union MiscUnion8 {
  byte hSpriteImageIndex;
  byte hSpriteFacingDirection;
  byte hSpriteMovementByte2;
};

typedef union MiscUnion9 MiscUnion9, * PMiscUnion9;

union MiscUnion9 {
  byte hSpriteOffset2;
  byte hOAMBufferOffset;
  byte hSpriteScreenX;
  byte hSpriteScreenY;
};

typedef enum cart_type {
  ROM_ONLY = 0,
  MBC1 = 1,
  MBC1_RAM = 2,
  MBC1_RAM_BATT = 3,
  MBC2 = 5,
  MBC2_BATT = 6,
  RAM = 8,
  RAM_BATT = 9,
  MMM01 = 11,
  MMM01_RAM = 12,
  MMM01_RAM_BATT = 13,
  MBC3_RTC_BATT = 15,
  MBC3_RAM_RTC_BATT = 16,
  MBC3 = 17,
  MBC3_RAM = 18,
  MBC3_RAM_BATT = 19,
  MBC5 = 25,
  MBC5_RAM = 26,
  MBC5_RAM_BATT = 27,
  MBC5_RUMBLE = 28,
  MBC5_RAM_RUMBLE = 29,
  MBC5_RAM_BATT_RUMBLE = 30,
  MBC6 = 32,
  MBC7 = 34,
  POCKET_CAMERA = 252,
  TAMA5 = 253,
  HUC3 = 254,
  HUC1 = 255
} cart_type;

typedef enum sgb_flag {
  NONE = 0,
  SUPPORT = 3
} sgb_flag;

typedef enum cgb_flag {
  NONE = 0,
  SUPPORT = 128,
  ONLY = 192
} cgb_flag;

typedef byte logo[48];

typedef enum rom_size {
  32K = 0,
  64K = 1,
  128K = 2,
  256K = 3,
  512K = 4,
  1MB = 5,
  2MB = 6,
  4MB = 7,
  8MB = 8
} rom_size;

typedef union title_block title_block, * Ptitle_block;

typedef struct title_block_old title_block_old, * Ptitle_block_old;

typedef struct title_block_new title_block_new, * Ptitle_block_new;

struct title_block_old {
  undefined field0_0x0;
  undefined field1_0x1;
  undefined field2_0x2;
  undefined field3_0x3;
  undefined field4_0x4;
  undefined field5_0x5;
  undefined field6_0x6;
  undefined field7_0x7;
  undefined field8_0x8;
  undefined field9_0x9;
  undefined field10_0xa;
  undefined field11_0xb;
  undefined field12_0xc;
  undefined field13_0xd;
  undefined field14_0xe;
  enum cgb_flag cgb_flag;
};

struct title_block_new {
  char title[11];
  char manufacturer_code[4];
  enum cgb_flag cgb_flag;
};

union title_block {
  struct title_block_old old_format;
  struct title_block_new new_format;
};

typedef enum INPUT_BUTTON {
  NO_INPUT = 0,
  A_BUTTON = 1,
  B_BUTTON = 2,
  SELECT = 4,
  START = 8,
  D_RIGHT = 16,
  D_LEFT = 32,
  D_UP = 64,
  D_DOWN = 128
} INPUT_BUTTON;

typedef enum ram_size {
  NONE = 0,
  2KB = 1,
  8KB = 2,
  32KB = 3,
  128KB = 4,
  64KB = 5
} ram_size;

typedef struct header header, * Pheader;

typedef enum region {
  JAPAN = 0,
  WORLD = 1
} region;

struct header {
  union title_block title_block;
  char new_licensee_code[2];
  enum sgb_flag sgb_flag;
  enum cart_type cartridge_type;
  enum rom_size rom_size;
  enum ram_size ram_size;
  enum region region;
  byte old_licensee_code;
  byte mask_rom_version;
  byte header_checksum;
  word global_checksum;
};



byte LY;
byte IF;
byte IE;
byte LCDC;
SpriteOAM wShadowOAM;
undefined UNION_Union8_cee9;
char LAB_1fff + 1;
undefined1 hLoadedROMBank;
byte wGBC;
byte hJoyInput;
byte P1;
undefined1 LAB_1fff + 1;
byte DAT_c45c;
undefined UNION_LinkAndSerialExchangeUnion_cc3d;
undefined SpritateStateData1_c100;
byte DAT_d790;
byte wWalkBikeSurfState;
byte DAT_d79b;
undefined SUB_785b;
byte wJoyIgnore;
byte wPlayerMovingDirection;
byte wStepCounter;
undefined1 wd72c;
byte wNumberOfNoRandomBattleStepsLeft;
byte wIsInBattle;
byte wOutOfBattleBlackout;
undefined1 wd72d;
undefined1 wFlags_D733;
byte wNumberOfWarps;
string wWarpEntries;
undefined1 wd730;
undefined1 wWarpedFromWhichWarp;
undefined1 wWarpedFromWhichMap;
byte wCurMapTileset;
byte wLastMap;
byte wCurMapWidth;
MiscUnion15 hPreviousTileset;
byte wUnusedD366;
byte wCurrentMapWidth2;
byte wCurrentMapHeight2;
string wDestinationWarpID;
byte wCurrentMapScriptFlags;
byte wMapPalOffset;
byte wYCoord;
undefined1 hJoyPressed;
byte wXCoord;
Union4 wStandingOnWarpPadOrHole;
byte wPlayerDirection;
byte wCurMap;
undefined1 wDestinationMap;
undefined1 wd732;
byte NorthConnectedMap;
byte NortConnectedMapYAlignment;
byte NorthConnectedMapXAlignment;
word NorthConnectedMapViewPointer;
byte SouthConnectedMap;
byte SouthConnectedMapYAlignment;
byte wTilePlayerStandingOn;
byte SouthConnectedMapXAlignment;
word SouthConnectedMapViewPointer;
byte WestConnectedMap;
byte WestConnectedMapWidth;
byte WestConnectedMapYAlignment;
byte WestConnectedMapXAlignment;
word WestConnectedMapViewPointer;
byte EastConnectedMap;
byte EastConnectedMapWidth;
byte EastConnectedMapYAlignment;
byte EastConnectedMapXAlignment;
word EastConnectedMapViewPointer;
word wCurrentTileBlockMapViewPointer;
undefined1 hJoyHeld;
undefined1 wd72e;
undefined1 wd736;
byte wWalkCounter;
byte wSafariZoneGameOver;
byte wCurOpponent;
MiscUnion22 hItemAlreadyFound;
byte wFlags_0xcd60;
byte wCheckFor180DegreeTurn;
byte wPlayerLastStopDirection;
byte wNPCMovementScriptPointerTableNum;
char DAT_c448;
byte wNewSoundID;
byte wAudioFadeOutControl;
undefined caseD_d1;
byte hTileAnimations;
byte wWalkBikeSurfStateCopy;
undefined1 DAT_09e2;
byte wTilesetBank;
word wTilesetGfxPtr;
undefined LAB_0514;
OverworldMapUnion UNION_OverworldMapUnion_c6e8;
word WestConnectionStripSrc;
word WestConnectionStripDest;
byte WestConnectionStripLength;
word EastConnectionStripSrc;
word EastConnectionStripDest;
byte EastConnectionStripLength;
byte wCurMapHeight;
word NorthConnectionStripSrc;
word NorthConnectionStripDest;
word wCurMapDataPtr;
byte NorthConnectionStripLength;
byte NorthConnectedMapWidth;
word SouthConnectionStripSrc;
word SouthConnectionStripDest;
byte SouthConnectionStripLength;
byte SouthConnectedMapWidth;
byte wNumSigns;
string wSignCoords;
string wSignTextIDs;
SpritateStateData1 SpritateStateData1_c110;
byte wNumSprites;
byte wTileInFrontOfPlayer;
undefined1 wTileMap;
byte wYBlockCoord;
TilemapUnion wTileMapBackup;
byte wXBlockCoord;
byte LAB_1fff + 1;
word wMapViewVRAMPointer;
byte wYOffsetSinceLastSpecialWarp;
byte wXOffsetSinceLastSpecialWarp;
undefined1 hSCX;
undefined1 hSCY;
byte hRedrawRowOrColumnMode;
word hRedrawRowOrColumnDest;
string wRedrawRowOrColumnSrcTiles;
undefined DAT_c3b2;
word wTilesetBlocksPtr;
undefined1 hJoyReleased;
byte wUnusedCD3A;
byte wOverrideSimulatedJoypadStatesMask;
byte wSimulatedJoypadStatesIndex;
Union1 Union_180_Bytes;
string wChannelSoundIDs;
word wTilesetCollisionPtr;
word wCurMapScriptPtr;
SpriteStateData2 SpriteStateData2_c210;
MiscUnion18 hNPCSpriteOffset;
string wMapSpriteData;
string wMapSpriteExtraData;
byte wMapBackgroundTile;
byte wUnusedD119;
word wObjectDataPointerTemp;
byte wCurMapConnections;
byte wMapMusicROMBank;
byte wMapMusicSoundID;
byte wUpdateSpritesEnabled;
byte wSpriteSetID;
undefined1 caseD_db;
byte hMapROMBank;
byte wIgnoreInputCounter;
undefined1 wd728;
undefined1 hJoy5;
byte wPredefParentBank;
Union15 UNION_Union15_cf94;
Union26 wd11e;
byte wcf91;
byte wSpriteFlipped;
byte hStartTileID;
byte wFrequencyModifier;
byte wTempoModifier;
byte wTopMenuItemX;
byte wTopMenuItemY;
byte wMenuWatchedKeys;
byte wForcePlayerToChooseMon;
byte wMaxMenuItem;
byte wPartyCount;
byte wMenuWatchMovingOutOfBounds;
byte wPartyAndBillsPCSavedMenuItem;
byte wMonDataLocation;
byte wCurrentMenuItem;
byte wLastMenuItem;
Union24 UNION_Union24_d09b;
byte wMenuItemToSwap;
byte wBattleMonSpecies2;
byte wMenuWrappingEnabled;
string wPartySpecies;
byte wWhichPokemon;
PartyData wLoadedMon;
byte wMonHIndex;
undefined1 wd0b5;
byte wMonHSpriteDim;
word wMonHFrontSprite;
Union6 UNION_Union6_cd6d;
byte wMonHBaseHP;
word wSpriteInputPtr;
undefined1 SUB_4000;
undefined1 DAT_a000;
undefined1 DAT_a188;
undefined DAT_a310;
undefined1 DAT_a187;
undefined1 DAT_a30f;
undefined1 DAT_a497;
undefined2 DAT_3931;
byte hVBlankCopySize;
MiscUnion4 LAB_1fff + 1;
word hVBlankCopySource;
word hVBlankCopyDest;
undefined1 hAutoBGTransferEnabled;
byte hVBlankCopyDoubleSize;
word hVBlankCopyDoubleSource;
word hVBlankCopyDoubleDest;
undefined DAT_c418;
undefined DAT_c490;
word hVBlankCopyBGSource;
word hVBlankCopyBGDest;
byte hVBlankCopyBGNumRows;
undefined1 DAT_c4f2;
undefined LAB_0712;
undefined[39] ARRAY_1a55;
undefined DAT_c4a5;
undefined DAT_c4e1;
string wEnemyMonNick;
string wPlayerName;
byte hWhoseTurn;
string wBattleMonNick;
byte hUILayoutFlags;
string wRivalName;
undefined FUN_1a8c;
byte wLinkState;
undefined DAT_c4a4;
undefined DAT_c4b8;
byte TextCommandSounds;
undefined LAB_1fff + 1;
byte hClearLetterPrintingDelayFlags;
word wTextDest;
byte wLetterPrintingDelayFlags;
word hSPTemp;
word hAutoBGTransferDest;
undefined1 hAutoBGTransferPortion;
undefined1 DAT_1f19;
undefined1 DAT_1f29;
undefined1 DAT_1f39;
undefined1 DAT_9030;
byte hMovingBGTilesCounter1;
undefined1 DAT_9140;
byte wMovingBGTilesCounter2;
undefined ARRAY_df00;
byte SB;
byte SC;
byte TMA;
byte TAC;
undefined1 hWY;
undefined1 hSerialConnectionStatus;
undefined1 hSoftReset;
byte wAudioROMBank;
byte wAudioSavedROMBank;
byte STAT;
byte SCY;
byte SCX;
byte BGP;
byte wUnusedC000;
byte OBP0;
byte OBP1;
byte WY;
byte WX;
byte wLastMusicSoundID;
byte wDisableVBlankWYUpdate;
byte hVBlankOccurred;
byte hFrameCounter;
undefined1 wVBlankSavedROMBank;
undefined1 DAT_211f;
undefined1 DAT_2115;
undefined FUN_2117;
byte DAT_2120;
undefined1 DAT_211e;
undefined UNION_MiscUnion16_ff8f;
undefined1 hSerialSendData;
byte DIV;
undefined1 hSerialReceivedNewData;
byte hNewPartyLength;
undefined1 hSerialReceiveData;
undefined1 hSerialIgnoringInitialData;
undefined UNION_Union18_d074;
LinkAndSerialExchangeUnion UNION_LinkAndSerialExchangeUnion_cc3d;
undefined SUB_5103;
undefined SUB_5177;
undefined SUB_5879;
undefined1 hSavedROMBank;
byte wAudioFadeOutCounterReloadValue;
undefined FUN_24d6;
byte wSpriteCurPosX;
byte wSpriteCurPosY;
byte wSpriteWidth;
byte wSpriteHeight;
byte wSpriteLoadFlags;
byte wSpriteUnpackMode;
undefined rst00;
byte wSpriteInputBitCounter;
byte wSpriteOutputBitOffset;
word wSpriteOutputPtr;
word wSpriteOutputPtrCached;
byte wSpriteInputCurByte;
undefined UNK_27bf;
word wSpriteDecodeTable0Ptr;
word wSpriteDecodeTable1Ptr;
undefined FUN_27b7;
undefined SUB_e6c4;
undefined LAB_0c03 + 1;
SpriteStateData2 SpriteStateData2_c200;
byte NR50;
byte wSpriteIndex;
byte wFontLoaded;
byte wDoNotWaitForButtonPressAfterDisplayingText;
undefined FUN_2aba;
string wItemList;
word wItemListPointer;
byte DAT_d74b;
byte wBattleAndStartSavedMenuItem;
word wListPointer;
byte wBattleType;
undefined1 hJoy7;
byte wListCount;
byte wTextBoxID;
undefined1 DAT_c3f5;
byte wPredefBank;
Name Name_de06;
byte wListScrollOffset;
byte wMaxItemQuantity;
byte wMenuExitMethod;
undefined LAB_c470;
byte wItemQuantity;
undefined FUN_2e30;
undefined UNION_MiscUnion19_ff9f;
undefined DAT_c479;
string hDivideBCDQuotient;
undefined DAT_c471;
undefined DAT_c474;
string s_P_2f97;
undefined DAT_c3e1;
undefined DAT_c3f6;
byte wPrintItemPrices;
byte wIsKeyItem;
undefined DAT_421e;
byte wNameListType;
string s__303c;
byte wNPCMovementScriptBank;
byte wNPCMovementScriptFunctionNum;
undefined FUN_3140;
word wTrainerHeaderPtr;
byte wCurMapScript;
string wGymCityName;
string wGymLeaderName;
byte wTrainerHeaderFlagBit;
undefined UNION_Union7_cd81;
Union3 wTitleMonSpecies;
byte wMissableObjectIndex;
undefined1 wEnemyMonOrTrainerClass;
undefined1 wMissableObjectList;
byte wTrainerNo;
byte wCurEnemyLVL;
Union2 Union_39_Bytes;
word wEndBattleWinTextPointer;
word wEndBattleLoseTextPointer;
byte wEndBattleTextRomBank;
byte wBattleResult;
byte wGymLeaderNo;
undefined1 SUB_3434;
undefined SpriteStateData2_c200;
word wTrainerPicPointer;
undefined1 wTrainerClass;
string wPlayerMoney;
word wPlayerCoins;
byte wBankswitchHomeSavedROMBank;
byte wBankswitchHomeTemp;
undefined DAT_c43a;
byte wTwoOptionMenuID;
byte wNPCNumScriptedSteps;
undefined UNION_MiscUnion21_ffe5;
Union7 UNION_Union7_cd81;
byte wLowHealthAlarm;
word wUnusedCF8D;
undefined FUN_375d;
word wItemPrices;
Union14 UNION_Union14_cf2e;
undefined1 hJoy6;
undefined DAT_c4f2;
byte wOptions;
Union22 wOptionsInitialized;
byte wTileBehindCursor;
word wMenuCursorLocation;
byte wAutoTextBoxDrawingControl;
undefined DAT_c4b9;
byte wOnSGB;
byte wNumBagItems;
undefined SUB_7da5;
byte wPredefID;
word wPredefHL;
byte wTextPredefFlag;
word wCurMapTextPtr;
undefined FUN_3f22;
undefined1* LAB_2963 + 2;
undefined LAB_0a09;
char LAB_0700 + 1;
undefined1* LAB_07ff + 1;
undefined1* LAB_0821 + 1;
undefined rst20;
undefined caseD_2;
undefined UNK_7258;
undefined[16] TitleMons;
undefined DAT_c46d;
byte hCurrentSpriteOffset;
byte wGrassTile;
undefined1 hTilePlayerStandingOn;
undefined SpriteOAM_c328;
undefined1 DAT_d79b;
undefined UNION_MiscUnion20_ffdb;
undefined UNK_0fd6;
undefined UNK_11fd;
undefined UNK_227c;
undefined1 UNK_2c1f;
undefined UNK_39fb;
undefined UNK_45aa;
undefined UNK_45b1;
undefined UNK_4fd7;
undefined UNK_5380;
undefined UNK_55a7;
undefined UNK_584e;
undefined UNK_5980;
undefined UNK_5a80;
undefined UNK_6997;
undefined UNK_6dd9;
undefined UNK_72ac;
undefined UNK_778d;
undefined UNK_77c2;
undefined DAT_c3b6;
undefined DAT_c42e;
undefined DAT_c460;
undefined DAT_c4f6;
byte wInitListType;
string SaveScreenInfoText_Player;
undefined caseD_e8;
undefined[41] wBagItems;
undefined FUN_3eda;
undefined1 wBeatLorelei;
byte wStack;
byte hRandomAdd;
byte hFieldMoveMonMenuTopMenuItemX;
byte.TitleScreenPokemonLogoYScrolls;
undefined FUN_1523;
undefined FUN_1001;
Union29 caseD_3a;
byte wMenuJoypadPollCount;
byte wTradeCenterPointerTableIndex;
undefined FUN_21b5;
undefined1 DAT_c4eb;
undefined1 DAT_c4e1;
undefined LAB_0402;
undefined LAB_0408;
undefined LAB_05c4;
undefined LAB_0794;
undefined UNK_08c0;
undefined UNK_0968;
undefined UNK_56f0;
undefined UNK_571f;
undefined DAT_c4e2;
undefined.moveAhead2;
undefined FUN_074c;
undefined UNK_57c0;
undefined DAT_c455;
undefined DAT_c445;
Union20 wDefaultMap;
byte wNumHoFTeams;
byte wBagSavedMenuItem;
byte wPlayerMoveListIndex;
byte wSaveFileStatus;
undefined caseD_fc;
undefined1 DAT_c432;
undefined1 DAT_c45a;
undefined1 DAT_c482;
undefined LAB_c433;
byte wFilteredBagItemsCount;
TextFar WhereWouldYouLikeText;
TextFar PleaseWaitText;
TextFar LinkCanceledText;
word wObtainedBadges;
undefined[19] wPokedexOwned;
byte wPlayTimeMinutes;
byte wPlayTimeHours;
undefined DAT_c404;
undefined DAT_c419;
undefined DAT_c47d;
string TextSpeedOptionText_TextSpeed;
string BattleAnimationOptionText_Text;
string BattleStyleOptionText_Text;
string OptionMenuCancelText;
TextSpeedOption TextSpeedOptionData;
undefined DAT_c3dc;
undefined MBC1SRamBank;
undefined1 sPlayerName;
SpritateStateData1 SpritateStateData1_c100;
undefined CheckWarpsCollision + 1;
undefined UNK_7042;
TextFar OakSpeechText1;
TextFar OakSpeechText3;
TextFar IntroduceRivalText;
TextFar IntroducePlayerText;
TextFar OakSpeechText2;
string wBoxItems;
undefined[6] IntroFadePalettes;
undefined UNK_63bf;
undefined1 wDungeonWarpDestinationMap;
undefined1 wWhichDungeonWarp;
Union27 wDungeonWarpDataEntrySize;
FlyWrap DungeonWarpData;
SpecialWarpSpec NewGameWarp;
SpecialWarpSpec TradeCenterPlayerWarp;
SpecialWarpSpec TradeCenterFriendWarp;
SpecialWarpSpec ColosseumPlayerWarp;
SpecialWarpSpec ColosseumFriendWarp;
undefined1 wBoulderSpriteIndex;
FlyWarpSpec FlyWarpDataPtr;
undefined UNION_Union14_cf2e;
Union21 wPartyMenuTypeOrMessageID;
undefined UNK_679e;
undefined DAT_c406;
undefined DAT_c3e6;
undefined UNK_699f;
undefined UNK_69e7;
undefined DAT_c3f5;
undefined DAT_c472;
undefined UNK_6aa3;
byte wPlayerMonNumber;
undefined UNK_7092;
undefined UNK_7671;
Union8 UNION_Union8_cee9;
undefined DAT_c47c;
undefined DAT_8d86;
undefined DAT_8ac2;
undefined2 DAT_b097;
undefined UNK_7b4a;
undefined FUN_rom11__7b8e;
byte wDamageMultipliers;
undefined FUN_rom11__7ba0;
undefined1 DAT_a38b;
undefined UNK_7247;
undefined FUN_0009;
undefined UNK_7244;
undefined FUN_rom13__72a0;
undefined UNK_746c;
undefined UNK_7466;
undefined UNK_7678;
undefined[23] SLOT_POINTERS;
undefined UNK_79e5;
undefined UNK_7a09;
undefined1 UNK_8204;
char DAT_c42f;
undefined DAT_c3cb;
undefined DAT_c443;
byte UNK_c41b;
byte UNK_c428;
char UNK_c43c;
byte wBaseCoordX;
byte wBaseCoordY;
SpriteOAM SpriteOAM_c330;
SpriteOAM SpriteOAM_c360;
Union5 UNION_Union5_cd6c;
string wMoves;
string wMovesString;
PartyData PartyData_d273;
char LAB_0021;
undefined1 UNK_a321;
undefined FUN_0c2a;
char DAT_9523;
BattleData wEnemyMon;
byte wEnemyBattleStatus2;
byte wEnemyMoveNum;
byte wEnemyMoveEffect;
undefined UNK_6844;
undefined[915] TRADING_GRAPHICS;
SpriteOAM SpriteOAM_c318;
undefined DAT_c405;
undefined UNK_4277;
byte wFirstMonsNotOutYet;
byte wNumSafariBalls;
byte wPartyGainExpFlags;
byte wActionResultOrTookBattleTurn;
byte wPlayerBattleStatus3;
byte wEnemyBattleStatus3;
byte wPlayerToxicCounter;
byte wEnemyToxicCounter;
byte wEnemyStatsToDouble;
byte wEnemyStatsToHalve;
byte wEnemyBattleStatus1;
byte wEnemyDisabledMove;
byte wPlayerBattleStatus1;
byte wNumRunAttempts;
PartyData PartyData_d16b;
undefined UNK_6902;
undefined UNK_6919;
undefined1 DAT_c4c1;
undefined1 DAT_c4e9;
undefined1 DAT_c4c7;
undefined1 DAT_c4ef;
undefined1 DAT_c4ed;
undefined1 DAT_c4b9;
byte wPlayerNumAttacksLeft;
undefined UNK_4f12;
byte wCapturedMonSpecies;
byte wPseudoItemID;
undefined caseD_c4;
undefined DAT_c442;
byte wPlayerDisabledMove;
BattleData wBattleMonNick;
string WHICH_TECHNIQUE;
undefined DAT_c494;
undefined1 DAT_c49a;
undefined DAT_c4aa;
undefined UNK_6677;
string DISABLED_STR;
undefined UNK_4c05;
undefined FUN_rom15__5aa8;
undefined UNK_5afb;
undefined UNK_5ba3;
undefined UNK_5c57;
byte wPlayerMoveEffect;
byte wCriticalHitOrOHKO;
undefined UNK_5c7a;
undefined FUN_d177;
byte wPlayerMovePower;
byte wPlayerMoveType;
word wDamage;
byte wPlayerMoveNum;
byte wPlayerBattleStatus2;
undefined FUN_rom15__62f8;
undefined FUN_rom15__6324;
byte wPlayerMoveAccuracy;
byte wEnemyMoveAccuracy;
byte wMoveMissed;
undefined UNK_76cb;
string wMonHMoves;
byte wEnemyMonSpecies2;
string wEnemyMonBaseStats;
byte wMonHType1;
byte wMonHType2;
byte wMonHCatchRate;
byte wMonHBaseEXP;
undefined FUN_rom15__6e58;
byte wPlayerStatsToDouble;
byte wPlayerStatsToHalve;
undefined1 DAT_rom15__4000;
undefined UNK_7e0a;
undefined UNK_7e9a;
undefined UNK_56d3;
undefined OverworldLoopLessDelay + 1;
undefined UNK_6080;
undefined UNK_6098;
undefined FUN_rom15__6e9a;
Union28 UNION_Union28_d141;
undefined UNK_783f;
undefined UNK_751a;
undefined UNK_7528;
undefined UNK_7b3e;
undefined UNK_769f;
undefined UNK_7b64;
undefined UNK_7b74;
undefined UNK_7f07;
undefined UNK_7f0d;
undefined DAT_c477;
undefined[19] wPokedexSeen;
undefined1 DAT_c44f;
undefined1 SUB_c450;
undefined1 DAT_c451;
undefined1 DAT_c452;
undefined1 DAT_c453;
undefined1 DAT_c3ae;
undefined DAT_c3dd;
undefined DAT_c3ec;
undefined LAB_c462;
undefined1 DAT_c3b3;
undefined DAT_c4f4;
undefined1 DAT_c507;
undefined1 DAT_c442;
undefined1 DAT_c443;
undefined UNK_4448;
undefined DAT_c3b4;
undefined DAT_c3d1;
undefined DAT_c454;
undefined UNION_OverworldMapUnion_c6e8;
undefined DAT_e6e6;
undefined SoftReset;
undefined[1][1][1][1][1][1][190] POKEDEX_ORDER;
undefined1 DAT_c3fb;
undefined DAT_c3fc;
undefined DAT_c3cf;
undefined1 DAT_c3f0;
undefined UNK_5574;
Union12 UNION_Union12_cf1d;
undefined UNK_5655;
Union25 UNION_Union25_d09f;
undefined OverworldLoop + 2;
undefined UNK_5910;
undefined UNK_591b;
undefined UNK_5926;
undefined UNK_5931;
undefined UNK_593c;
undefined UNK_5947;
undefined caseD_e;
undefined UNK_5950;
undefined1 DAT_9c00;
undefined UNK_5959;
undefined UNK_7e39;
undefined UNK_481e;
undefined FUN_rom28__4140;
SpriteOAM SpriteOAM_c310;
SpriteOAM SpriteOAM_c350;
SpriteOAM SpriteOAM_c35c;
undefined DAT_c3c8;
undefined1 DAT_c3f1;
undefined DAT_c41a;
undefined DAT_c434;
undefined1 wEventFlags;
undefined UNK_47a9;
byte wOutwardSpiralTileMapPointer;
undefined UNK_4b73;
undefined UNK_42ca;
undefined UNK_42d1;
undefined* LAB_rom28__6dca + 1;
undefined1[2] wTownVisitedFlag;
SpriteOAM SpriteOAM_c380;
SpriteOAM SpriteOAM_c390;
byte wSymmetricSpriteOAMAttributes;
undefined UNK_5382;
undefined UNK_57c2;
undefined UNK_5b7c;
undefined UNK_5b7d;
undefined TradeMons;
undefined UNK_5b7e;
undefined UNK_5d64;
undefined UNK_5d65;
undefined UNK_5d59;
word SetPalFunctions;
byte wDefaultPaletteCommand;
Union13 UNION_Union13_cf2d;
byte wEnemyHPBarColor;
byte wStatusScreenHPBarColor;
byte BadgeBlkDataLengths;
undefined caseD_22;
string wPartyMenuHPBarColors;
byte hDisableJoypadPolling;
undefined caseD_7a;
undefined caseD_6d;
undefined caseD_72;
word.packetPointers;
undefined caseD_c8;
undefined caseD_ec;
undefined LAB_0a13;
undefined1 UNK_rom28__4a3d;
undefined1 FUN_1509 + 1;
byte UNK_rom28__4a3e;
undefined FUN_0001;
undefined1 UNK_120b;
undefined1 caseD_0 + 2;
byte ARRAY_01ae[83];
undefined1* ARRAY_01ae[93];
undefined1* ARRAY_01ae[95];
undefined1* ARRAY_01ae[349];
undefined FUN_1304;
undefined1* ARRAY_01ae[354];
undefined FUN_1306;
byte LAB_07ff + 2;
undefined1 LAB_08fe + 1;
undefined1* caseD_9 + 1;
undefined1* LAB_0aff + 1;
undefined1* LAB_1300;
logo caseD_e4;
undefined1 LAB_1302 + 1;
undefined FUN_020e;
undefined1 LAB_20ff;
byte UNK_fa06;
byte UNK_fb09;
byte UNK_fc0a;
byte UNK_fd0b;
undefined FUN_02fd;
undefined FUN_1012;
undefined caseD_6b;
undefined FUN_0f08;
undefined caseD_f;
undefined FUN_0912;
undefined FUN_1200;
undefined1 DAT_ff79;
undefined rst08;
undefined1 DAT_eaea;
undefined1 DAT_a9ea;
undefined1 LAB_rom28__60ea;
undefined1 LAB_10e9 + 1;
undefined UNK_4233;
undefined FUN_160b;
undefined LAB_3933;
undefined LoadDestinationWarpPosition;
undefined1* LAB_rom28__7fbf;
undefined1 LAB_1010;
undefined rst10;
undefined caseD_49;
char DAT_b523;
undefined FUN_rom28__4000;
byte DAT_b522;
undefined1 LAB_rom28__6000;
undefined FUN_b0c0;
byte wBoxCount;
undefined1 DAT_b523;
undefined1 DAT_ba4d;
byte wCurrentBoxNum;
undefined1 DAT_ba4c;
undefined1 DAT_a462;
undefined1 DAT_a8c4;
undefined1 DAT_ad26;
undefined1 DAT_b188;
undefined1 DAT_b5ea;
byte DAT_d751;
byte DAT_d752;
undefined1 wBeatGymFlags;
undefined UNK_5000;
undefined UNK_5001;
undefined UNK_5002;
undefined UNK_5003;
byte DAT_d78e;
byte wSafariZoneGateCurScript;
word wSafariSteps;
Union11 wOakWalkedToPlayer;
byte DAT_d792;
byte wSaffronGymCurScript;
undefined DAT_f721;
undefined2 LAB_1b21;
undefined1* LAB_1b21;
undefined FUN_0500;
byte DAT_d79a;
byte wGameCornerCurScript;
byte wOpponentAfterWrongAnswer;
undefined FUN_rom29__5914;
undefined1 DAT_d79a;
undefined caseD_ca;
undefined FUN_rom29__595f;
undefined FUN_rom29__5994;
undefined FUN_rom29__59c9;
undefined FUN_rom29__5a68;
byte DAT_d7a1;
byte DAT_d7a3;
byte wFossilMon;
undefined DAT_fe05;
byte DAT_d863;
byte wBrunosRoomCurScript;
byte DAT_d864;
byte wAgathasRoomCurScript;
byte DAT_d865;
byte wCeruleanCaveB1FCurScript;
undefined UNK_42f0;
byte LAB_1405 + 2;
byte LAB_1712;
byte LAB_1812;
byte LAB_1816 + 2;
undefined1 wCardKeyDoorY;
undefined1 wCardKeyDoorX;
byte wUnusedD5A3;
undefined FUN_1d17;
byte LAB_1c17;
byte DAT_c484;
byte DAT_c434;
byte DAT_c45a;
byte DAT_c45e;
byte DAT_c4ac;
byte DAT_c40c;
byte DAT_c458;
byte DAT_c460;
undefined1 wTileInFrontOfBoulderAndBoulderCollisionResult;
byte wDayCareInUse;
BoxData wDayCareMonSpecies;
undefined DAT_e74a;
undefined ARRAY_rom3__50dd;
byte DAT_9415;
byte wUnusedD71F;
undefined FUN_rom3__5a96;
byte DAT_d7e0;
undefined DAT_d790;
undefined UNK_209c;
undefined UNK_2109;
undefined LAB_35e4;
undefined UNK_47b6;
undefined UNK_55e2;
undefined UNK_56c7;
undefined UNK_5ffd;
undefined FUN_rom3__5a4c;
undefined UNK_612f;
undefined UNK_63d6;
undefined FUN_rom3__63c8;
undefined UNK_6594;
byte wEscapedFromBattle;
undefined FUN_rom3__5b14;
undefined FUN_rom3__6029;
undefined FUN_01ef;
undefined FUN_rom3__5fa5;
byte wRepelRemainingSteps;
undefined caseD_d7;
byte wEvoStoneItemID;
undefined FUN_rom3__645d;
byte switchdataD_rom3::55e1;
undefined FUN_rom3__6247;
undefined FUN_rom3__627f;
undefined FUN_rom3__5b01;
undefined FUN_rom3__5ae9;
undefined1 DAT_ff60;
byte wEnemyMonActualCatchRate;
byte wUnusedCD39;
byte wEvolutionOccurred;
undefined UNK_5f2e;
undefined UNK_6072;
undefined UNK_609b;
undefined UNK_60c4;
undefined GetPredefRegisters;
undefined UNK_71c5;
undefined UNK_7e2c;
Name Name_dd2a;
undefined FUN_rom3__68e0;
undefined DAT_9e17;
undefined1 hRedrawMapViewRowOffset;
undefined DAT_8fd0;
undefined FUN_8fc0;
undefined FUN_rom3__42d0;
undefined UNK_7060;
undefined UNK_708f;
undefined UNK_7097;
undefined[32] wMissableObjectFlags;
byte wMissableObjectCounter;
undefined TextScriptEnd;
byte wFBMode;
byte wFBTileCounter;
byte wSubAnimFrameDelay;
byte wNumFBTiles;
word wFBDestAddr;
word wSubAnimAddrPtr;
undefined UNK_50da;
undefined FUN_rom30__676d;
undefined UNK_607d;
byte wAnimSoundID;
undefined UNION_Union23_d096;
byte wSubAnimCounter;
undefined UNK_41f2;
undefined UNK_41f3;
undefined1 DAT_fff0;
undefined1* LAB_0876 + 1;
byte NR41;
byte NR10;
undefined1 LAB_3ae9 + 1;
undefined1 DAT_f212;
undefined UNK_4d90;
undefined* LAB_183d;
byte WAVE[6];
undefined FUN_3633;
undefined UNK_5e50;
byte switchdataD_rom30::4dcf;
Union23 UNION_Union23_d096;
undefined DAT_c424;
undefined AnimationBlinkMon;
undefined AnimationShowMonPic;
undefined UNK_56bf;
undefined UNK_5d3e;
undefined UNK_5d63;
SpriteOAM SpriteOAM_c398;
undefined UNK_5fb0;
undefined UNK_5fb2;
undefined LAB_0036;
undefined LAB_rom30__48ff + 1;
undefined caseD_42;
undefined LAB_rom30__5a00;
undefined LAB_3aff + 1;
undefined LAB_rom30__4600;
byte LAB_001b;
byte LAB_001a;
byte DAT_fafa;
byte DAT_b101;
byte DAT_b005;
undefined FUN_2a28;
undefined1 LAB_rom30__58ee;
char OverworldLoopLessDelay + 2;
undefined LAB_0026;
undefined2 LAB_0071 + 1;
undefined2 LAB_0095 + 1;
undefined2 ARRAY_01ae[85];
byte caseD_4 + 1;
undefined1 LAB_04ed + 1;
byte LAB_0625;
undefined1 LAB_06e9 + 2;
undefined1 LAB_06ee;
undefined1 LAB_06f9 + 1;
undefined1* LAB_21ff + 1;
undefined1 LAB_2504 + 2;
undefined1 LAB_25f5;
undefined1 LAB_2a22;
undefined1 LAB_2a22 + 2;
undefined1 LAB_2a26;
undefined1 FUN_2a28 + 1;
undefined1 LAB_38ff + 1;
undefined LAB_3901 + 1;
undefined FUN_391f;
undefined1* LAB_3f00;
undefined2 LAB_rom30__4d3a;
undefined1 LAB_rom30__6107;
byte LAB_rom30__6c00;
byte LAB_rom30__6c05;
undefined2 LAB_rom30__70fe + 2;
undefined1 UNK_f5ea;
undefined FUN_3cfc;
undefined FUN_3c00;
undefined JoypadLowSensitivity;
undefined UNK_0c20;
undefined* LAB_rom30__70fe + 2;
undefined DAT_3a00;
undefined caseD_0 + 1;
undefined1 DAT_ff70;
undefined1 DAT_ff78;
undefined LAB_0021;
undefined LAB_002d;
char* LAB_3c10;
undefined caseD_30;
char* LAB_rom30__402d;
undefined LAB_0023;
undefined LAB_0031;
undefined1* LAB_15ff + 1;
undefined FUN_3306;
undefined1* LAB_2001 + 1;
undefined1* LAB_3c10;
undefined1* LAB_rom30__402d;
undefined1* LAB_rom30__4032 + 1;
undefined FUN_31fe;
undefined FUN_1700;
undefined FUN_1704;
undefined ClearVram;
undefined1* LAB_3108;
undefined1* LAB_rom30__4030 + 1;
undefined caseD_2b;
undefined RunNPCMovementScript;
undefined* LAB_15ff + 1;
byte* LAB_rom30__4032 + 1;
undefined FUN_38d3;
undefined DAT_rom30__601c;
undefined LAB_1f08;
undefined LAB_rom30__4528;
undefined LAB_rom30__4630;
undefined LAB_rom30__4307 + 1;
undefined ARRAY_01ae;
undefined LAB_rom30__6046;
undefined LAB_0025;
undefined1* LAB_000f;
undefined2 LAB_0ffe + 2;
undefined1* LAB_1330;
undefined1* LAB_1408;
undefined2 LAB_1800;
undefined FUN_35ff;
undefined2 LAB_1908;
undefined1* LAB_1c00;
undefined1* LAB_1e00;
undefined1 ClearVram + 2;
undefined3 LAB_2028;
undefined2 LAB_2035 + 1;
undefined2 LAB_2042 + 1;
undefined2 LAB_21ff + 1;
undefined1* LAB_33ff + 1;
undefined2 LAB_3606 + 2;
undefined1* LAB_37ff + 1;
undefined1* LAB_3808;
undefined2 LAB_38ff + 1;
undefined FUN_1508;
undefined FUN_14fc;
undefined AddPartyMon;
undefined RedrawRowOrColumn;
undefined FUN_3709;
undefined DisplayPartyMenu;
undefined FUN_3905;
undefined LAB_0013;
undefined2 LAB_000f;
undefined1 LAB_103e + 2;
undefined2 LAB_1300;
undefined2 LAB_1330;
undefined2 LAB_1408;
undefined2 LAB_1c00;
undefined2 LAB_1e00;
undefined2 LAB_33ff + 1;
undefined2 LAB_37ff + 1;
undefined2 LAB_3808;
undefined UNK_2c72;
undefined1* LAB_0ffe + 2;
undefined1* LAB_1800;
undefined1* LAB_1908;
undefined* LAB_1e28;
undefined LAB_2028;
undefined1* LAB_2035 + 1;
undefined1* LAB_2042 + 1;
undefined1* LAB_3606 + 2;
undefined1* LAB_38ff + 1;
undefined VBlank;
undefined1* LAB_3628;
undefined1* LAB_3816 + 2;
undefined1* LAB_3820;
undefined FUN_3826;
undefined DAT_0a00;
undefined LAB_rom30__4400;
undefined LAB_rom30__4408;
undefined LAB_0027;
undefined LAB_0029;
undefined LAB_0c00;
undefined LAB_0d08;
undefined LAB_002f;
undefined BYTE_rom30__4e10;
undefined LAB_003f;
undefined BYTE_rom30__4e00;
undefined LAB_002b;
undefined LAB_1107 + 1;
undefined DAT_a000;
undefined Joypad;
undefined LAB_10ff + 1;
undefined OverworldLoop + 1;
undefined caseD_ee;
undefined LAB_003d;
undefined LAB_rom30__4200;
undefined LAB_rom30__4207 + 1;
undefined LAB_rom30__4210;
undefined DAT_rom30__4218;
undefined LAB_rom30__4220;
undefined LAB_rom30__4300;
undefined LAB_rom30__4410;
undefined1* LAB_000d;
undefined1* intr_vblank + 1;
undefined IsSpriteOrSignInFrontOfPlayer;
undefined1* CheckWarpsCollision + 2;
undefined1* LAB_1a00;
undefined1* LAB_1a0f + 1;
undefined1* LAB_1b00;
undefined1* LAB_1b07 + 1;
undefined1* LAB_2013 + 1;
undefined1* LAB_2026;
undefined1* LAB_2028;
undefined1* LAB_2038;
undefined LAB_2042 + 1;
undefined2 LAB_3310;
undefined1* LAB_rom30__402d + 2;
undefined FUN_0046;
undefined2 LAB_rom30__4032 + 1;
undefined2 LAB_rom30__403d + 2;
undefined1* LAB_rom30__4044;
undefined2 LAB_rom30__40ff + 1;
undefined2 LAB_rom30__4107 + 1;
undefined2 LAB_rom30__4110;
undefined2 LAB_rom30__4118;
undefined2 LAB_rom30__411f + 1;
undefined ScrollTextUpOneLine;
undefined FUN_0b06;
undefined caseD_31;
undefined LoadScreenTilesFromBuffer2;
undefined FUN_36fd;
undefined LAB_rom30__4418;
undefined3 LAB_2042 + 1;
undefined1* LAB_rom30__40ff + 1;
undefined* LAB_rom30__4118;
undefined FUN_3193;
undefined* LAB_2042 + 1;
undefined VBlankCopyBgMap;
undefined1* LAB_3310;
undefined1* LAB_rom30__4107 + 1;
undefined1* LAB_rom30__4110;
undefined1* LAB_rom30__4118;
undefined1* LAB_rom30__411f + 1;
undefined1* ClearVram + 2;
undefined LAB_0049;
undefined LAB_rom30__4908;
undefined LAB_rom30__4310;
undefined1* LAB_rom30__4043;
undefined LAB_003b;
undefined LAB_rom30__4a00;
undefined LAB_rom30__604a;
undefined LAB_rom30__4438;
undefined LAB_3b08;
undefined LAB_003c;
undefined LAB_rom30__4702;
undefined LAB_001a;
undefined DAT_4444;
undefined1* LAB_0f00;
undefined1* LAB_3000;
undefined1* LAB_3204 + 2;
undefined1* LAB_3540;
undefined1* LAB_rom30__4023;
undefined1* LAB_rom30__4049 + 1;
undefined FUN_3f05;
undefined StopAllSounds;
undefined PlayDefaultMusic;
undefined1* LAB_2310;
undefined caseD_e0;
undefined LAB_0043;
undefined1* LAB_3216 + 2;
undefined DAT_rom30__4c18;
undefined LAB_004b;
undefined LAB_2c10;
undefined1* LAB_2d1f + 1;
undefined1* LAB_2d2f + 1;
undefined1* LAB_3120;
undefined1* LAB_3c17 + 1;
undefined FUN_3126;
undefined LAB_rom30__6043;
undefined LAB_rom30__4910;
undefined LAB_rom30__4928;
undefined LAB_rom30__4938;
undefined LAB_rom30__4918;
undefined LAB_001d;
undefined LAB_001f;
undefined LAB_rom30__4b08;
undefined1 LAB_0026;
undefined LAB_2720;
undefined UNK_4c70;
undefined1* LAB_2001 + 2;
undefined1* LAB_2048;
undefined1* LAB_2048 + 2;
undefined1* LAB_2610;
undefined1* LAB_2618;
undefined1* LAB_2718;
byte LAB_2f16 + 2;
undefined1* LAB_3860;
undefined1* LAB_3e10;
undefined2 LAB_rom30__4c00;
undefined rst28;
SpriteOAM SpriteOAM_c33c;
undefined* LAB_3860;
undefined1* LAB_rom30__4c00;
undefined1 DAT_ff4e;
undefined1 LAB_0442 + 2;
undefined UNK_444b;
undefined* LAB_rom31__4444;
byte DAT_rom31__6d40;
char DAT_rom31__70e3;
byte DAT_rom31__7409;
byte DAT_rom31__76ba;
byte DAT_rom31__771c;
byte DAT_rom31__77a2;
undefined UNK_4dee;
byte LAB_33e2;
undefined LAB_33e2;
undefined2 DAT_rom31__4600;
undefined DAT_ecff;
undefined1 DAT_c45c;
undefined1 DAT_d790;
undefined FUN_22ff;
undefined DAT_f22d;
undefined FUN_rom31__46de;
undefined DAT_fcff;
undefined1 DAT_ff84;
undefined hDMARoutine;
undefined1 DAT_ff2f;
byte UNK_a8ff;
byte TIMA;
undefined FUN_2553;
undefined1 DAT_ff28;
undefined UNK_4da0;
undefined FUN_rom31__4e09;
undefined1 LAB_2600;
byte NR52;
undefined UNK_505c;
undefined1 DAT_ff2d;
undefined UNK_508f;
undefined1 DAT_f2d2;
byte DAT_38a2;
byte DAT_38a3;
byte NR30;
byte wMuteAudioAndPauseMusic;
byte NR51;
string wChannelFlags2;
string wChannelVibratoRates;
string wChannelFrequencyLowBytes;
string wChannelVibratoDelayCounterReloadValues;
string wChannelVibratoDelayCounters;
string wChannelNoteDelayCounters;
string wChannelFlags1;
string wChannelVibratoExtents;
undefined UNK_5b93;
string wChannelVolumes;
string wChannelOctaves;
string wChannelNoteDelayCountersFractionalPart;
byte wSavedVolume;
string wChannelNoteSpeeds;
string wChannelLoopCounters;
string wChannelReturnAddresses;
string wChannelPitchSlideLengthModifiers;
string wChannelPitchSlideTargetFrequencyHighBytes;
string wChannelPitchSlideTargetFrequencyLowBytes;
string wChannelCommandPointers;
word wSfxTempo;
word wMusicTempo;
byte wSfxWaveInstrument;
byte wMusicWaveInstrument;
string wChannelDutyCycles;
string wChannelDutyCyclePatterns;
byte wDisableChannelOutputWhenSfxEnds;
byte wStereoPanning;
undefined UNK_5b9b;
undefined UNK_4361;
byte[16] WAVE;
string wChannelPitchSlideFrequencySteps;
string wChannelPitchSlideFrequencyStepsFractionalPart;
string wChannelPitchSlideCurrentFrequencyFractionalPart;
string wChannelPitchSlideCurrentFrequencyHighBytes;
string wChannelPitchSlideCurrentFrequencyLowBytes;
byte NR12;
byte NR14;
byte NR22;
byte NR24;
byte NR32;
byte NR42;
word wSfxHeaderPointer;
byte NR44;
byte wSoundID;
undefined1 LAB_1408;
undefined FUN_1505;
undefined BoxData_dc22;
undefined DAT_rom31__673e;
undefined2 LAB_3539 + 2;
undefined FUN_3408;
byte* LAB_353c + 1;
byte* LAB_353f;
undefined2 LAB_3f0b;
undefined2 LAB_3f0d;
undefined FUN_rom31__6740;
undefined FUN_3f0f;
undefined LAB_rom31__613f;
undefined1 DAT_b0dc;
byte DAT_b0c4;
byte DAT_b0c5;
byte DAT_b0c1;
undefined FUN_b0c2;
undefined DAT_fd26;
byte LAB_1103 + 1;
byte LAB_33ff + 1;
undefined FUN_31c7;
undefined FUN_rom31__43da;
char LAB_1107 + 1;
undefined LAB_111f;
undefined UNK_76e9;
undefined1 LAB_b7dc;
byte LAB_2507 + 1;
undefined UNK_78ed;
undefined FUN_rom31__790b;
char LAB_2509 + 1;
undefined DAT_c3e7;
undefined[29] TYPES_OF_STATUSES;
undefined FUN_225c;
undefined Delay3;
undefined1 DAT_ff08;
undefined UNK_2ae7;
undefined UNK_323c;
undefined UNK_6e07;
undefined UNK_6e61;
undefined UNK_6e73;
undefined FUN_rom4__72e8;
undefined1* LAB_04f8;
undefined1* LAB_0808;
undefined2 LAB_100d + 2;
undefined FUN_rom4__7296;
undefined FUN_1775;
undefined FUN_rom4__7259;
undefined FUN_rom4__7228;
undefined FUN_rom4__7222;
undefined FUN_rom4__720d;
undefined FUN_rom4__71e1;
undefined FUN_rom4__71ca;
undefined FUN_rom4__71a7;
undefined[1024] FONT_GRAPHICS;
undefined FUN_rom4__72e5;
byte hBackupGymGateIndex;
undefined FUN_rom4__72d4;
undefined1 DAT_c41d;
undefined1 DAT_c445;
undefined1 DAT_c46d;
string wSpriteSet;
undefined LAB_0706;
undefined1* LAB_0c0d;
undefined2 LAB_0d35;
byte wPalletTownCurScript;
byte DAT_d74a;
byte.HeyWaitDontGoOutText;
byte.ItsUnsafeText;
byte DAT_d74c;
byte wViridianCityCurScript;
byte DAT_d754;
byte wMuseum1FCurScript;
byte DAT_d755;
word PewterCityPlayerLeavingEastCoords;
byte MovementData_PewterMuseumGuyExit;
byte wPewterCityCurScript;
undefined UNK_5353;
byte wCeruleanCityCurScript;
undefined UNK_566d;
undefined UNK_5672;
byte DAT_d75a;
undefined UNK_5600;
undefined UNK_5608;
undefined UNK_5b47;
byte hRandomSub;
undefined wFirstLockTrashCanIndex;
undefined FUN_d803;
byte wVermilionCityCurScript;
undefined UNK_5823;
byte wBluesHouseCurScript;
byte DAT_d82a;
undefined UNK_5d58;
byte DAT_d82c;
undefined UNK_5f97;
byte DAT_d82e;
undefined UNK_61e3;
undefined UNK_7b6a;
char UNK_ff83;
undefined FUN_rom7__499c;
undefined UNK_4c9c;
undefined UNK_4ca3;
undefined UNK_4cb7;
undefined UNK_4cbf;
undefined UNK_4cef;
byte wOaksLabCurScript;
undefined1 wPlayerStarter;
undefined1 wRivalStarter;
undefined UNK_5152;
byte DAT_d7eb;
byte DAT_d74e;
undefined UNK_54e0;
undefined UNK_54ea;
undefined UNK_5c74;
undefined[1900] POKEMON_NAMES;
undefined1 DAT_ff51;
char DAT_rom8__65a1;
undefined1* LAB_2ffe + 1;
undefined1* LAB_20ff;
undefined2 DAT_b223;
undefined UNK_4fd3;
undefined FUN_rom8__503c;
undefined UNK_528f;
undefined UNK_52c2;
undefined UNK_53c0;
undefined[30] PC_OPTIONS_ITEMS;
undefined UNK_62de;
undefined UNK_62e6;
undefined DAT_e761;
undefined2 UNK_e75b;
undefined2 UNK_e75d;
undefined2 DAT_e75f;
undefined1 DAT_ff65;
undefined SUB_e611;
undefined FUN_rom8__513a;
undefined DAT_e612;
undefined DAT_e613;
undefined1 LAB_2f51;
byte LAB_rom8__6141;
undefined1 SUB_e611;
byte DAT_e612;
byte DAT_e613;
undefined1 LAB_1140 + 1;
undefined1 LAB_rom8__6141;
undefined FUN_rom8__7d6e;
BoxData BoxData_dc22;
undefined BoxData_dc43;
undefined1 LAB_2418;
byte DAT_rom9__422d;
char DAT_8cf1;
undefined UNK_4702;
byte DAT_ff82;
char DAT_9494;
undefined UNK_5c6a;
undefined1 DAT_ff69;
char DAT_ff82;
undefined FUN_ff53;
undefined2 DAT_baa4;
char* LAB_rom9__526d;
undefined[78] Unused_d66a;
undefined FUN_rom9__641b;
char DAT_ff85;
undefined1 DAT_a41a;
undefined LAB_26a3;
undefined UNK_4903;
undefined UNK_49c3;
undefined UNK_51cb;
undefined UNK_5287;
undefined1 caseD_97;
undefined DAT_rom9__48e4;
undefined UNK_449a;
undefined2 DAT_a452;
undefined2 DAT_a455;
undefined UNK_29b8;
undefined UNK_29cb;
undefined UNK_2a35;
undefined UNK_347c;
undefined UNK_4ee0;
undefined UNK_54c2;
undefined UNK_6918;
byte DAT_rom9__449a;
undefined UNK_4a6a;
undefined UNK_4a5c;
char DAT_rom9__5852;
undefined1* LAB_rom9__70a6;
undefined DAT_86de;
undefined LAB_rom9__7675;
undefined UNK_5009;
undefined UNK_57a5;
string TRAINER_NAMES;
undefined1 LAB_04a9 + 1;
undefined1 DAT_ff5e;
undefined DAT_d878;
undefined1 DAT_87e0;
undefined2 DAT_a64e;
undefined* DAT_a64a;
undefined* DAT_a64c;
byte DAT_aa58;
undefined UNK_4f90;
undefined UNK_5824;
undefined UNK_5a3a;
undefined UNK_5abc;
undefined FUN_a650;
char DAT_rom9__4e49;
undefined1 DAT_aa58;
undefined UNK_5789;
undefined DAT_9584;
undefined2 LAB_1209;
undefined1 DAT_rom9__7b39;
undefined UNK_7b39;
undefined2 LAB_07d5;
undefined2 DAT_rom9__4897;
byte DAT_8963;
undefined1 LAB_2064 + 1;
undefined DAT_efc5;
byte DAT_9796;
undefined UNK_4a83;
byte DAT_9788;
undefined UNK_4899;
char DAT_9e36;
byte DAT_eb43;
undefined DAT_e1b9;
undefined DAT_e1bb;
undefined NewBattle;
undefined1* ARRAY_01ae[133];
undefined[54] TYPE_NAME_POINTERS;