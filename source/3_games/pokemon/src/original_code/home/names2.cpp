#include <cstdint>
#include <cstring>

// ---- external data (ROM tables) ----
extern const uint8_t* NamePointers[]; // MonsterNames, MoveNames, etc.
extern uint8_t MonsterNames[];
extern uint8_t MoveNames[];
extern uint8_t UnusedBadgeNames[];
extern uint8_t ItemNames[];
extern uint8_t PlayerMonOT[];
extern uint8_t EnemyMonOT[];
extern uint8_t TrainerNames[];

// ---- RAM equivalents ----
uint8_t wd0b5;
uint8_t wd11e;
uint8_t wNameListType;
uint8_t wPredefBank;

uint8_t wcd6d[NAME_BUFFER_LENGTH];

uint8_t wUnusedCF8D;
uint8_t wUnusedCF8D_hi;

uint8_t hLoadedROMBank;
uint8_t hSwapTemp;
uint8_t hSwapTemp_hi;

// ---- provided helpers ----
extern void GetMonName();
extern void GetMachineName();
extern void CopyData(uint8_t* src, uint8_t* dst, uint16_t len);

// ---- GetName translation ----
void GetName()
{
  wd11e = wd0b5;

  if (wd0b5 >= 0xF0) // HM01 threshold
  {
    GetMachineName();
    return;
  }

  uint8_t savedBank = hLoadedROMBank;

  if (wNameListType == 1)
  {
    GetMonName();

    uint8_t* de = nullptr; // returned in original code
    de += NAME_LENGTH;

    // store pointer (not really used meaningfully in original)
    wUnusedCF8D = (uint16_t)de & 0xFF;
    wUnusedCF8D_hi = (uint16_t)de >> 8;

    hLoadedROMBank = savedBank;
    return;
  }

  // switch ROM bank (no-op in C++ version)
  hLoadedROMBank = wPredefBank;

  uint8_t tableIndex = (wNameListType - 1) * 2;

  uint8_t* basePtr =
    (uint8_t*)NamePointers[tableIndex / 2];

  const uint8_t* hl = basePtr;

  uint8_t target = wd0b5;
  uint8_t count = 0;

  const uint8_t* entryStart = hl;

  while (true)
  {
    entryStart = hl;

    // scan until '@'
    while (*hl != '@')
      hl++;

    hl++; // skip '@'
    count++;

    if (count == target)
      break;
  }

  // copy NAME_BUFFER_LENGTH bytes from entryStart to wcd6d
  memcpy(wcd6d, entryStart, NAME_BUFFER_LENGTH);

  wUnusedCF8D = (uintptr_t)entryStart & 0xFF;
  wUnusedCF8D_hi = ((uintptr_t)entryStart >> 8) & 0xFF;

  hLoadedROMBank = savedBank;
}