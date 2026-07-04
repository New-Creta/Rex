// ============================================================
// DisplayTextID (full pseudocode translation)
// ============================================================

void DisplayTextID() {
  uint8_t savedBank = hLoadedROMBank;
  push(savedBank);

  DisplayTextIDInit();

  if (!(wTextPredefFlag & 1)) {
    SwitchToMapRomBank(wCurMap);
  }
  wTextPredefFlag &= ~1;

  hFrameCounter = 30;

  uint8_t* hl = wCurMapTextPtr;

  uint8_t textID = hSpriteIndexOrTextID;
  wSpriteIndex = textID;

  // Special text handlers
  switch (textID) {
  case TEXT_START_MENU:       DisplayStartMenu(); return;
  case TEXT_SAFARI_GAME_OVER:  DisplaySafariGameOverText(); return;
  case TEXT_MON_FAINTED:      DisplayPokemonFaintedText(); return;
  case TEXT_BLACKED_OUT:      DisplayPlayerBlackedOutText(); return;
  case TEXT_REPEL_WORE_OFF:   DisplayRepelWoreOffText(); return;
  }

  // Sprite handling
  if (textID <= wNumSprites) {
    UpdateSpriteFacingOffsetAndDelayMovement();

    uint8_t spriteIndex = textID - 1;

    uint8_t* ptr = wMapSpriteData + spriteIndex * 2;
    textID = *(ptr + 1);

    hl = wCurMapTextPtr;
  }

  // Lookup text pointer in map table
  uint8_t* textPtrTable = hl;
  uint8_t entry = textID - 1;

  uint8_t* textAddr = *(uint16_t*)(textPtrTable + entry * 2);
  uint8_t firstByte = *textAddr;

  // Scripted text entry dispatch
  switch (firstByte) {
  case TX_SCRIPT_MART:
    DisplayPokemartDialogue(textAddr);
    break;

  case TX_SCRIPT_POKECENTER_NURSE:
    DisplayPokemonCenterDialogue(textAddr);
    break;

  case TX_SCRIPT_PLAYERS_PC:
    TextScript_ItemStoragePC(textAddr);
    break;

  case TX_SCRIPT_BILLS_PC:
    TextScript_BillsPC(textAddr);
    break;

  case TX_SCRIPT_POKECENTER_PC:
    TextScript_PokemonCenterPC(textAddr);
    break;

  case TX_SCRIPT_VENDING_MACHINE:
    VendingMachineMenu(textAddr);
    break;

  case TX_SCRIPT_PRIZE_VENDOR:
    TextScript_GameCornerPrizeMenu(textAddr);
    break;

  case TX_SCRIPT_CABLE_CLUB_RECEPTIONIST:
    CableClubNPC(textAddr);
    break;

  default:
    PrintText_NoCreatingTextBox(textAddr);
    break;
  }

  if (wDoNotWaitForButtonPressAfterDisplayingText == 0) {
    WaitForTextScrollButtonPress();
  }

AfterDisplayingTextID:

  if (wEnteringCableClub == 0) {
    WaitForTextScrollButtonPress();
  }

HoldTextDisplayOpen:

  while (Joypad(), (hJoyHeld & BIT_A_BUTTON)) {
    // hold open
  }

CloseTextDisplay:

  SwitchToMapRomBank(wCurMap);

  hWY = 0x90;
  DelayFrame();

  LoadGBPal();

  hAutoBGTransferEnabled = 0;

  // restore sprite facing directions
  for (int i = 0; i < 15; i++) {
    wSpriteStateData1_FacingDirection[i] =
      wSpriteStateData2_OrigFacingDirection[i];
  }

  hLoadedROMBank = BANK(InitMapSprites);
  InitMapSprites();

  wFontLoaded &= ~1;

  if (!(wd732 & (1 << 3))) {
    LoadPlayerSpriteGraphics();
  }

  LoadCurrentMapView();

  hLoadedROMBank = savedBank;
  UpdateSprites();
}

// ============================================================
// Pokemart Dialogue
// ============================================================

void DisplayPokemartDialogue(uint8_t* hl) {
  PrintText(PokemartGreetingText);

  hl++;

  LoadItemList(hl);

  wListMenuID = PRICEDITEMLISTMENU;

  DisplayPokemartDialogue_();

  goto AfterDisplayingTextID;
}

// ============================================================
// Item list loader
// ============================================================

void LoadItemList(uint8_t* hl) {
  wUpdateSpritesEnabled = 1;
  wItemListPointer = hl;

  uint8_t* de = wItemList;

  while (true) {
    *de++ = *hl++;
    if (*(de - 1) == 0xFF) break;
  }
}

// ============================================================
// Pokémon Center
// ============================================================

void DisplayPokemonCenterDialogue(uint8_t* hl) {
  hItemPrice = 0;
  hItemPrice2 = 0;
  hItemPrice3 = 0;

  hl++;

  DisplayPokemonCenterDialogue_();

  goto AfterDisplayingTextID;
}

// ============================================================
// Safari / faint / blackout / repel
// ============================================================

void DisplaySafariGameOverText() {
  PrintSafariGameOverText();
  goto AfterDisplayingTextID;
}

void DisplayPokemonFaintedText() {
  PrintText(PokemonFaintedText);
  goto AfterDisplayingTextID;
}

void DisplayPlayerBlackedOutText() {
  PrintText(PlayerBlackedOutText);

  wd732 &= ~(1 << 5);

  goto HoldTextDisplayOpen;
}

void DisplayRepelWoreOffText() {
  PrintText(RepelWoreOffText);
  goto AfterDisplayingTextID;
}

// ============================================================
// Text constants (placeholders)
// ============================================================

const char* PokemartGreetingText = nullptr;
const char* PokemonFaintedText = nullptr;
const char* PlayerBlackedOutText = nullptr;
const char* RepelWoreOffText = nullptr;