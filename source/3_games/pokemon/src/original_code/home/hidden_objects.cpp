#include <cstdint>

// ============================================================================
// UpdateCinnabarGymGateTileBlocks
//
// ASM:
//   farjp UpdateCinnabarGymGateTileBlocks_
// ============================================================================

void UpdateCinnabarGymGateTileBlocks_();

inline void UpdateCinnabarGymGateTileBlocks()
{
  UpdateCinnabarGymGateTileBlocks_();
}

// ============================================================================
// CheckForHiddenObjectOrBookshelfOrCardKeyDoor
// ============================================================================

extern uint8_t hLoadedROMBank;
extern uint8_t hJoyHeld;
extern uint8_t hDidntFindAnyHiddenObject;
extern uint8_t hInteractedWithBookshelf;
extern uint8_t hItemAlreadyFound;

extern uint8_t wHiddenObjectFunctionRomBank;

extern uint8_t MBC1RomBank;

constexpr uint8_t BIT_A_BUTTON = 0;

void CheckForHiddenObject();
void PrintBookshelfText();

inline void CheckForHiddenObjectOrBookshelfOrCardKeyDoor()
{
  uint8_t savedBank = hLoadedROMBank;

  // if A not pressed -> skip
  if ((hJoyHeld & (1 << BIT_A_BUTTON)) == 0)
  {
    hItemAlreadyFound = 0xFF;
    goto done;
  }

  // switch to hidden object bank
  {
    uint8_t bank = /* BANK(CheckForHiddenObject) */ 0;
    MBC1RomBank = bank;
    hLoadedROMBank = bank;

    CheckForHiddenObject();
  }

  if (hDidntFindAnyHiddenObject != 0)
  {
    // fallback: bookshelf text
    PrintBookshelfText();

    if (hInteractedWithBookshelf == 0)
    {
      hItemAlreadyFound = 0xFF;
      goto restore;
    }

    hItemAlreadyFound = 0xFF;
    goto restore;
  }

  // hidden object found -> call function pointer in HL
  {
    uint8_t bank = wHiddenObjectFunctionRomBank;

    MBC1RomBank = bank;
    hLoadedROMBank = bank;

    using Func = void(*)();
    Func hl = reinterpret_cast<Func>(0); // original HL function pointer

    hl();

    hItemAlreadyFound = 0;
    goto restore;
  }

done:
  hItemAlreadyFound = 0xFF;

restore:
  MBC1RomBank = savedBank;
  hLoadedROMBank = savedBank;
}