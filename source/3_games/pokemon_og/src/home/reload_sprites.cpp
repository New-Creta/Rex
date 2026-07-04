// =========================================================
// ReloadMapSpriteTilePatterns
// =========================================================

uint8_t wFontLoaded;
uint8_t wSpriteSetID;

// external engine stubs
void DisableLCD();
void EnableLCD();
void InitMapSprites();
void LoadPlayerSpriteGraphics();
void LoadFontTilePatterns();
void UpdateSprites();
void FarCall_InitMapSprites();

void ReloadMapSpriteTilePatterns()
{
  uint8_t savedFontLoaded = wFontLoaded;

  // res 0, [wFontLoaded]
  wFontLoaded &= ~(1 << 0);

  wSpriteSetID = 0;

  DisableLCD();

  FarCall_InitMapSprites();

  EnableLCD();

  wFontLoaded = savedFontLoaded;

  LoadPlayerSpriteGraphics();
  LoadFontTilePatterns();

  UpdateSprites();
}