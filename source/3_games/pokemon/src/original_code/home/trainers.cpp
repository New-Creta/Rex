// ------------------------------------------------------------
// Trainer header pointer storage
// ------------------------------------------------------------

void StoreTrainerHeaderPointer(uint8_t h, uint8_t l)
{
  wTrainerHeaderPtr[0] = h;
  wTrainerHeaderPtr[1] = l;
}

// ------------------------------------------------------------
// Execute current map script from table
// ------------------------------------------------------------

void ExecuteCurMapScriptInTable(uint8_t a, uint8_t* de)
{
  uint8_t savedA = a;

  StoreTrainerHeaderPointer((uint8_t)(uintptr_t)de >> 8,
    (uint8_t)(uintptr_t)de);

  uint8_t override = wFlags_D733;

  if (override & (1 << 4))
  {
    wFlags_D733 &= ~(1 << 4);
    a = wCurMapScript;
  }
  else
  {
    a = savedA;
  }

  wCurMapScript = a;

  CallFunctionInTable(de, a);

  return wCurMapScript;
}

// ------------------------------------------------------------

void LoadGymLeaderAndCityName(uint8_t* de, uint8_t* hl)
{
  std::memcpy(wGymCityName, de, 0x11);
  std::memcpy(wGymLeaderName, hl, NAME_LENGTH);
}

// ------------------------------------------------------------

void ReadTrainerHeaderInfo(uint8_t a)
{
  uint16_t base = *(uint16_t*)wTrainerHeaderPtr;
  uint8_t* hl = (uint8_t*)(base + a);

  switch (a)
  {
  case 0:
    wTrainerHeaderFlagBit = *hl;
    break;

  case 2:
  case 4:
  case 6:
    hl = *(uint16_t*)hl;
    break;

  case 8:
    hl = *(uint16_t*)hl;
    break;

  case 0xA:
  {
    uint8_t lo = *hl++;
    uint8_t hi = *hl;
    hl = (uint8_t*)(((uint16_t)hi << 8) | lo);
  } break;
  }
}

// ------------------------------------------------------------

void TrainerFlagAction();
void PrintText();
void EngageMapTrainer();
void StartTrainerBattle();

// ------------------------------------------------------------

void TalkToTrainer()
{
  StoreTrainerHeaderPointerFromContext();

  ReadTrainerHeaderInfo(0);

  ReadTrainerHeaderInfo(2);

  uint8_t flagBit = wTrainerHeaderFlagBit;

  uint8_t flag = TrainerFlagAction(flagBit, FLAG_TEST);

  if (flag)
  {
    ReadTrainerHeaderInfo(6);
    PrintText();
    return;
  }

  ReadTrainerHeaderInfo(4);
  PrintText();

  ReadTrainerHeaderInfo(0xA);

  uint8_t* de_saved = wTempDE;

  ReadTrainerHeaderInfo(8);

  SaveEndBattleTextPointers(de_saved);

  wFlags_D733 |= (1 << 4);

  if (wFlags_0xcd60 & 1)
    return;

  EngageMapTrainer();
  wCurMapScript++;
  StartTrainerBattle();
}

// ------------------------------------------------------------

void CheckFightingMapTrainers()
{
  if (CheckForEngagingTrainers())
  {
    wFlags_D733 |= (1 << 3);

    wEmotionBubbleSpriteIndex = wSpriteIndex;
    wWhichEmotionBubble = EXCLAMATION_BUBBLE;

    TrainerWalkUpToPlayer_Bank0();

    wCurMapScript++;
  }
  else
  {
    wSpriteIndex = 0;
    wTrainerHeaderFlagBit = 0;
  }
}

// ------------------------------------------------------------

void StartTrainerBattle()
{
  wJoyIgnore = 0;

  InitBattleEnemyParameters();

  wd72d |= (1 << 6) | (1 << 7);
  wd72e |= (1 << 1);

  wCurMapScript++;
}

// ------------------------------------------------------------

void EndTrainerBattle()
{
  wCurrentMapScriptFlags |= (1 << 5) | (1 << 6);
  wd72d &= ~(1 << 7);
  wFlags_0xcd60 &= ~1;

  if (wIsInBattle == 0xFF)
  {
    ResetButtonPressedAndMapScript();
    return;
  }

  TrainerFlagAction(wTrainerHeaderFlagBit, FLAG_SET);

  if (wEnemyMonOrTrainerClass < OPP_ID_OFFSET)
  {
    HideObject(wSpriteIndex);
  }

  if (wd730 & (1 << 4))
    return;

  ResetButtonPressedAndMapScript();
}

// ------------------------------------------------------------

void ResetButtonPressedAndMapScript()
{
  wJoyIgnore = 0;
  hJoyHeld = 0;
  hJoyPressed = 0;
  hJoyReleased = 0;
  wCurMapScript = 0;
}

// ------------------------------------------------------------

void TrainerWalkUpToPlayer_Bank0()
{
  TrainerWalkUpToPlayer();
}

// ------------------------------------------------------------

void InitBattleEnemyParameters()
{
  wCurOpponent = wEngagedTrainerClass;
  wEnemyMonOrTrainerClass = wEngagedTrainerClass;

  if (wEngagedTrainerClass >= OPP_ID_OFFSET)
  {
    wTrainerNo = wEngagedTrainerSet;
  }
  else
  {
    wCurEnemyLVL = wEngagedTrainerSet;
  }
}

// ------------------------------------------------------------

void CheckForEngagingTrainers()
{
  uint8_t* de = (uint8_t*)wTrainerHeaderPtr;

  while (true)
  {
    StoreTrainerHeaderPointerFromDE(de);

    wSpriteIndex = *de;
    wTrainerHeaderFlagBit = *de;

    if (*de == 0xFF)
      return 0;

    ReadTrainerHeaderInfo(2);

    uint8_t flagBit = wTrainerHeaderFlagBit;

    uint8_t flag = TrainerFlagAction(flagBit, FLAG_TEST);

    if (!flag)
    {
      uint8_t dist = ReadTrainerHeaderInfo(0);
      wTrainerEngageDistance = *(uint8_t*)(dist + 1);

      wTrainerSpriteOffset = wSpriteIndex << 4;

      if (TrainerEngage())
        return 1;
    }

    de += 0xC;
  }
}

// ------------------------------------------------------------

void SaveEndBattleTextPointers(uint8_t* win, uint8_t* lose)
{
  wEndBattleTextRomBank = hLoadedROMBank;

  wEndBattleWinTextPointer = win;
  wEndBattleLoseTextPointer = lose;
}

// ------------------------------------------------------------

void EngageMapTrainer()
{
  uint8_t idx = wSpriteIndex - 1;

  wEngagedTrainerClass = wMapSpriteExtraData[idx].trainerClass;
  wEngagedTrainerSet = wMapSpriteExtraData[idx].trainerSet;

  PlayTrainerMusic();
}

// ------------------------------------------------------------

void PrintEndBattleText()
{
  if (!(wd72d & (1 << 7)))
    return;

  uint8_t bank = hLoadedROMBank;

  SwitchBank(wEndBattleTextRomBank);

  SaveTrainerName();
  PrintText(TrainerEndBattleText);

  SwitchBank(bank);

  FreezeEnemyTrainerSprite();
  WaitForSoundToFinish();
}

// ------------------------------------------------------------

void GetSavedEndBattleTextPointer()
{
  if (wBattleResult == 0)
    return wEndBattleWinTextPointer;

  return wEndBattleLoseTextPointer;
}

// ------------------------------------------------------------

void PlayTrainerMusic()
{
  if (wEngagedTrainerClass == OPP_RIVAL1 ||
    wEngagedTrainerClass == OPP_RIVAL2 ||
    wEngagedTrainerClass == OPP_RIVAL3)
    return;

  if (wGymLeaderNo != 0)
    return;

  StopAllMusic();

  uint8_t music = MUSIC_MEET_MALE_TRAINER;

  if (IsInList(EvilTrainerList, wEngagedTrainerClass))
    music = MUSIC_MEET_EVIL_TRAINER;
  else if (IsInList(FemaleTrainerList, wEngagedTrainerClass))
    music = MUSIC_MEET_FEMALE_TRAINER;

  wNewSoundID = music;
  PlaySound();
}