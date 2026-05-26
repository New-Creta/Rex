#include <cstdint>

// ============================================================================
// SubtractAmountPaidFromMoney
//
// ASM:
//   farjp SubtractAmountPaidFromMoney_
// ============================================================================

bool SubtractAmountPaidFromMoney()
{
  return SubtractAmountPaidFromMoney_();
}

// ============================================================================
// AddAmountSoldToMoney
//
// ASM:
//   adds BCD price to player money
//   updates UI
//   plays purchase sound
// ============================================================================

extern uint8_t wPlayerMoney[3];
extern uint8_t hMoney[3];

extern uint8_t wTextBoxID;

constexpr uint8_t MONEY_BOX = 0;
constexpr uint8_t SFX_PURCHASE = 0;

void AddBCDPredef(uint8_t* de, const uint8_t* hl, uint8_t c);
void DisplayTextBoxID();
void PlaySoundWaitForCurrent(uint8_t id);
void WaitForSoundToFinish();

inline void AddAmountSoldToMoney()
{
  uint8_t* de = &wPlayerMoney[2];
  const uint8_t* hl = &hMoney[2];
  uint8_t c = 3;

  AddBCDPredef(de, hl, c);

  wTextBoxID = MONEY_BOX;
  DisplayTextBoxID();

  PlaySoundWaitForCurrent(SFX_PURCHASE);
  WaitForSoundToFinish();
}

// ============================================================================
// RemoveItemFromInventory
// ============================================================================

extern uint8_t wWhichPokemon;
extern uint8_t wItemQuantity;

void RemoveItemFromInventory_(uint8_t* hl);

inline void RemoveItemFromInventory(uint8_t* hl)
{
  RemoveItemFromInventory_(hl);
}

// ============================================================================
// AddItemToInventory
// ============================================================================

bool AddItemToInventory_(uint8_t* hl);

inline bool AddItemToInventory(uint8_t* hl)
{
  // push bc / pop bc is irrelevant in C++ (no state dependency here)
  return AddItemToInventory_(hl);
}