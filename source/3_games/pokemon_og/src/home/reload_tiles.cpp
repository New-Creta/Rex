// =========================================================
// ReloadMapData
// =========================================================

uint8_t hLoadedROMBank;
uint8_t wCurMap;
uint8_t MBC1RomBank;
uint8_t wd72e;
uint8_t wDoNotWaitForButtonPressAfterDisplayingText;

void SwitchToMapRomBank();
void DisableLCD();
void EnableLCD();
void LoadTextBoxTilePatterns();
void LoadCurrentMapView();
void LoadTilesetTilePatternData();

// far call stubs
void LoadTownMap_Fly();

void ReloadMapData()
{
  uint8_t savedBank = hLoadedROMBank;

  uint8_t map = wCurMap;
  SwitchToMapRomBank();

  DisableLCD();

  LoadTextBoxTilePatterns();
  LoadCurrentMapView();
  LoadTilesetTilePatternData();

  EnableLCD();

  hLoadedROMBank = savedBank;
  MBC1RomBank = savedBank;
}

// =========================================================
// ReloadTilesetTilePatterns
// =========================================================
void ReloadTilesetTilePatterns()
{
  uint8_t savedBank = hLoadedROMBank;

  uint8_t map = wCurMap;
  SwitchToMapRomBank();

  DisableLCD();

  LoadTilesetTilePatternData();

  EnableLCD();

  hLoadedROMBank = savedBank;
  MBC1RomBank = savedBank;
}

// =========================================================
// ChooseFlyDestination
// =========================================================
void ChooseFlyDestination()
{
  wd72e &= ~(1 << 4);
  LoadTownMap_Fly();
}

// =========================================================
// DisableWaitingAfterTextDisplay
// =========================================================
void DisableWaitingAfterTextDisplay()
{
  wDoNotWaitForButtonPressAfterDisplayingText = 1;
}