// ------------------------------------------------------------
// Helper primitives / assumed memory model
// ------------------------------------------------------------
using u8 = uint8_t;
using u16 = uint16_t;

extern u8  hSpriteIndex;
extern u8  wNPCMovementDirections[256];
extern u8  wNPCNumScriptedSteps;
extern u8  wd730;
extern u8  wOverrideSimulatedJoypadStatesMask;
extern u8  wSimulatedJoypadStatesEnd;
extern u8  wJoyIgnore;
extern u8  wUnusedCD3A;

extern u8  hDividend2, hDivisor2, hQuotient2;

// ------------------------------------------------------------
// CalcDifference
// ------------------------------------------------------------
bool CalcDifference(u8 a, u8 b, u8& out)
{
  int diff = int(a) - int(b);
  if (diff >= 0) {
    out = (u8)diff;
    return false; // no carry
  }
  out = (u8)(-diff);
  return true; // carry if a < b
}

// ------------------------------------------------------------
// MoveSprite
// ------------------------------------------------------------
extern u8* GetSpriteMovementByte1Pointer();

void SetSpriteMovementBytesToFF();

void MoveSprite(u8* de)
{
  SetSpriteMovementBytesToFF();
  MoveSprite_(de);
}

void MoveSprite_(u8* de)
{
  u8* hl = GetSpriteMovementByte1Pointer();
  *hl = 0;

  u8 c = 0;
  while (true)
  {
    u8 v = *de++;
    wNPCMovementDirections[c++] = v;
    if ((int8_t)v == -1) break;
  }

  wNPCNumScriptedSteps = c;

  wd730 |= 0x01;
  wOverrideSimulatedJoypadStatesMask = 0;
  wSimulatedJoypadStatesEnd = 0;
  wJoyIgnore = 0xFF;
  wUnusedCD3A = 0;
}

// ------------------------------------------------------------
// DivideBytes (very literal behavior emulation)
// ------------------------------------------------------------
u8 DivideBytes(u8 dividend, u8 divisor)
{
  if (divisor == 0) return 0;

  u8 q = 0;
  while (dividend >= divisor)
  {
    dividend -= divisor;
    q++;
  }
  return q;
}

// ------------------------------------------------------------
// UncompressMonSprite (bank selection logic preserved)
// ------------------------------------------------------------
enum {
  MEW = 0x15,
  FOSSIL_KABUTOPS = 0x2A,
  TANGELA = 0x1A,
  MOLTRES = 0x92,
  BEEDRILL = 0x2F,
  STARMIE = 0x98,
  NUM_POKEMON = 151
};

u8 wMonHeader[32];
u8 wSpriteInputPtr[2];
u8 wcf91;

u8 SelectBankForMon(u8 id)
{
  if (id == MEW) return BANK_MewPicFront;
  if (id == FOSSIL_KABUTOPS) return BANK_FossilKabutopsPic;

  if (id < TANGELA + 1) return BANK_PICS_1;
  if (id < MOLTRES + 1) return BANK_PICS_2;
  if (id < BEEDRILL + 2) return BANK_PICS_3;
  if (id < STARMIE + 1) return BANK_PICS_4;
  return BANK_PICS_5;
}

void UncompressSpriteData(u8 bank);

void UncompressMonSprite(u8* hlOffset)
{
  u8* base = wMonHeader;

  wSpriteInputPtr[0] = hlOffset[base + 0];
  wSpriteInputPtr[1] = hlOffset[base + 1];

  u8 bank = SelectBankForMon(wcf91);
  UncompressSpriteData(bank);
}

// ------------------------------------------------------------
// LoadMonFrontSprite
// ------------------------------------------------------------
extern u8 vFrontPic[];

void LoadUncompressedSpriteData(u8* de);

void LoadMonFrontSprite(u8* de)
{
  u8* hl = wMonHeader + 0xB;
  UncompressMonSprite(hl);

  u8 dims = wMonHeader[0x ? ]; // simplified placeholder
  LoadUncompressedSpriteData(de);
}

// ------------------------------------------------------------
// Sprite buffer helpers (simplified)
// ------------------------------------------------------------
u8 sSpriteBuffer0[0x300];
u8 sSpriteBuffer1[0x300];
u8 sSpriteBuffer2[0x300];

void ZeroSpriteBuffer(u8* hl, int size)
{
  for (int i = 0; i < size; i++) hl[i] = 0;
}

// ------------------------------------------------------------
// AlignSpriteDataCentered
// ------------------------------------------------------------
u8 hSpriteOffset, hSpriteWidth, hSpriteHeight;

void AlignSpriteDataCentered(u8* de, u8* hl)
{
  hl += hSpriteOffset;

  for (int col = 0; col < hSpriteWidth; col++)
  {
    u8* start = hl;
    for (int row = 0; row < hSpriteHeight; row++)
    {
      *hl++ = *de++;
    }
    hl = start + 7 * 8;
  }
}

// ------------------------------------------------------------
// InterlaceMergeSpriteBuffers
// ------------------------------------------------------------
u8 wSpriteFlipped;

void InterlaceMergeSpriteBuffers(u8* de)
{
  u8* dst = sSpriteBuffer2 + sizeof(sSpriteBuffer2) - 1;
  u8* src1 = sSpriteBuffer1 + sizeof(sSpriteBuffer1) - 1;
  u8* src0 = sSpriteBuffer0 + sizeof(sSpriteBuffer0) - 1;

  for (int i = 0; i < (int)(sizeof(sSpriteBuffer0) / 2); i++)
  {
    *dst-- = *src1--;
    *dst-- = *src0--;
    *dst-- = *src1--;
    *dst-- = *src0--;
  }

  if (wSpriteFlipped)
  {
    for (int i = 0; i < (int)sizeof(sSpriteBuffer1); i++)
      sSpriteBuffer1[i] = (sSpriteBuffer1[i] >> 4) | (sSpriteBuffer1[i] << 4);
  }

  // CopyVideoData(de, sSpriteBuffer1, ...)
}

// ------------------------------------------------------------
// DrawHPBar
// ------------------------------------------------------------
u8 wHPBarType;

void DrawHPBar(u8* hl, u8 d, u8 e, u8 c)
{
  *hl++ = 0x71;
  *hl++ = 0x62;

  u8* mid = hl;

  for (int i = 0; i < d; i++)
    *hl++ = 0x63;

  u8 right = (wHPBarType == 1) ? 0x6D : 0x6C;
  *hl = right;

  hl = mid;

  if (e == 0 && c == 0) return;

  if (e == 0) e = (c ? 1 : 0);

  while (e >= 8)
  {
    *hl++ = 0x6B;
    e -= 8;
  }

  if (e)
    *hl = 0x63 + e;
}

// ------------------------------------------------------------
// LoadMonData wrapper
// ------------------------------------------------------------
void LoadMonData()
{
  LoadMonData_(); // far call stub
}

void OverwritewMoves(u8 b, u8 c)
{
  extern u8 wMoves[4];
  wMoves[b] = c;
}

// ------------------------------------------------------------
// Front sprite loader
// ------------------------------------------------------------
u8 wd11e;

void LoadFrontSpriteByMonIndex();
void LoadMonFrontSprite(u8*);

void LoadFrontSpriteByMonIndex()
{
  u8 mon = wcf91;
  wd11e = mon;

  if (mon == 0 || mon > NUM_POKEMON)
    wcf91 = 1; // RHYDON fallback

  LoadMonFrontSprite(vFrontPic);
}

// ------------------------------------------------------------
// Cry system
// ------------------------------------------------------------
void PlaySound(u8 id);
void WaitForSoundToFinish();

struct CryEntry { u8 id, freq, tempo; };
extern CryEntry CryData[];

u8 GetCryData()
{
  u8 idx = wcf91 - 1;
  CryEntry e = CryData[idx];

  wFrequencyModifier = e.freq;
  wTempoModifier = e.tempo;

  return e.id;
}

void PlayCry(u8 mon)
{
  wcf91 = mon;
  u8 cry = GetCryData();
  PlaySound(cry);
  WaitForSoundToFinish();
}

// ------------------------------------------------------------
// Party menu (simplified structural translation)
// ------------------------------------------------------------
void PartyMenuInit();
void DrawPartyMenu();
void HandlePartyMenuInput();

void DisplayPartyMenu()
{
  hTileAnimations = 0;
  GBPalWhiteOutWithDelay3();
  ClearSprites();
  PartyMenuInit();
  DrawPartyMenu();
  HandlePartyMenuInput();
}