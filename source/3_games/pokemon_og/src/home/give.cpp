#include <cstdint>

// ============================================================================
// GiveItem
//
// ASM:
//   - Give player quantity c of item b
//   - Copy item name into wStringBuffer
//   - Return carry on success
// ============================================================================

extern uint8_t wd11e;
extern uint8_t wcf91;
extern uint8_t wItemQuantity;
extern uint8_t wNumBagItems;

extern uint8_t wStringBuffer[];

bool AddItemToInventory(uint8_t* hl);
const uint8_t* GetItemName(uint8_t item);
void CopyToStringBuffer(const uint8_t* de);

inline bool GiveItem(uint8_t item, uint8_t quantity)
{
  // ld a, b / store item id
  wd11e = item;
  wcf91 = item;

  // ld a, c / store quantity
  wItemQuantity = quantity;

  // hl = wNumBagItems
  uint8_t* hl = &wNumBagItems;

  // call AddItemToInventory
  bool success = AddItemToInventory(hl);

  if (!success)
    return false;

  // call GetItemName + CopyToStringBuffer
  const uint8_t* name = GetItemName(item);
  CopyToStringBuffer(name);

  return true; // scf
}

// ============================================================================
// GivePokemon
//
// ASM:
//   - Give player monster b at level c
//   - far call _GivePokemon
// ============================================================================

extern uint8_t wCurEnemyLVL;
extern uint8_t wMonDataLocation;

void _GivePokemon(); // banked routine in original engine

inline void GivePokemon(uint8_t species, uint8_t level)
{
  wcf91 = species;
  wCurEnemyLVL = level;

  // PLAYER_PARTY_DATA = 0
  wMonDataLocation = 0;

  _GivePokemon();
}