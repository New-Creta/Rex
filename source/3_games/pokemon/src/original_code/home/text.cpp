// ============================================================
// Game Boy Text / Serial / Menu / Rendering System (pseudocode)
// Single-file consolidated translation of provided assembly
// ============================================================

#include <cstdint>
#include <cstring>

// ------------------------------------------------------------
// Global state (mocked)
// ------------------------------------------------------------

uint8_t wd730, wLetterPrintingDelayFlags, wOptions;
uint8_t hFrameCounter, hJoyHeld;
uint8_t wCurMapTextPtr[2], hSavedMapTextPtr[2];
uint8_t wTextDest[2];
uint8_t wPredefID, wPredefBank, wPredefParentBank;

uint8_t hSerialConnectionStatus, hSerialReceiveData, hSerialSendData;
uint8_t hSerialReceivedNewData, hSerialIgnoringInitialData;

uint8_t wSerialExchangeNybbleSendData, wSerialExchangeNybbleReceiveData;
uint8_t wSerialExchangeNybbleTempReceiveData, wSerialSyncAndExchangeNybbleReceiveData;

uint8_t wUnknownSerialCounter[2], wUnknownSerialCounter2[2];

#define BIT_A_BUTTON 0
#define BIT_B_BUTTON 1

#define SCREEN_WIDTH 20

// ------------------------------------------------------------
// Utility stubs
// ------------------------------------------------------------

void push(...) {}
void pop(...) {}
void Joypad() {}
void DelayFrame() {}
void DelayFrames(int) {}
void BankswitchHome() {}
void BankswitchBack() {}
void call_far(...) {}
void switch_bank(uint8_t) {}
void ManualTextScroll() {}
void ScrollTextUpOneLine() {}
void ManualTextScroll() {}
void PlaySound(...) {}
void PlayCry(...) {}
void WaitForSoundToFinish() {}
bool A_or_B_pressed() { return false; }

// ------------------------------------------------------------
// CalcDifference
// ------------------------------------------------------------

uint8_t CalcDifference(uint8_t a, uint8_t b) {
  uint8_t r = a - b;
  if (a >= b) return r;
  r = ~r + 1;
  return r;
}

// ------------------------------------------------------------
// PrintLetterDelay
// ------------------------------------------------------------

void PrintLetterDelay() {
  if (wd730 & (1 << 6)) return;
  if (!(wLetterPrintingDelayFlags & (1 << 1))) return;

  if (wLetterPrintingDelayFlags & (1 << 0))
    hFrameCounter = wOptions & 0x0F;
  else
    hFrameCounter = 1;

  Joypad();

  if (hJoyHeld & ((1 << BIT_A_BUTTON) | (1 << BIT_B_BUTTON)))
    return;

  DelayFrame();
}

// ------------------------------------------------------------
// TextBoxBorder
// ------------------------------------------------------------

void TextBoxBorder(uint8_t* hl, int w, int h) {
  *hl++ = '┌';
  for (int i = 0; i < w; i++) *hl++ = '─';
  *hl = '┐';

  hl += SCREEN_WIDTH;

  for (int y = 0; y < h; y++) {
    *hl++ = '│';
    for (int i = 0; i < w; i++) *hl++ = ' ';
    *hl = '│';
    hl += SCREEN_WIDTH;
  }

  *hl++ = '└';
  for (int i = 0; i < w; i++) *hl++ = '─';
  *hl = '┘';
}

// ------------------------------------------------------------
// PlaceString (core renderer)
// ------------------------------------------------------------

uint16_t PlaceString(uint8_t* de, uint8_t* hl) {
  while (true) {
    uint8_t a = *de;

    if (a == '@') return (uint16_t)hl;

    if (a == '<NEXT>') {
      hl += SCREEN_WIDTH;
      de++;
      continue;
    }

    if (a == '<LINE>') {
      hl = hl; // simplified
      de++;
      continue;
    }

    *hl++ = a;
    PrintLetterDelay();
    de++;
  }
}

// ------------------------------------------------------------
// PrintBCDNumber (simplified)
// ------------------------------------------------------------

void PrintBCDNumber(uint8_t* de, uint8_t* hl, uint8_t c) {
  for (int i = 0; i < (c & 0x1F); i++) {
    uint8_t v = *de++;
    uint8_t hi = v >> 4;
    uint8_t lo = v & 0x0F;

    *hl++ = '0' + hi;
    *hl++ = '0' + lo;
  }
}

// ------------------------------------------------------------
// PrintNumber (simplified placeholder)
// ------------------------------------------------------------

void PrintNumber(uint8_t* de, uint8_t* hl) {
  uint32_t val = *(uint32_t*)de;
  char buf[16];
  sprintf(buf, "%u", val);

  for (int i = 0; buf[i]; i++) {
    *hl++ = buf[i];
  }
}

// ------------------------------------------------------------
// TextCommandProcessor (VM core)
// ------------------------------------------------------------

void TextCommandProcessor(uint8_t* hl, uint8_t* bc) {
  uint8_t savedFlags = wLetterPrintingDelayFlags;
  wLetterPrintingDelayFlags |= (1 << 1);

  while (true) {
    uint8_t cmd = *hl++;

    if (cmd == 0xFF) break;

    switch (cmd) {

    case 0x01: // START
      PlaceString(hl, bc);
      break;

    case 0x02: // RAM
      PlaceString((uint8_t*)*(uint16_t*)hl, bc);
      break;

    case 0x03: // BCD
      PrintBCDNumber((uint8_t*)*(uint16_t*)hl, bc, *hl);
      break;

    case 0x04: // MOVE
      wTextDest[0] = hl[0];
      wTextDest[1] = hl[1];
      hl += 2;
      break;

    case 0x05: // BOX
      TextBoxBorder(bc, hl[0], hl[1]);
      hl += 2;
      break;

    case 0x06: // LOW
      bc = nullptr;
      break;

    case 0x07: // PROMPT
      ManualTextScroll();
      break;

    case 0x08: // SCROLL
      ScrollTextUpOneLine();
      ScrollTextUpOneLine();
      break;

    case 0x09: // ASM
      call_far(*(uint16_t*)hl);
      break;

    case 0x0A: // NUM
      PrintNumber((uint8_t*)*(uint16_t*)hl, bc);
      break;

    case 0x0B: // PAUSE
      if (!A_or_B_pressed()) DelayFrames(30);
      break;

    case 0x0C: // SOUND
      PlaySound(*(uint16_t*)hl);
      break;

    case 0x0D: // DOTS
      for (int i = 0; i < *hl; i++) {
        *bc++ = '…';
        DelayFrames(10);
      }
      break;

    case 0x0E: // WAIT
      ManualTextScroll();
      break;

    case 0x0F: // FAR
    {
      uint8_t bank = *hl++;
      uint8_t* addr = (uint8_t*)*(uint16_t*)hl;

      switch_bank(bank);
      TextCommandProcessor(addr, bc);
      break;
    }

    default:
      break;
    }
  }

  wLetterPrintingDelayFlags = savedFlags;
}

// ------------------------------------------------------------
// Serial core (simplified)
// ------------------------------------------------------------

uint8_t Serial_ExchangeByte(uint8_t send) {
  hSerialReceivedNewData = 0;

  if (hSerialConnectionStatus == 0)
    hSerialSendData = send;

  while (!hSerialReceivedNewData) {
    DelayFrame();
  }

  return hSerialReceiveData;
}

// ------------------------------------------------------------
// Start menu (simplified flow)
// ------------------------------------------------------------

void DisplayStartMenu() {
  PlaySound(0);

  while (true) {
    uint8_t input = 0;

    if (input & (1 << BIT_B_BUTTON)) {
      break;
    }

    if (input & (1 << BIT_A_BUTTON)) {
      switch (0) {
      case 0: /* Pokedex */ break;
      case 1: /* Pokemon */ break;
      case 2: /* Item */ break;
      default: break;
      }
    }
  }
}

// ------------------------------------------------------------
// Entry
// ------------------------------------------------------------

int main() {
  return 0;
}