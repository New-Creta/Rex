

// =========================================================
// DisplayStartMenu (partial C++ translation)
// =========================================================

#include <cstdint>

// ---------------- RAM / hardware mocks ----------------
uint8_t hLoadedROMBank;
uint8_t MBC1RomBank;

uint8_t wWalkBikeSurfState;
uint8_t wWalkBikeSurfStateCopy;

uint8_t wCurrentMenuItem;
uint8_t wLastMenuItem;
uint8_t wBattleAndStartSavedMenuItem;

uint8_t hJoyPressed;

// ---------------- constants ----------------
constexpr uint8_t BIT_D_UP = 0;
constexpr uint8_t BIT_D_DOWN = 1;
constexpr uint8_t BIT_A_BUTTON = 0;
constexpr uint8_t BIT_B_BUTTON = 1;
constexpr uint8_t START = 0x80;
constexpr uint8_t B_BUTTON = 0x02;

// ---------------- stubs ----------------
void PlaySound(uint8_t);
void UpdateSprites();
uint8_t HandleMenuInput();
void EraseMenuCursor();
void PlaceUnfilledArrowMenuCursor();
void SaveScreenTilesToBuffer2();
void LoadTextBoxTilePatterns();
void CloseTextDisplay();
void Joypad();

// far calls
void DrawStartMenu();
void PrintSafariZoneSteps();

// event stub
bool CheckEvent(uint8_t);

// menu entry stubs
void StartMenu_Pokedex();
void StartMenu_Pokemon();
void StartMenu_Item();
void StartMenu_TrainerInfo();
void StartMenu_SaveReset();
void StartMenu_Option();

// sound
constexpr uint8_t SFX_START_MENU = 0x10;

// =========================================================
// DisplayStartMenu
// =========================================================
void DisplayStartMenu()
{
  uint8_t bank = /*BANK(StartMenu_Pokedex)*/ 0x01;

  hLoadedROMBank = bank;
  MBC1RomBank = bank;

  wWalkBikeSurfStateCopy = wWalkBikeSurfState;

  PlaySound(SFX_START_MENU);

  // fall through
}

// =========================================================
// RedisplayStartMenu
// =========================================================
void RedisplayStartMenu()
{
  DrawStartMenu();
  PrintSafariZoneSteps();
  UpdateSprites();

  while (true)
  {
    uint8_t input = HandleMenuInput();
    uint8_t b = input;

    // UP
    if (input & (1 << BIT_D_UP))
    {
      if (wCurrentMenuItem == 0 && wLastMenuItem == 0)
      {
        if (CheckEvent(/*EVENT_GOT_POKEDEX*/1))
          wCurrentMenuItem = 6;
        else
          wCurrentMenuItem = 5;

        EraseMenuCursor();
      }
      continue;
    }

    // DOWN
    if (input & (1 << BIT_D_DOWN))
    {
      uint8_t maxItem = CheckEvent(/*EVENT_GOT_POKEDEX*/1) ? 7 : 6;

      if (wCurrentMenuItem >= maxItem - 1)
      {
        wCurrentMenuItem = 0;
        EraseMenuCursor();
      }
      continue;
    }

    // BUTTON
    PlaceUnfilledArrowMenuCursor();
    wBattleAndStartSavedMenuItem = wCurrentMenuItem;

    if (b & (B_BUTTON | START))
    {
      // CloseStartMenu
      while (true)
      {
        Joypad();
        if (hJoyPressed & (1 << BIT_A_BUTTON))
          break;

        LoadTextBoxTilePatterns();
        return;
      }
    }

    SaveScreenTilesToBuffer2();

    uint8_t item = wCurrentMenuItem;

    if (!CheckEvent(/*EVENT_GOT_POKEDEX*/1))
    {
      item++;
    }

    switch (item)
    {
    case 0: StartMenu_Pokedex(); break;
    case 1: StartMenu_Pokemon(); break;
    case 2: StartMenu_Item(); break;
    case 3: StartMenu_TrainerInfo(); break;
    case 4: StartMenu_SaveReset(); break;
    case 5: StartMenu_Option(); break;
    default: break;
    }
  }
}