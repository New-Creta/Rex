#include <cstdint>

extern uint8_t hLoadedROMBank;
extern uint8_t MBC1RomBank;

extern uint8_t wBankswitchHomeTemp;
extern uint8_t wBankswitchHomeSavedROMBank;

// ============================================================================
// BankswitchHome
//
// ASM:
//   ; switches to bank # in a
//   ; Only use this when in the home bank
//
// Saves current ROM bank and switches to new bank.
// ============================================================================

inline void BankswitchHome(uint8_t bank)
{
  wBankswitchHomeTemp = bank;

  wBankswitchHomeSavedROMBank =
    hLoadedROMBank;

  hLoadedROMBank =
    wBankswitchHomeTemp;

  MBC1RomBank =
    wBankswitchHomeTemp;
}

// ============================================================================
// BankswitchBack
//
// ASM:
//   ; returns from BankswitchHome
//
// Restores previously saved ROM bank.
// ============================================================================

inline void BankswitchBack()
{
  hLoadedROMBank =
    wBankswitchHomeSavedROMBank;

  MBC1RomBank =
    wBankswitchHomeSavedROMBank;
}

// ============================================================================
// Bankswitch
//
// ASM:
//   ; self-contained bankswitch
//   ; use this when not in the home bank
//   ; switches to the bank in b
//
// Temporarily switches ROM bank,
// calls function,
// then restores previous bank.
// ============================================================================

using BankedFunction = void(*)();

inline void Bankswitch(
  uint8_t bank,
  BankedFunction function)
{
  // Save current bank.
  uint8_t previousBank =
    hLoadedROMBank;

  // Switch to requested bank.
  hLoadedROMBank = bank;
  MBC1RomBank = bank;

  // ASM:
  //   jp hl
  function();

  // Restore original bank.
  hLoadedROMBank = previousBank;
  MBC1RomBank = previousBank;
}