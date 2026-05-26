#include <cstdint>

// ============================================================================
// GetMonName
//
// ASM:
//   wd11e = monster id (1-based)
//   looks up name in MonsterNames table (10 bytes per entry)
//   writes result to wcd6d and appends '@'
// ============================================================================

extern uint8_t hLoadedROMBank;
extern uint8_t MBC1RomBank;

extern uint8_t wd11e;

extern uint8_t wcd6d[16];

extern const uint8_t MonsterNames[];

void AddNTimes();
void CopyData();

inline void GetMonName()
{
  uint8_t savedBank = hLoadedROMBank;

  hLoadedROMBank = /*BANK(MonsterNames)*/ 0;
  MBC1RomBank = hLoadedROMBank;

  uint8_t id = wd11e - 1;

  const uint8_t* hl = MonsterNames + (id * 10);

  uint8_t* de = wcd6d;

  for (int i = 0; i < 10; ++i)
    de[i] = hl[i];

  wcd6d[10] = '@';

  hLoadedROMBank = savedBank;
  MBC1RomBank = savedBank;
}

// ============================================================================
// GetItemName
// ============================================================================

extern uint8_t wd0b5;
extern uint8_t wNameListType;
extern uint8_t wPredefBank;

constexpr uint8_t ITEM_NAME = 1;
constexpr uint8_t HM01 = 0xF0;

void GetName();
void GetMachineName();

inline uint8_t* GetItemName()
{
  uint8_t item = wd11e;

  if (item >= HM01)
  {
    GetMachineName();
  }
  else
  {
    wd0b5 = item;
    wNameListType = ITEM_NAME;
    wPredefBank = /*BANK(ItemNames)*/ 0;
    GetName();
  }

  return wcd6d;
}

// ============================================================================
// GetMachineName
// ============================================================================

extern const char TechnicalPrefix[];
extern const char HiddenPrefix[];

constexpr uint8_t TM01 = 0xC0;
constexpr uint8_t NUM_HMS = 8;

inline void GetMachineName()
{
  uint8_t id = wd11e;
  uint8_t original = id;

  const char* prefix;
  uint8_t prefixLen = 2;

  if (id >= TM01)
  {
    prefix = TechnicalPrefix;
  }
  else
  {
    id += NUM_HMS;
    wd11e = id;
    prefix = HiddenPrefix;
  }

  // write prefix
  wcd6d[0] = prefix[0];
  wcd6d[1] = prefix[1];

  uint8_t num = wd11e - (TM01 - 1);

  uint8_t tens = '0';

  while (num >= 10)
  {
    num -= 10;
    ++tens;
  }

  wcd6d[2] = tens;
  wcd6d[3] = '0' + num;
  wcd6d[4] = '@';

  wd11e = original;
}

// ============================================================================
// IsItemHM / IsMoveHM
// ============================================================================

inline bool IsItemHM(uint8_t a)
{
  if (a < TM01) return false;
  if (a < HM01) return true;
  return false;
}

inline bool IsMoveHM(uint8_t move)
{
  for (auto hm : HMMoves)
    if (hm == move) return true;

  return false;
}

// ============================================================================
// GetMoveName
// ============================================================================

extern uint8_t wd0b5;
extern uint8_t wNameListType;

constexpr uint8_t MOVE_NAME = 2;

inline uint8_t* GetMoveName()
{
  wNameListType = MOVE_NAME;
  wd0b5 = wd11e;

  wPredefBank = /*BANK(MoveNames)*/ 0;
  GetName();

  return wcd6d;
}

// ============================================================================
// Data
// ============================================================================

const char TechnicalPrefix[] = "TM";
const char HiddenPrefix[] = "HM";