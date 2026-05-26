#include <cstdint>
#include <cstring>

// -----------------------------
// Helpers / hardware stubs
// -----------------------------
uint8_t hLoadedROMBank;
uint8_t MBC1RomBank;
uint8_t MBC1SRamEnable;
uint8_t MBC1SRamBank;
constexpr uint8_t SRAM_ENABLE = 1;

// -----------------------------
// Memory / globals (simplified)
// -----------------------------
uint8_t wSpriteInputPtr[2];
uint8_t wSpriteInputBitCounter;
uint8_t wSpriteInputCurByte;

uint8_t wSpriteOutputPtr[2];
uint8_t wSpriteOutputPtrCached[2];

uint8_t wSpriteCurPosX;
uint8_t wSpriteCurPosY;

uint8_t wSpriteWidth;
uint8_t wSpriteHeight;

uint8_t wSpriteLoadFlags;
uint8_t wSpriteUnpackMode;

uint8_t wSpriteOutputBitOffset;

uint8_t wSpriteDecodeTable0Ptr[2];
uint8_t wSpriteDecodeTable1Ptr[2];

uint8_t wSpriteFlipped;

// buffers
static uint8_t sSpriteBuffer1[0x400];
static uint8_t sSpriteBuffer2[0x400];

// -----------------------------
// Memory helpers
// -----------------------------
void FillMemory(uint8_t* hl, uint16_t bc, uint8_t a) {
  std::memset(hl, a, bc);
}

// -----------------------------
// Input stream
// -----------------------------
uint8_t ReadNextInputByte() {
  uint16_t addr = wSpriteInputPtr[0] | (wSpriteInputPtr[1] << 8);
  uint8_t val = *((uint8_t*)addr);
  addr++;
  wSpriteInputPtr[0] = addr & 0xFF;
  wSpriteInputPtr[1] = addr >> 8;
  return val;
}

uint8_t ReadNextInputBit() {
  if (--wSpriteInputBitCounter == 0) {
    wSpriteInputCurByte = ReadNextInputByte();
    wSpriteInputBitCounter = 8;
  }
  uint8_t bit = wSpriteInputCurByte & 1;
  wSpriteInputCurByte >>= 1;
  return bit;
}

// -----------------------------
// Output pointer helpers
// -----------------------------
void StoreSpriteOutputPointer(uint8_t* ptr) {
  uintptr_t p = (uintptr_t)ptr;
  wSpriteOutputPtr[0] = p & 0xFF;
  wSpriteOutputPtr[1] = p >> 8;
  wSpriteOutputPtrCached[0] = wSpriteOutputPtr[0];
  wSpriteOutputPtrCached[1] = wSpriteOutputPtr[1];
}

uint8_t* GetOutputPtr() {
  uintptr_t p = wSpriteOutputPtr[0] | (wSpriteOutputPtr[1] << 8);
  return (uint8_t*)p;
}

// -----------------------------
// Write 2 bits into buffer
// -----------------------------
void WriteSpriteBitsToBuffer(uint8_t a) {
  uint8_t e = a;
  uint8_t offset = wSpriteOutputBitOffset;

  if (offset == 0) {
    e <<= 0;
  }
  else if (offset < 2) {
    e <<= 2;
  }
  else if (offset == 2) {
    uint8_t t = e;
    e = (t >> 4) | (t << 4);
  }
  else {
    e = (e >> 2);
  }

  uint8_t* hl = GetOutputPtr();
  *hl |= e;
}

// -----------------------------
// Core decompression
// -----------------------------
void MoveToNextBufferPosition(); // forward

void UncompressSpriteDataLoop();

// -----------------------------
void UncompressSpriteData(uint8_t bank) {
  uint8_t old = hLoadedROMBank;
  hLoadedROMBank = bank;
  MBC1RomBank = bank;

  MBC1SRamEnable = SRAM_ENABLE;
  MBC1SRamBank = 0;

  extern void _UncompressSpriteData();
  _UncompressSpriteData();

  hLoadedROMBank = old;
  MBC1RomBank = old;
}

// -----------------------------
void _UncompressSpriteData() {
  FillMemory(sSpriteBuffer1, sizeof(sSpriteBuffer1) * 2, 0);

  wSpriteInputBitCounter = 1;
  wSpriteOutputBitOffset = 3;

  wSpriteCurPosX = 0;
  wSpriteCurPosY = 0;
  wSpriteLoadFlags = 0;

  uint8_t b = ReadNextInputByte();
  uint8_t height = (b & 0x0F) * 8;
  uint8_t width = ((b >> 4) & 0x0F) * 8;

  wSpriteHeight = height;
  wSpriteWidth = width;

  wSpriteLoadFlags = ReadNextInputBit();

  UncompressSpriteDataLoop();
}

// -----------------------------
void UncompressSpriteDataLoop() {
  uint8_t* hl = (wSpriteLoadFlags & 1) ? sSpriteBuffer2 : sSpriteBuffer1;
  StoreSpriteOutputPointer(hl);

  if (wSpriteLoadFlags & 2) {
    uint8_t mode = ReadNextInputBit();
    if (mode == 0) wSpriteUnpackMode = 0;
    else {
      if (ReadNextInputBit() == 0) wSpriteUnpackMode = 1;
      else wSpriteUnpackMode = 2;
    }
  }

  if (!ReadNextInputBit()) {
    // RL zeros path (simplified placeholder)
  }

  while (true) {
    uint8_t c = (ReadNextInputBit() << 1) | ReadNextInputBit();

    if (c == 0) {
      // RLE zeros
    }
    else {
      WriteSpriteBitsToBuffer(c);
      MoveToNextBufferPosition();
    }
  }
}

// -----------------------------
void MoveToNextBufferPosition() {
  if (++wSpriteCurPosY == wSpriteHeight) {
    wSpriteCurPosY = 0;

    if (wSpriteOutputBitOffset != 0) {
      wSpriteOutputBitOffset--;
      return;
    }

    wSpriteOutputBitOffset = 3;

    wSpriteCurPosX += 8;
    if (wSpriteCurPosX == wSpriteWidth) {
      wSpriteCurPosX = 0;

      if (!(wSpriteLoadFlags & 2)) {
        wSpriteLoadFlags ^= 1;
        UncompressSpriteDataLoop();
      }
      else {
        UnpackSprite();
      }
    }
  }
}

// -----------------------------
void UnpackSprite();
void XorSpriteChunks();
void SpriteDifferentialDecode();

// -----------------------------
void UnpackSprite() {
  if (wSpriteUnpackMode == 2) {
    UnpackSpriteMode2();
  }
  else if (wSpriteUnpackMode != 0) {
    XorSpriteChunks();
  }
  else {
    SpriteDifferentialDecode();
  }
}

// -----------------------------
void SpriteDifferentialDecode() {
  uint8_t last = 0;

  uint8_t* ptr = GetOutputPtr();

  for (int y = 0; y < wSpriteHeight; y++) {
    for (int x = 0; x < wSpriteWidth; x++) {
      uint8_t v = ptr[x];
      uint8_t hi = (v >> 4) & 0xF;
      uint8_t lo = v & 0xF;

      // simplified decode
      hi ^= last;
      lo ^= last;

      last = lo;

      ptr[x] = (hi << 4) | lo;
    }
    ptr += wSpriteHeight;
  }
}

// -----------------------------
void XorSpriteChunks() {
  SpriteDifferentialDecode();
}

// -----------------------------
void UnpackSpriteMode2() {
  SpriteDifferentialDecode();
  XorSpriteChunks();
}

// -----------------------------
int main() {
  return 0;
}