#include <cstdint>

// ============================================================================
// GetItemPrice
//
// ASM:
//   Stores item's price as BCD at hItemPrice (3 bytes)
//   Input: wcf91 = item id
// ============================================================================

extern uint8_t hLoadedROMBank;
extern uint8_t MBC1RomBank;

extern uint8_t wListMenuID;
extern uint8_t wcf91;

extern uint8_t hItemPrice[3];
extern uint8_t wItemPrices[];

constexpr uint8_t MOVESLISTMENU = 0;
constexpr uint8_t HM01 = 0;

bool GetMachinePrice();

inline void GetItemPrice()
{
  uint8_t savedBank = hLoadedROMBank;

  uint8_t bank = (wListMenuID == MOVESLISTMENU)
    ? 0x0F
    : /* BANK(ItemPrices) */ 0;

  hLoadedROMBank = bank;
  MBC1RomBank = bank;

  // hl = pointer table (banked)
  const uint16_t basePtr =
    reinterpret_cast<uint16_t*>(wItemPrices)[0];

  const uint8_t* hl =
    reinterpret_cast<const uint8_t*>(basePtr);

  uint8_t a = wcf91;

  if (a >= HM01)
  {
    hLoadedROMBank = /* BANK(GetMachinePrice) */ 0;
    MBC1RomBank = hLoadedROMBank;

    GetMachinePrice();
  }
  else
  {
    const uint8_t* ptr = hl;

    while (a--)
      ptr += 3;

    // read 3-byte BCD (big endian)
    hItemPrice[0] = ptr[0];
    hItemPrice[1] = ptr[1];
    hItemPrice[2] = ptr[2];
  }

  // restore bank
  hLoadedROMBank = savedBank;
  MBC1RomBank = savedBank;
}