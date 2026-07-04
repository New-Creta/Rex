#include <cstdint>

// ============================================================================
// DisplayListMenuID
// Huge Pokémon-style menu system (item/party/PC/moves/etc)
// ============================================================================

extern uint8_t hAutoBGTransferEnabled;
extern uint8_t hJoy7;
extern uint8_t hLoadedROMBank;
extern uint8_t MBC1RomBank;

extern uint8_t wBattleType;
extern uint8_t wd730;
extern uint8_t wMenuItemToSwap;
extern uint8_t wListCount;
extern uint8_t wListPointer[2];
extern uint8_t wTextBoxID;
extern uint8_t wListMenuID;

extern uint8_t wMaxMenuItem;
extern uint8_t wTopMenuItemY;
extern uint8_t wTopMenuItemX;
extern uint8_t wMenuWatchedKeys;
extern uint8_t wMenuWatchMovingOutOfBounds;
extern uint8_t wCurrentMenuItem;
extern uint8_t wMenuCursorLocation[2];
extern uint8_t wListScrollOffset;
extern uint8_t wWhichPokemon;
extern uint8_t wcf91;
extern uint8_t wMaxItemQuantity;

constexpr uint8_t LIST_MENU_BOX = 0;
constexpr uint8_t ITEMLISTMENU = 1;
constexpr uint8_t MOVESLISTMENU = 2;
constexpr uint8_t PCPOKEMONLISTMENU = 3;

constexpr uint8_t BIT_A_BUTTON = 0;
constexpr uint8_t BIT_B_BUTTON = 1;
constexpr uint8_t BIT_SELECT = 2;
constexpr uint8_t BIT_D_DOWN = 3;
constexpr uint8_t BIT_D_UP = 4;

constexpr uint8_t A_BUTTON = 1 << 0;
constexpr uint8_t B_BUTTON = 1 << 1;
constexpr uint8_t SELECT = 1 << 2;

void BankswitchHome();
void BankswitchBack();
void DisplayTextBoxID();
void UpdateSprites();
void DelayFrames(uint8_t);
void Delay3();
void PrintListMenuEntries();
void LoadGBPal();
uint8_t HandleMenuInput();
void PlaceMenuCursor();
void PlaceUnfilledArrowMenuCursor();
void GetItemPrice();
void GetName();
void GetPartyMonName();
void CopyToStringBuffer();
void ExitListMenu();
void HandleItemListSwapping();

inline void DisplayListMenuID()
{
  hAutoBGTransferEnabled = 0;
  hJoy7 = 1;

  uint8_t bank = (wBattleType != 0)
    ? /* BANK(DisplayBattleMenu) */ 0
    : 0x01;

  BankswitchHome();

  wd730 |= (1 << 6);
  wMenuItemToSwap = 0;
  wListCount = 0;

  uint8_t* hl = &wListPointer[0];
  wListCount = hl[0];

  wTextBoxID = LIST_MENU_BOX;
  DisplayTextBoxID();

  UpdateSprites();

  wMenuWatchMovingOutOfBounds = 1;

  wMaxMenuItem = (wListCount < 2) ? 1 : 2;

  wTopMenuItemY = 4;
  wTopMenuItemX = 5;

  wMenuWatchedKeys = A_BUTTON | B_BUTTON | SELECT;

  DelayFrames(10);

  // ============================================================
  // Main loop
  // ============================================================

  while (true)
  {
    hAutoBGTransferEnabled = 0;

    PrintListMenuEntries();

    hAutoBGTransferEnabled = 1;
    Delay3();

    if (wBattleType == 0)
    {
      LoadGBPal();

      uint8_t input = HandleMenuInput();
      PlaceMenuCursor();

      if (!(input & A_BUTTON))
      {
        if (input & B_BUTTON)
        {
          ExitListMenu();
          return;
        }

        if (input & SELECT)
        {
          HandleItemListSwapping();
          continue;
        }

        if (input & BIT_D_DOWN)
        {
          if (wListScrollOffset + 3 <= wListCount)
            wListScrollOffset++;
          continue;
        }

        if (wListScrollOffset != 0)
          wListScrollOffset--;

        continue;
      }

      // A pressed
      PlaceUnfilledArrowMenuCursor();

      wMenuExitMethod = 1;
      wChosenMenuItem = 1;

      wMenuWatchMovingOutOfBounds = 0;

      uint8_t c = wCurrentMenuItem + wListScrollOffset;

      if (wListCount == 0)
      {
        ExitListMenu();
        return;
      }

      if (c > wListCount - 1)
      {
        ExitListMenu();
        return;
      }

      wWhichPokemon = c;

      uint8_t id = wListMenuID;

      // item list handling
      if (id == ITEMLISTMENU)
        c *= 2;

      uint8_t* base = &wListPointer[0];
      uint8_t item = base[0 + c];

      wcf91 = item;

      if (id == PCPOKEMONLISTMENU)
      {
        GetItemPrice();
      }
      else
      {
        if (id == ITEMLISTMENU)
          wMaxItemQuantity = base[1 + c];

        GetName();
      }

      CopyToStringBuffer();

      wMenuExitMethod = 0xFF;
      wChosenMenuItem = wCurrentMenuItem;

      hJoy7 = 0;
      wd730 &= ~(1 << 6);

      BankswitchBack();
      return;
    }

    // Old Man battle shortcut
    wCurrentMenuItem = 0;
    wMenuCursorLocation[0] = 0;
    wMenuCursorLocation[1] = 0;

    DelayFrames(80);

    PlaceUnfilledArrowMenuCursor();
  }
}

// ============================================================================
// DisplayChooseQuantityMenu
// ============================================================================

inline void DisplayChooseQuantityMenu()
{
  wItemQuantity = 0;

  while (true)
  {
    JoypadLowSensitivity();

    uint8_t pressed = hJoyPressed;

    if (pressed & A_BUTTON) return;
    if (pressed & B_BUTTON) return;

    if (pressed & BIT_D_UP)
    {
      if (++wItemQuantity > wMaxItemQuantity)
        wItemQuantity = 1;
    }

    if (pressed & BIT_D_DOWN)
    {
      if (--wItemQuantity == 0)
        wItemQuantity = wMaxItemQuantity;
    }
  }
}