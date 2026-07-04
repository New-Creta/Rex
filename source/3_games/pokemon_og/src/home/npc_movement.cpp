// not zero if an NPC movement script is running, player is forced moving, or input is simulated
uint8_t wNPCMovementScriptPointerTableNum;
uint8_t wd736;
uint8_t wd730;

bool IsPlayerCharacterBeingControlledByGame()
{
  if (wNPCMovementScriptPointerTableNum != 0)
    return true;

  if (wd736 & (1 << 1)) // stepping down from door
    return true;

  if (wd730 & 0x80) // joypad simulation active
    return true;

  return false;
}

void RunNPCMovementScript()
{
  if (wd736 & (1 << 0))
  {
    wd736 &= ~(1 << 0);
    PlayerStepOutFromDoor();
    return;
  }

  if (wNPCMovementScriptPointerTableNum == 0)
    return;

  uint8_t index = wNPCMovementScriptPointerTableNum - 1;
  uint8_t tableIndex = index * 2;

  const uint16_t* table = NPCMovementScriptPointerTables[tableIndex / 2];

  uint8_t savedBank = hLoadedROMBank;

  hLoadedROMBank = wNPCMovementScriptBank;

  auto funcTable = (void(**)())table;

  CallFunctionInTable(wNPCMovementScriptFunctionNum, funcTable);

  hLoadedROMBank = savedBank;
}

const void* NPCMovementScriptPointerTables[] =
{
    PalletMovementScriptPointerTable,
    PewterMuseumGuyMovementScriptPointerTable,
    PewterGymGuyMovementScriptPointerTable
};

void EndNPCMovementScript()
{
  _EndNPCMovementScript();
}

bool DebugPressedOrHeldB()
{
#ifdef _DEBUG
  if (!(wd732 & (1 << BIT_DEBUG_MODE)))
    return false;

  if (hJoyHeld & (1 << BIT_B_BUTTON))
    return true;

  if (hJoyPressed & (1 << BIT_B_BUTTON))
    return true;
#endif

  return false;
}