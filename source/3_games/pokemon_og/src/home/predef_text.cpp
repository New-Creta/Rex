// ------------------------------------------------------------
// PrintPredefTextID / Map text pointer handling (C++ translation)
// ------------------------------------------------------------

using u8 = uint8_t;
using u16 = uint16_t;

// RAM / hardware-like globals
u8  hSpriteIndexOrTextID;
u8  hSavedMapTextPtr[2];

u8  wTextPredefFlag;
u8  wCurMapTextPtr[2];

// External systems
void DisplayTextID();
extern const u8 TextPredefs[];

// ------------------------------------------------------------
// PrintPredefTextID
// ------------------------------------------------------------
void PrintPredefTextID(u8 a)
{
  hSpriteIndexOrTextID = a;

  const u8* hl = TextPredefs;
  SetMapTextPointer(hl);

  wTextPredefFlag |= 0x01;
  DisplayTextID();
}

// ------------------------------------------------------------
// RestoreMapTextPointer
// ------------------------------------------------------------
void RestoreMapTextPointer()
{
  wCurMapTextPtr[0] = hSavedMapTextPtr[0];
  wCurMapTextPtr[1] = hSavedMapTextPtr[1];
}

// ------------------------------------------------------------
// SetMapTextPointer
// ------------------------------------------------------------
void SetMapTextPointer(const u8* hl)
{
  hSavedMapTextPtr[0] = wCurMapTextPtr[0];
  hSavedMapTextPtr[1] = wCurMapTextPtr[1];

  wCurMapTextPtr[0] = (u16)hl & 0xFF;
  wCurMapTextPtr[1] = (u16)hl >> 8;
}

// ------------------------------------------------------------
// Data include placeholder
// ------------------------------------------------------------
// data/text_predef_pointers.asm -> assumed converted elsewhere
// extern const u8 TextPredefs[];