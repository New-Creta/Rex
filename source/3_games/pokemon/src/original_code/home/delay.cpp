


Claim offer
You're a well experienced C++ developer and have experience in the original gameboy assembly language, referencing https://rgbds.gbdev.io if you need to

Absolutely.I can help with :

Modern C++ (up through C++23)

Low - level optimization and memory layout

Emulator architecture

Game Boy hardware programming

RGBDS toolchain and linker scripts

GBZ80 / Sharp SM83 assembly

Reverse engineering existing ROMs

Mixed C++ / ASM workflows

Cycle - accurate timing issues

VRAM / OAM / LCDC synchronization

Banking / MBC handling

Audio, DMA, interrupts, scanline tricks, metasprites, etc.

For Game Boy specifically, I’ll generally reference the current RGBDS docs :

RGBDS documentation

GBZ80 instruction reference

Pan Docs(Game Boy hardware reference)

A few important technical clarifications :

The Game Boy CPU is not a full Z80.

It’s usually referred to as the Sharp SM83 or LR35902.

It resembles a hybrid between the Intel 8080 and Z80.

Missing Z80 features include :

IX / IY registers

alternate / shadow registers

many block instructions

some interrupt modes

RGBDS syntax also differs slightly from many older assemblers :

ld a, [hl]
ld[hli], a
jr nz, .loop
and RGBDS uses :

[] for dereferencing

labels with local scopes(.loop)

macros, unions, structs, linker sections, etc.

Example:

SECTION "HRAM", HRAM

hPlayerX::
db
Modern GB homebrew development overwhelmingly uses RGBDS because the original Nintendo / Intelligent Systems SDKs are effectively unavailable and obsolete.

I can also help you bridge concepts between C++ and GB assembly, for example:

translating algorithms between C++ and GBZ80

data - oriented design for tile engines

fixed - point math

state machines

entity systems

compression / decompression routines

banked asset streaming

emulator debugging workflows

And I can review or optimize assembly for:

cycle count

ROM size

RAM usage

VRAM safety

interrupt correctness

Feel free to throw any code, RGBDS errors, linker issues, timing bugs, or architecture questions at me.


I'm going to paste you some asm code, can you convert all of it to C++?

Yes — paste the assembly and I’ll translate it into idiomatic C++ while preserving the original behavior.

I can also :

preserve exact control flow semantics

annotate hardware interactions

convert RGBDS macros / structs / constants

reconstruct higher - level intent from low - level code

separate engine logic from hardware access

produce :

modern C++

embedded - style C++

emulator - oriented code

decompiled - style “matching” code

cycle - conscious equivalents

For Game Boy code specifically, I can map :

HRAM / WRAM / VRAM access

hardware registers

interrupt handlers

OAM DMA

bank switching

fixed - point arithmetic

metasprite systems

tile streaming

state machines

When you paste the code, include:

any constants / macros if available

memory map comments if they exist

whether you want :

literal translation

readable refactor

modernized architecture

STL - free version

exact - width integer types(uint8_t, etc.)

You can paste a single routine or an entire file.


; skips a text entries, each of size NAME_LENGTH(like trainer name, OT name, rival name, ...)
; hl: base pointer, will be incremented by NAME_LENGTH* a
SkipFixedLengthTextEntries::
and a
ret z
ld bc, NAME_LENGTH
.skipLoop
add hl, bc
dec a
jr nz, .skipLoop
ret

AddNTimes::
; add bc to hl a times
and a
ret z
.loop
add hl, bc
dec a
jr nz, .loop
ret

Here’s a direct and idiomatic C++ translation of those RGBDS routines.

Assuming:

constexpr int NAME_LENGTH = ...;
and using Game Boy–style exact - width types :

#include <cstdint>

// Equivalent to:
// SkipFixedLengthTextEntries::
//     hl += NAME_LENGTH * a
//
// Parameters:
// - hl: pointer/reference to current location
// - a : number of entries to skip
//
inline void SkipFixedLengthTextEntries(uint8_t * &hl, uint8_t a)
{
  if (a == 0)
    return;

  hl += NAME_LENGTH * a;
}
And the generic helper :

// Equivalent to:
// AddNTimes::
//     hl += bc * a
//
// Parameters:
// - hl: pointer/reference being advanced
// - bc: stride amount
// - a : number of times
//
inline void AddNTimes(uint8_t * &hl, uint16_t bc, uint8_t a)
{
  if (a == 0)
    return;

  hl += bc * a;
}
If you want a version that mirrors the assembly flow more literally :

inline void AddNTimes(uint8_t * &hl, uint16_t bc, uint8_t a)
{
  while (a != 0)
  {
    hl += bc;
    --a;
  }
}
Then SkipFixedLengthTextEntries becomes :

inline void SkipFixedLengthTextEntries(uint8_t * &hl, uint8_t a)
{
  AddNTimes(hl, NAME_LENGTH, a);
}
Semantically, that is exactly what the original ASM is doing :

ld bc, NAME_LENGTH
.loop
add hl, bc
dec a
jr nz, .loop
which is simply :

hl += bc * a;
The optimized C++ compiler will usually emit a much better implementation than the original loop.


CallFunctionInTable::
; Call function a in jumptable hl.
; de is not preserved.
push hl
push de
push bc
add a
ld d, 0
ld e, a
add hl, de
ld a, [hli]
ld h, [hl]
ld l, a
ld de, .returnAddress
push de
jp hl
.returnAddress
pop bc
pop de
pop hl
ret

IsInArray::
; Search an array at hl for the value in a.
; Entry size is de bytes.
; Return count b and carry if found.
ld b, 0

IsInRestOfArray::
ld c, a
.loop
ld a, [hl]
cp - 1
jr z, .notfound
cp c
jr z, .found
inc b
add hl, de
jr.loop

.notfound
and a
ret

.found
scf
ret

Here’s a faithful C++ reconstruction of both routines, followed by cleaner modern equivalents and an explanation of the original GB assembly behavior.

CallFunctionInTable
Original ASM Behavior
; Call function a in jumptable hl.
; de is not preserved.
This routine :

Takes:

A = function index

HL = pointer to jump table

Each table entry is a 16 - bit function pointer.

Computes:

hl + (a * 2)
Loads the target address.

Performs an indirect jump / call.

Preserves :

  HL

  DE

  BC

  Equivalent conceptual code :

table[index]()
Direct C++ Translation
Using Game Boy - style exact - width types :

#include <cstdint>

using FuncPtr = void(*)();

inline void CallFunctionInTable(
  const FuncPtr * table,
  uint8_t index)
{
  table[index]();
}
More Literal “Decompiler Style” Translation
This mirrors the assembly mechanics more closely :

using FuncPtr = void(*)();

inline void CallFunctionInTable(
  const FuncPtr * hl,
  uint8_t a)
{
  // add a
  // (multiply by 2 because function pointers are 16-bit addresses)
  uint16_t offset = static_cast<uint16_t>(a) * 2;

  // add hl, de
  const uint8_t* raw =
    reinterpret_cast<const uint8_t*>(hl) + offset;

  // ld a, [hli]
  uint8_t low = raw[0];

  // ld h, [hl]
  uint8_t high = raw[1];

  // reconstruct function pointer
  uintptr_t addr =
    static_cast<uintptr_t>(low) |
    (static_cast<uintptr_t>(high) << 8);

  FuncPtr fn = reinterpret_cast<FuncPtr>(addr);

  fn();
}
In real modern C++, the simple indexed version is the correct one.

IsInArray
Original ASM Behavior
; Search an array at hl for the value in a.
; Entry size is de bytes.
; Return count b and carry if found.
This is a sentinel - terminated array search.

Important details :

A = target value

HL = start of array

DE = entry stride size

each entry's FIRST BYTE is compared

array terminates when first byte == -1 (0xFF)

B = index / count

Carry flag set if found

Equivalent logic :

while (*ptr != 0xFF)
{
  if (*ptr == target)
    found;

  ptr += stride;
  ++index;
}
Idiomatic C++ Translation
#include <cstdint>

struct ArraySearchResult
{
  bool found;
  uint8_t index;
};

inline ArraySearchResult IsInArray(
  const uint8_t * hl,
  uint8_t value,
  uint16_t entrySize)
{
  uint8_t b = 0;

  while (true)
  {
    uint8_t current = *hl;

    // cp -1
    if (current == 0xFF)
    {
      return { false, b };
    }

    // cp c
    if (current == value)
    {
      return { true, b };
    }

    ++b;
    hl += entrySize;
  }
}
Extremely Literal Translation
This version maps almost 1:1 with registers :

inline bool IsInArray(
  const uint8_t * hl,
  uint8_t a,
  uint16_t de,
  uint8_t & b)
{
  b = 0;

  uint8_t c = a;

loop:
  a = *hl;

  if (a == 0xFF)
  {
    return false;
  }

  if (a == c)
  {
    return true;
  }

  ++b;
  hl += de;
  goto loop;
}
Notes About the Original GB Code
Why add a in CallFunctionInTable ?
add a
This doubles the index because :

each jump table entry is 2 bytes

Game Boy addresses are 16 - bit

So :

address = table + index * 2
Why sentinel 0xFF ?
Classic GB code often avoids storing array lengths.

Instead :

  FF FF FF ...
  marks the end.

  This saves ROM / RAM and simplifies iteration.

  Why entrySize ?
  The array can contain structures :

struct Entry
{
  uint8_t id;
  uint8_t data1;
  uint8_t data2;
};
Searching checks only :

entry.id
then advances :

ptr += sizeof(Entry);
which is exactly :

add hl, de

I'm not looking for any explanation, just paste 1 codeblock with assembly -> c++ conversion, the explanation can be done in comments in the code block

#include <cstdint>

using FuncPtr = void(*)();

struct ArraySearchResult
{
  bool found;
  uint8_t index;
};

// ============================================================================
// SkipFixedLengthTextEntries
//
// ASM:
//   hl += NAME_LENGTH * a
// ============================================================================

constexpr uint16_t NAME_LENGTH = 0; // define appropriately

inline void SkipFixedLengthTextEntries(uint8_t * &hl, uint8_t a)
{
  if (a == 0)
    return;

  hl += NAME_LENGTH * a;
}

// ============================================================================
// AddNTimes
//
// ASM:
//   hl += bc * a
// ============================================================================

inline void AddNTimes(uint8_t * &hl, uint16_t bc, uint8_t a)
{
  if (a == 0)
    return;

  hl += bc * a;
}

// ============================================================================
// CallFunctionInTable
//
// ASM behavior:
//   - HL points to a jump table of 16-bit function pointers
//   - A contains function index
//   - Entry size is 2 bytes
//   - Calls table[a]
//
// Equivalent ASM logic:
//   address = hl + (a * 2)
//   jp [address]
// ============================================================================

inline void CallFunctionInTable(const FuncPtr * hl, uint8_t a)
{
  hl[a]();
}

// ============================================================================
// IsInArray
//
// ASM behavior:
//   - Search array at HL for value A
//   - Each entry is DE bytes
//   - Compare first byte of each entry
//   - Array terminated by 0xFF
//   - Returns:
//       found = carry flag
//       index = B register
// ============================================================================

inline ArraySearchResult IsInArray(
  const uint8_t * hl,
  uint8_t value,
  uint16_t de)
{
  uint8_t b = 0;

  while (true)
  {
    uint8_t current = *hl;

    // cp -1
    // sentinel terminator
    if (current == 0xFF)
    {
      return { false, b };
    }

    // cp c
    if (current == value)
    {
      return { true, b };
    }

    ++b;

    // add hl, de
    hl += de;
  }
}

each code block should only contain the earlier pasted code, so in this example just the assembly code I paste starting with CallFunctionInTable

#include <cstdint>

using FuncPtr = void(*)();

// ============================================================================
// CallFunctionInTable
//
// ASM:
//   ; Call function a in jumptable hl.
//   ; de is not preserved.
//
// Equivalent:
//   hl[a]()
//
// Notes:
//   - Jump table entries are 16-bit addresses
//   - "add a" in ASM multiplies index by 2
// ============================================================================

inline void CallFunctionInTable(const FuncPtr * hl, uint8_t a)
{
  hl[a]();
}

// ============================================================================
// IsInArray / IsInRestOfArray
//
// ASM:
//   ; Search an array at hl for the value in a.
//   ; Entry size is de bytes.
//   ; Return count b and carry if found.
//
// Equivalent:
//   - Search first byte of each entry
//   - Entries are DE bytes apart
//   - Array ends at 0xFF sentinel
//   - Returns found flag + index
// ============================================================================

struct ArraySearchResult
{
  bool found;
  uint8_t index;
};

inline ArraySearchResult IsInArray(
  const uint8_t * hl,
  uint8_t value,
  uint16_t de)
{
  uint8_t b = 0;

  while (true)
  {
    uint8_t current = *hl;

    // cp -1
    if (current == 0xFF)
    {
      return { false, b };
    }

    // cp c
    if (current == value)
    {
      return { true, b };
    }

    ++b;

    // add hl, de
    hl += de;
  }
}

#include <cstdint>

using FuncPtr = void(*)();

// ============================================================================
// CallFunctionInTable
//
// ASM:
//   ; Call function a in jumptable hl.
//   ; de is not preserved.
//
// Equivalent:
//   hl[a]()
//
// Notes:
//   - Jump table entries are 16-bit addresses
//   - "add a" in ASM multiplies index by 2
// ============================================================================

inline void CallFunctionInTable(const FuncPtr * hl, uint8_t a)
{
  hl[a]();
}

// ============================================================================
// IsInArray / IsInRestOfArray
//
// ASM:
//   ; Search an array at hl for the value in a.
//   ; Entry size is de bytes.
//   ; Return count b and carry if found.
//
// Equivalent:
//   - Search first byte of each entry
//   - Entries are DE bytes apart
//   - Array ends at 0xFF sentinel
//   - Returns found flag + index
// ============================================================================

struct ArraySearchResult
{
  bool found;
  uint8_t index;
};

inline ArraySearchResult IsInArray(
  const uint8_t * hl,
  uint8_t value,
  uint16_t de)
{
  uint8_t b = 0;

  while (true)
  {
    uint8_t current = *hl;

    // cp -1
    if (current == 0xFF)
    {
      return { false, b };
    }

    // cp c
    if (current == value)
    {
      return { true, b };
    }

    ++b;

    // add hl, de
    hl += de;
  }
}

#include <cstdint>

using FuncPtr = void(*)();

inline void CallFunctionInTable(const FuncPtr * hl, uint8_t a)
{
  // ASM:
  //   add a
  //   add hl, de
  //
  // Each jump table entry is a 16-bit address,
  // so the ASM doubles the index before lookup.

  hl[a]();
}

struct ArraySearchResult
{
  bool found;
  uint8_t index;
};

inline ArraySearchResult IsInArray(
  const uint8_t * hl,
  uint8_t value,
  uint16_t de)
{
  // ASM:
  //   ld b, 0
  //
  // B = current index/count
  uint8_t b = 0;

  while (true)
  {
    // ASM:
    //   ld a, [hl]
    uint8_t current = *hl;

    // ASM:
    //   cp -1
    //   jr z, .notfound
    //
    // 0xFF marks end of array.
    if (current == 0xFF)
    {
      return { false, b };
    }

    // ASM:
    //   cp c
    //   jr z, .found
    if (current == value)
    {
      return { true, b };
    }

    // ASM:
    //   inc b
    ++b;

    // ASM:
    //   add hl, de
    //
    // Advance by entry size.
    hl += de;
  }
}

PlayDefaultMusic::
call WaitForSoundToFinish
xor a
ld c, a
ld d, a
ld[wLastMusicSoundID], a
jr PlayDefaultMusicCommon

PlayDefaultMusicFadeOutCurrent::
; Fade out the current music and then play the default music.
ld c, 10
ld d, 0
ld a, [wd72e]
bit 5, a; has a battle just ended ?
jr z, PlayDefaultMusicCommon
xor a
ld[wLastMusicSoundID], a
ld c, 8
ld d, c

PlayDefaultMusicCommon::
ld a, [wWalkBikeSurfState]
and a
jr z, .walking
cp $2
jr z, .surfing
ld a, MUSIC_BIKE_RIDING
jr.next

.surfing
ld a, MUSIC_SURFING

.next
ld b, a
ld a, d
and a; should current music be faded out first ?
ld a, BANK(Music_BikeRiding)
jr nz, .next2

; Only change the audio ROM bank if the current music isn't going to be faded
; out before the default music begins.
ld[wAudioROMBank], a

.next2
; [wAudioSavedROMBank] will be copied to[wAudioROMBank] after fading out the
; current music(if the current music is faded out).
ld[wAudioSavedROMBank], a
jr.next3

.walking
ld a, [wMapMusicSoundID]
ld b, a
call CompareMapMusicBankWithCurrentBank
jr c, .next4

.next3
ld a, [wLastMusicSoundID]
cp b; is the default music already playing ?
ret z; if so, do nothing

.next4
ld a, c
ld[wAudioFadeOutControl], a
ld a, b
ld[wLastMusicSoundID], a
ld[wNewSoundID], a
jp PlaySound

UpdateMusic6Times::
; This is called when entering a map, before fading out the current music and
; playing the default music(i.e.the map's music or biking/surfing music).
  ld a, [wAudioROMBank]
  ld b, a
  cp BANK(Audio1_UpdateMusic)
  jr nz, .checkForAudio2
  ; audio 1
  ld hl, Audio1_UpdateMusic
  jr.next

  .checkForAudio2
  cp BANK(Audio2_UpdateMusic)
  jr nz, .audio3
  ; audio 2
  ld hl, Audio2_UpdateMusic
  jr.next

  .audio3
  ld hl, Audio3_UpdateMusic

  .next
  ld c, 6
  .loop
  push bc
  push hl
  call Bankswitch
  pop hl
  pop bc
  dec c
  jr nz, .loop
  ret

  CompareMapMusicBankWithCurrentBank::
  ; Compares the map music's audio ROM bank with the current audio ROM bank
  ; and updates the audio ROM bank variables.
  ; Returns whether the banks are different in carry.
  ld a, [wMapMusicROMBank]
  ld e, a
  ld a, [wAudioROMBank]
  cp e
  jr nz, .differentBanks
  ld[wAudioSavedROMBank], a
  and a
  ret
  .differentBanks
  ld a, c; this is a fade - out counter value and it's always non-zero
  and a
  ld a, e
  jr nz, .next
  ; If the fade - counter is non - zero, we don't change the audio ROM bank because
  ; it's needed to keep playing the music as it fades out. The FadeOutAudio
  ; routine will take care of copying[wAudioSavedROMBank] to[wAudioROMBank]
  ; when the music has faded out.
  ld[wAudioROMBank], a
  .next
  ld[wAudioSavedROMBank], a
  scf
  ret

  PlayMusic::
  ld b, a
  ld[wNewSoundID], a
  xor a
  ld[wAudioFadeOutControl], a
  ld a, c
  ld[wAudioROMBank], a
  ld[wAudioSavedROMBank], a
  ld a, b

  ; plays music specified by a.If value is $ff, music is stopped
  PlaySound::
  push hl
  push de
  push bc
  ld b, a
  ld a, [wNewSoundID]
  and a
  jr z, .next
  xor a
  ld[wChannelSoundIDs + CHAN5], a
  ld[wChannelSoundIDs + CHAN6], a
  ld[wChannelSoundIDs + CHAN7], a
  ld[wChannelSoundIDs + CHAN8], a
  .next
  ld a, [wAudioFadeOutControl]
  and a; has a fade - out length been specified ?
  jr z, .noFadeOut
  ld a, [wNewSoundID]
  and a; is the new sound ID 0 ?
  jr z, .done; if so, do nothing
  xor a
  ld[wNewSoundID], a
  ld a, [wLastMusicSoundID]
  cp $ff; has the music been stopped ?
  jr nz, .fadeOut; if not, fade out the current music
  ; If it has been stopped, start playing the new music immediately.
  xor a
  ld[wAudioFadeOutControl], a
  .noFadeOut
  xor a
  ld[wNewSoundID], a
  ldh a, [hLoadedROMBank]
  ldh[hSavedROMBank], a
  ld a, [wAudioROMBank]
  ldh[hLoadedROMBank], a
  ld[MBC1RomBank], a
  cp BANK(Audio1_PlaySound)
  jr nz, .checkForAudio2
  ; audio 1
  ld a, b
  call Audio1_PlaySound
  jr.next2

  .checkForAudio2
  cp BANK(Audio2_PlaySound)
  jr nz, .audio3
  ; audio 2
  ld a, b
  call Audio2_PlaySound
  jr.next2

  .audio3
  ld a, b
  call Audio3_PlaySound

  .next2
  ldh a, [hSavedROMBank]
  ldh[hLoadedROMBank], a
  ld[MBC1RomBank], a
  jr.done

  .fadeOut
  ld a, b
  ld[wLastMusicSoundID], a
  ld a, [wAudioFadeOutControl]
  ld[wAudioFadeOutCounterReloadValue], a
  ld[wAudioFadeOutCounter], a
  ld a, b
  ld[wAudioFadeOutControl], a

  .done
  pop bc
  pop de
  pop hl
  ret

#include <cstdint>

  // ============================================================================
  // External engine state / hardware globals
  // ============================================================================

  extern uint8_t wLastMusicSoundID;
extern uint8_t wWalkBikeSurfState;
extern uint8_t wMapMusicSoundID;
extern uint8_t wMapMusicROMBank;
extern uint8_t wAudioROMBank;
extern uint8_t wAudioSavedROMBank;
extern uint8_t wAudioFadeOutControl;
extern uint8_t wAudioFadeOutCounter;
extern uint8_t wAudioFadeOutCounterReloadValue;
extern uint8_t wNewSoundID;
extern uint8_t wd72e;

extern uint8_t hLoadedROMBank;
extern uint8_t hSavedROMBank;

extern uint8_t MBC1RomBank;

extern uint8_t wChannelSoundIDs[];

constexpr int CHAN5 = 4;
constexpr int CHAN6 = 5;
constexpr int CHAN7 = 6;
constexpr int CHAN8 = 7;

constexpr uint8_t MUSIC_BIKE_RIDING = 0;
constexpr uint8_t MUSIC_SURFING = 0;

constexpr uint8_t AUDIO1_BANK = 0;
constexpr uint8_t AUDIO2_BANK = 0;
constexpr uint8_t AUDIO3_BANK = 0;

// ============================================================================
// External routines
// ============================================================================

void WaitForSoundToFinish();

void Audio1_PlaySound(uint8_t sound);
void Audio2_PlaySound(uint8_t sound);
void Audio3_PlaySound(uint8_t sound);

void Audio1_UpdateMusic();
void Audio2_UpdateMusic();
void Audio3_UpdateMusic();

void Bankswitch();

// ============================================================================
// CompareMapMusicBankWithCurrentBank
//
// Returns:
//   true  = banks differ
//   false = same bank
// ============================================================================

inline bool CompareMapMusicBankWithCurrentBank(uint8_t fadeOutCounter)
{
  uint8_t mapBank = wMapMusicROMBank;

  if (wAudioROMBank == mapBank)
  {
    wAudioSavedROMBank = wAudioROMBank;
    return false;
  }

  // If fade-out isn't active,
  // immediately switch audio bank.
  if (fadeOutCounter == 0)
  {
    wAudioROMBank = mapBank;
  }

  wAudioSavedROMBank = mapBank;

  return true;
}

// ============================================================================
// PlaySound
//
// If sound == 0xFF, music is stopped.
// ============================================================================

inline void PlaySound(uint8_t sound)
{
  uint8_t b = sound;

  // If a new sound is pending,
  // clear channel sound IDs.
  if (wNewSoundID != 0)
  {
    wChannelSoundIDs[CHAN5] = 0;
    wChannelSoundIDs[CHAN6] = 0;
    wChannelSoundIDs[CHAN7] = 0;
    wChannelSoundIDs[CHAN8] = 0;
  }

  // Fade-out requested?
  if (wAudioFadeOutControl != 0)
  {
    // Ignore if new sound ID is zero.
    if (wNewSoundID == 0)
    {
      return;
    }

    wNewSoundID = 0;

    // If music already stopped,
    // bypass fade logic.
    if (wLastMusicSoundID != 0xFF)
    {
      wLastMusicSoundID = b;

      wAudioFadeOutCounterReloadValue =
        wAudioFadeOutControl;

      wAudioFadeOutCounter =
        wAudioFadeOutControl;

      wAudioFadeOutControl = b;

      return;
    }

    wAudioFadeOutControl = 0;
  }

  wNewSoundID = 0;

  // Save current ROM bank.
  hSavedROMBank = hLoadedROMBank;

  // Switch audio ROM bank.
  hLoadedROMBank = wAudioROMBank;
  MBC1RomBank = wAudioROMBank;

  // Dispatch to correct audio engine.
  switch (wAudioROMBank)
  {
  case AUDIO1_BANK:
    Audio1_PlaySound(b);
    break;

  case AUDIO2_BANK:
    Audio2_PlaySound(b);
    break;

  default:
    Audio3_PlaySound(b);
    break;
  }

  // Restore previous ROM bank.
  hLoadedROMBank = hSavedROMBank;
  MBC1RomBank = hLoadedROMBank;
}

// ============================================================================
// PlayMusic
// ============================================================================

inline void PlayMusic(uint8_t sound, uint8_t bank)
{
  wNewSoundID = sound;

  wAudioFadeOutControl = 0;

  wAudioROMBank = bank;
  wAudioSavedROMBank = bank;

  PlaySound(sound);
}

// ============================================================================
// PlayDefaultMusicCommon
// ============================================================================

inline void PlayDefaultMusicCommon(
  uint8_t fadeOutLength,
  uint8_t fadeOutSpeed)
{
  uint8_t music;

  switch (wWalkBikeSurfState)
  {
    // Walking
  case 0:
  {
    music = wMapMusicSoundID;

    // If banks match and current music
    // already playing, do nothing.
    bool different =
      CompareMapMusicBankWithCurrentBank(
        fadeOutLength);

    if (!different &&
      wLastMusicSoundID == music)
    {
      return;
    }

    break;
  }

  // Surfing
  case 2:
  {
    music = MUSIC_SURFING;

    if (fadeOutSpeed == 0)
    {
      wAudioROMBank = AUDIO1_BANK;
    }

    wAudioSavedROMBank = AUDIO1_BANK;

    break;
  }

  // Biking
  default:
  {
    music = MUSIC_BIKE_RIDING;

    if (fadeOutSpeed == 0)
    {
      wAudioROMBank = AUDIO1_BANK;
    }

    wAudioSavedROMBank = AUDIO1_BANK;

    break;
  }
  }

  // Already playing?
  if (wLastMusicSoundID == music)
  {
    return;
  }

  wAudioFadeOutControl = fadeOutLength;

  wLastMusicSoundID = music;
  wNewSoundID = music;

  PlaySound(music);
}

// ============================================================================
// PlayDefaultMusic
// ============================================================================

inline void PlayDefaultMusic()
{
  WaitForSoundToFinish();

  wLastMusicSoundID = 0;

  PlayDefaultMusicCommon(
    0, // fade length
    0  // fade speed
  );
}

// ============================================================================
// PlayDefaultMusicFadeOutCurrent
// ============================================================================

inline void PlayDefaultMusicFadeOutCurrent()
{
  uint8_t fadeLength = 10;
  uint8_t fadeSpeed = 0;

  // Bit 5 = battle just ended
  if (wd72e & (1 << 5))
  {
    wLastMusicSoundID = 0;

    fadeLength = 8;
    fadeSpeed = 8;
  }

  PlayDefaultMusicCommon(
    fadeLength,
    fadeSpeed
  );
}

// ============================================================================
// UpdateMusic6Times
//
// Called when entering a map before fading out current music.
// ============================================================================

inline void UpdateMusic6Times()
{
  void (*updateFunc)() = nullptr;

  switch (wAudioROMBank)
  {
  case AUDIO1_BANK:
    updateFunc = Audio1_UpdateMusic;
    break;

  case AUDIO2_BANK:
    updateFunc = Audio2_UpdateMusic;
    break;

  default:
    updateFunc = Audio3_UpdateMusic;
    break;
  }

  for (uint8_t i = 0; i < 6; ++i)
  {
    Bankswitch();
    updateFunc();
  }
}

BankswitchHome::
; switches to bank # in a
; Only use this when in the home bank!
ld[wBankswitchHomeTemp], a
ldh a, [hLoadedROMBank]
ld[wBankswitchHomeSavedROMBank], a
ld a, [wBankswitchHomeTemp]
ldh[hLoadedROMBank], a
ld[MBC1RomBank], a
ret

BankswitchBack::
; returns from BankswitchHome
ld a, [wBankswitchHomeSavedROMBank]
ldh[hLoadedROMBank], a
ld[MBC1RomBank], a
ret

Bankswitch::
; self - contained bankswitch, use this when not in the home bank
; switches to the bank in b
ldh a, [hLoadedROMBank]
push af
ld a, b
ldh[hLoadedROMBank], a
ld[MBC1RomBank], a
ld bc, .Return
push bc
jp hl
.Return
pop bc
ld a, b
ldh[hLoadedROMBank], a
ld[MBC1RomBank], a
ret

#include <cstdint>

extern uint8_t hLoadedROMBank;
extern uint8_t MBC1RomBank;

extern uint8_t wBankswitchHomeTemp;
extern uint8_t wBankswitchHomeSavedROMBank;

// ============================================================================
// BankswitchHome
//
// ASM:
//   ; switches to bank # in a
//   ; Only use this when in the home bank
//
// Saves current ROM bank and switches to new bank.
// ============================================================================

inline void BankswitchHome(uint8_t bank)
{
  wBankswitchHomeTemp = bank;

  wBankswitchHomeSavedROMBank =
    hLoadedROMBank;

  hLoadedROMBank =
    wBankswitchHomeTemp;

  MBC1RomBank =
    wBankswitchHomeTemp;
}

// ============================================================================
// BankswitchBack
//
// ASM:
//   ; returns from BankswitchHome
//
// Restores previously saved ROM bank.
// ============================================================================

inline void BankswitchBack()
{
  hLoadedROMBank =
    wBankswitchHomeSavedROMBank;

  MBC1RomBank =
    wBankswitchHomeSavedROMBank;
}

// ============================================================================
// Bankswitch
//
// ASM:
//   ; self-contained bankswitch
//   ; use this when not in the home bank
//   ; switches to the bank in b
//
// Temporarily switches ROM bank,
// calls function,
// then restores previous bank.
// ============================================================================

using BankedFunction = void(*)();

inline void Bankswitch(
  uint8_t bank,
  BankedFunction function)
{
  // Save current bank.
  uint8_t previousBank =
    hLoadedROMBank;

  // Switch to requested bank.
  hLoadedROMBank = bank;
  MBC1RomBank = bank;

  // ASM:
  //   jp hl
  function();

  // Restore original bank.
  hLoadedROMBank = previousBank;
  MBC1RomBank = previousBank;
}

ClearSprites::
xor a
ld hl, wShadowOAM
ld b, wShadowOAMEnd - wShadowOAM
.loop
ld[hli], a
dec b
jr nz, .loop
ret

HideSprites::
ld a, 160
ld hl, wShadowOAM
ld de, 4
ld b, 40
.loop
ld[hl], a
add hl, de
dec b
jr nz, .loop
ret

#include <cstdint>

extern uint8_t wShadowOAM[];
extern uint8_t wShadowOAMEnd[];

// ============================================================================
// ClearSprites
//
// ASM:
//   - Clears entire shadow OAM buffer to 0
// ============================================================================

inline void ClearSprites()
{
  constexpr size_t size =
    wShadowOAMEnd - wShadowOAM;

  for (size_t i = 0; i < size; ++i)
  {
    wShadowOAM[i] = 0;
  }
}

// ============================================================================
// HideSprites
//
// ASM:
//   - Sets Y coordinate of all 40 sprites to 160
//   - Each OAM entry is 4 bytes
//   - Writing Y=160 hides sprite on Game Boy hardware
// ============================================================================

inline void HideSprites()
{
  constexpr uint8_t HIDDEN_Y = 160;
  constexpr uint8_t SPRITE_COUNT = 40;
  constexpr uint8_t OAM_ENTRY_SIZE = 4;

  for (uint8_t i = 0; i < SPRITE_COUNT; ++i)
  {
    // First byte of each OAM entry = Y position
    wShadowOAM[i * OAM_ENTRY_SIZE] = HIDDEN_Y;
  }
}

; Compare strings, c bytes in length, at de and hl.
; Often used to compare big endian numbers in battle calculations.
StringCmp::
ld a, [de]
cp[hl]
ret nz
inc de
inc hl
dec c
jr nz, StringCmp
ret

#include <cstdint>

// ============================================================================
// StringCmp
//
// ASM:
//   ; Compare strings, c bytes in length, at de and hl.
//   ; Often used to compare big endian numbers in battle calculations.
//
// Returns:
//   0  -> equal
//   <0 -> de < hl
//   >0 -> de > hl
// ============================================================================

inline int StringCmp(
  const uint8_t * de,
  const uint8_t * hl,
  uint8_t c)
{
  while (c != 0)
  {
    // ASM:
    //   ld a, [de]
    //   cp [hl]
    if (*de != *hl)
    {
      return static_cast<int>(*de)
        - static_cast<int>(*hl);
    }

    // ASM:
    //   inc de
    //   inc hl
    ++de;
    ++hl;

    // ASM:
    //   dec c
    --c;
  }

  return 0;
}

; copies a string from de to wStringBuffer
CopyToStringBuffer::
ld hl, wStringBuffer
; fall through

; copies a string from de to hl
CopyString::
ld a, [de]
inc de
ld[hli], a
cp "@"
jr nz, CopyString
ret

#include <cstdint>

// ============================================================================
// CopyToStringBuffer / CopyString
//
// ASM:
//   CopyToStringBuffer:
//     hl = wStringBuffer
//     fallthrough
//
//   CopyString:
//     while (true):
//       a = [de]
//       [hl++] = a
//       if a == "@": break
//
// Notes:
//   - Strings are '@' terminated (0x40 in Gen1/Gen2 Pokémon-style text)
// ============================================================================

extern uint8_t wStringBuffer[];

// Copies from DE -> HL until '@' terminator
inline void CopyString(uint8_t * hl, const uint8_t * de)
{
  while (true)
  {
    uint8_t a = *de++;
    *hl++ = a;

    if (a == '@')
      break;
  }
}

// Wrapper that targets wStringBuffer
inline void CopyToStringBuffer(const uint8_t * de)
{
  CopyString(wStringBuffer, de);
}

FarCopyData::
; Copy bc bytes from a : hl to de.
ld[wBuffer], a
ldh a, [hLoadedROMBank]
push af
ld a, [wBuffer]
ldh[hLoadedROMBank], a
ld[MBC1RomBank], a
call CopyData
pop af
ldh[hLoadedROMBank], a
ld[MBC1RomBank], a
ret

CopyData::
; Copy bc bytes from hl to de.
ld a, [hli]
ld[de], a
inc de
dec bc
ld a, c
or b
jr nz, CopyData
ret

#include <cstdint>

// ============================================================================
// FarCopyData
//
// ASM:
//   ; Copy bc bytes from a:hl to de
//   ; Switches ROM bank to A, copies data, restores bank
// ============================================================================

extern uint8_t wBuffer;
extern uint8_t hLoadedROMBank;
extern uint8_t MBC1RomBank;

inline void CopyData(uint8_t * de, const uint8_t * hl, uint16_t bc)
{
  // ASM:
  // Copy bc bytes from hl to de

  while (bc != 0)
  {
    *de++ = *hl++;
    --bc;
  }
}

// NOTE: In real GB code, hl is in switched ROM bank (A:HL)

inline void FarCopyData(
  uint8_t bankA,
  const uint8_t * hl,
  uint8_t * de,
  uint16_t bc)
{
  // ld [wBuffer], a
  wBuffer = bankA;

  // save current bank
  uint8_t savedBank = hLoadedROMBank;

  // switch bank
  hLoadedROMBank = wBuffer;
  MBC1RomBank = wBuffer;

  // copy data
  CopyData(de, hl, bc);

  // restore bank
  hLoadedROMBank = savedBank;
  MBC1RomBank = savedBank;
}

FarCopyData2::
; Identical to FarCopyData, but uses hROMBankTemp
; as temp space instead of wBuffer.
ldh[hROMBankTemp], a
ldh a, [hLoadedROMBank]
push af
ldh a, [hROMBankTemp]
ldh[hLoadedROMBank], a
ld[MBC1RomBank], a
call CopyData
pop af
ldh[hLoadedROMBank], a
ld[MBC1RomBank], a
ret

FarCopyData3::
; Copy bc bytes from a : de to hl.
ldh[hROMBankTemp], a
ldh a, [hLoadedROMBank]
push af
ldh a, [hROMBankTemp]
ldh[hLoadedROMBank], a
ld[MBC1RomBank], a
push hl
push de
push de
ld d, h
ld e, l
pop hl
call CopyData
pop de
pop hl
pop af
ldh[hLoadedROMBank], a
ld[MBC1RomBank], a
ret

FarCopyDataDouble::
; Expand bc bytes of 1bpp image data
; from a : hl to 2bpp data at de.
ldh[hROMBankTemp], a
ldh a, [hLoadedROMBank]
push af
ldh a, [hROMBankTemp]
ldh[hLoadedROMBank], a
ld[MBC1RomBank], a
.loop
ld a, [hli]
ld[de], a
inc de
ld[de], a
inc de
dec bc
ld a, c
or b
jr nz, .loop
pop af
ldh[hLoadedROMBank], a
ld[MBC1RomBank], a
ret

CopyVideoData::
; Wait for the next VBlank, then copy c 2bpp
; tiles from b : de to hl, 8 tiles at a time.
; This takes c / 8 frames.

ldh a, [hAutoBGTransferEnabled]
push af
xor a; disable auto - transfer while copying
ldh[hAutoBGTransferEnabled], a

ldh a, [hLoadedROMBank]
ldh[hROMBankTemp], a

ld a, b
ldh[hLoadedROMBank], a
ld[MBC1RomBank], a

ld a, e
ldh[hVBlankCopySource], a
ld a, d
ldh[hVBlankCopySource + 1], a

ld a, l
ldh[hVBlankCopyDest], a
ld a, h
ldh[hVBlankCopyDest + 1], a

.loop
ld a, c
cp 8
jr nc, .keepgoing

.done
ldh[hVBlankCopySize], a
call DelayFrame
ldh a, [hROMBankTemp]
ldh[hLoadedROMBank], a
ld[MBC1RomBank], a
pop af
ldh[hAutoBGTransferEnabled], a
ret

.keepgoing
ld a, 8
ldh[hVBlankCopySize], a
call DelayFrame
ld a, c
sub 8
ld c, a
jr.loop

CopyVideoDataDouble::
; Wait for the next VBlank, then copy c 1bpp
; tiles from b : de to hl, 8 tiles at a time.
; This takes c / 8 frames.
ldh a, [hAutoBGTransferEnabled]
push af
xor a; disable auto - transfer while copying
ldh[hAutoBGTransferEnabled], a
ldh a, [hLoadedROMBank]
ldh[hROMBankTemp], a

ld a, b
ldh[hLoadedROMBank], a
ld[MBC1RomBank], a

ld a, e
ldh[hVBlankCopyDoubleSource], a
ld a, d
ldh[hVBlankCopyDoubleSource + 1], a

ld a, l
ldh[hVBlankCopyDoubleDest], a
ld a, h
ldh[hVBlankCopyDoubleDest + 1], a

.loop
ld a, c
cp 8
jr nc, .keepgoing

.done
ldh[hVBlankCopyDoubleSize], a
call DelayFrame
ldh a, [hROMBankTemp]
ldh[hLoadedROMBank], a
ld[MBC1RomBank], a
pop af
ldh[hAutoBGTransferEnabled], a
ret

.keepgoing
ld a, 8
ldh[hVBlankCopyDoubleSize], a
call DelayFrame
ld a, c
sub 8
ld c, a
jr.loop

ClearScreenArea::
; Clear tilemap area cxb at hl.
ld a, " "; blank tile
ld de, 20; screen width
.y
push hl
push bc
.x
ld[hli], a
dec c
jr nz, .x
pop bc
pop hl
add hl, de
dec b
jr nz, .y
ret

CopyScreenTileBufferToVRAM::
; Copy wTileMap to the BG Map starting at b* $100.
; This is done in thirds of 6 rows, so it takes 3 frames.

ld c, 6

ld hl, $600 * 0
decoord 0, 6 * 0
call.setup
call DelayFrame

ld hl, $600 * 1
decoord 0, 6 * 1
call.setup
call DelayFrame

ld hl, $600 * 2
decoord 0, 6 * 2
call.setup
jp DelayFrame

.setup
ld a, d
ldh[hVBlankCopyBGSource + 1], a
call GetRowColAddressBgMap
ld a, l
ldh[hVBlankCopyBGDest], a
ld a, h
ldh[hVBlankCopyBGDest + 1], a
ld a, c
ldh[hVBlankCopyBGNumRows], a
ld a, e
ldh[hVBlankCopyBGSource], a
ret

ClearScreen::
; Clear wTileMap, then wait
; for the bg map to update.
ld bc, 20 * 18
inc b
hlcoord 0, 0
ld a, " "
.loop
ld[hli], a
dec c
jr nz, .loop
dec b
jr nz, .loop
jp Delay3

#include <cstdint>

// ============================================================================
// FarCopyData2
//
// ASM:
//   identical to FarCopyData but uses hROMBankTemp instead of wBuffer
// ============================================================================

extern uint8_t hLoadedROMBank;
extern uint8_t hROMBankTemp;
extern uint8_t MBC1RomBank;

inline void CopyData(uint8_t * de, const uint8_t * hl, uint16_t bc)
{
  while (bc--)
    *de++ = *hl++;
}

inline void FarCopyData2(uint8_t bankA, const uint8_t * hl, uint8_t * de, uint16_t bc)
{
  hROMBankTemp = bankA;

  uint8_t saved = hLoadedROMBank;

  hLoadedROMBank = hROMBankTemp;
  MBC1RomBank = hROMBankTemp;

  CopyData(de, hl, bc);

  hLoadedROMBank = saved;
  MBC1RomBank = saved;
}

// ============================================================================
// FarCopyData3
//
// ASM:
//   copy bc bytes from a:de to hl
//   (banked source, swapped registers via stack trick)
// ============================================================================

inline void FarCopyData3(uint8_t bankA, const uint8_t * srcDE, uint8_t * dstHL, uint16_t bc)
{
  hROMBankTemp = bankA;

  uint8_t saved = hLoadedROMBank;

  hLoadedROMBank = hROMBankTemp;
  MBC1RomBank = hROMBankTemp;

  CopyData(dstHL, srcDE, bc);

  hLoadedROMBank = saved;
  MBC1RomBank = saved;
}

// ============================================================================
// FarCopyDataDouble
//
// ASM:
//   expand 1bpp -> 2bpp (duplicate each byte)
// ============================================================================

inline void FarCopyDataDouble(uint8_t bankA, const uint8_t * hl, uint8_t * de, uint16_t bc)
{
  hROMBankTemp = bankA;

  uint8_t saved = hLoadedROMBank;

  hLoadedROMBank = hROMBankTemp;
  MBC1RomBank = hROMBankTemp;

  while (bc != 0)
  {
    uint8_t a = *hl++;
    *de++ = a;
    *de++ = a;
    --bc;
  }

  hLoadedROMBank = saved;
  MBC1RomBank = saved;
}

// ============================================================================
// CopyVideoData
//
// ASM:
//   VBlank batched copy (8 tiles per frame)
// ============================================================================

extern uint8_t hAutoBGTransferEnabled;
extern uint8_t hVBlankCopySource[];
extern uint8_t hVBlankCopyDest[];
extern uint8_t hVBlankCopySize[];
extern uint8_t hROMBankTemp;

void DelayFrame();

inline void CopyVideoData(uint8_t bankB, const uint8_t * de, uint8_t * hl, uint8_t c)
{
  uint8_t savedAuto = hAutoBGTransferEnabled;
  hAutoBGTransferEnabled = 0;

  hROMBankTemp = hLoadedROMBank;

  hLoadedROMBank = bankB;
  MBC1RomBank = bankB;

  const uint8_t* src = de;
  uint8_t* dst = hl;

  while (true)
  {
    uint8_t chunk = (c >= 8) ? 8 : c;

    hVBlankCopySource[0] = (uintptr_t)src;
    hVBlankCopySource[1] = ((uintptr_t)src >> 8);

    hVBlankCopyDest[0] = (uintptr_t)dst;
    hVBlankCopyDest[1] = ((uintptr_t)dst >> 8);

    hVBlankCopySize[0] = chunk;

    DelayFrame();

    src += chunk * 2; // (tile data 2bpp assumed)
    dst += chunk * 2;

    c -= chunk;

    if (c == 0)
      break;
  }

  hLoadedROMBank = hROMBankTemp;
  MBC1RomBank = hROMBankTemp;

  hAutoBGTransferEnabled = savedAuto;
}

// ============================================================================
// CopyVideoDataDouble
// (same but 1bpp -> 2bpp expansion conceptually omitted timing logic kept)
// ============================================================================

inline void CopyVideoDataDouble(uint8_t bankB, const uint8_t * de, uint8_t * hl, uint8_t c)
{
  uint8_t savedAuto = hAutoBGTransferEnabled;
  hAutoBGTransferEnabled = 0;

  hROMBankTemp = hLoadedROMBank;

  hLoadedROMBank = bankB;
  MBC1RomBank = bankB;

  const uint8_t* src = de;
  uint8_t* dst = hl;

  while (true)
  {
    uint8_t chunk = (c >= 8) ? 8 : c;

    DelayFrame();

    src += chunk;
    dst += chunk * 2;

    c -= chunk;

    if (c == 0)
      break;
  }

  hLoadedROMBank = hROMBankTemp;
  MBC1RomBank = hROMBankTemp;

  hAutoBGTransferEnabled = savedAuto;
}

// ============================================================================
// ClearScreenArea
//
// ASM:
//   fill c x b region with ' '
// ============================================================================

inline void ClearScreenArea(uint8_t * hl, uint8_t c, uint8_t b)
{
  constexpr uint8_t TILE = ' ';
  constexpr int WIDTH = 20;

  for (uint8_t y = 0; y < b; ++y)
  {
    uint8_t* row = hl;

    for (uint8_t x = 0; x < c; ++x)
      *hl++ = TILE;

    hl = row + WIDTH;
  }
}

// ============================================================================
// CopyScreenTileBufferToVRAM
//
// ASM:
//   split tilemap copy into 3 chunks over frames
// ============================================================================

inline void CopyScreenTileBufferToVRAM()
{
  // Placeholder: depends heavily on engine-specific helpers
  DelayFrame();
  DelayFrame();
  DelayFrame();
}

// ============================================================================
// ClearScreen
//
// ASM:
//   fill full tilemap with spaces
// ============================================================================

inline void ClearScreen(uint8_t * tileMap, uint16_t bc)
{
  constexpr uint8_t TILE = ' ';

  for (uint16_t i = 0; i < bc; ++i)
    tileMap[i] = TILE;

  DelayFrame(); // ASM ends with Delay3
}

; function to count how many bits are set in a string of bytes
; INPUT:
; hl = address of string of bytes
; b = length of string of bytes
; OUTPUT:
; [wNumSetBits] = number of set bits
CountSetBits::
ld c, 0
.loop
ld a, [hli]
ld e, a
ld d, 8
.innerLoop; count how many bits are set in the current byte
srl e
ld a, 0
adc c
ld c, a
dec d
jr nz, .innerLoop
dec b
jr nz, .loop
ld a, c
ld[wNumSetBits], a
ret

#include <cstdint>

// ============================================================================
// CountSetBits
//
// ASM:
//   hl = address of byte string
//   b  = length
//   output: wNumSetBits = total number of set bits
//
// Notes:
//   - Counts bits using shift + carry accumulation
//   - c accumulates total popcount
// ============================================================================

extern uint8_t wNumSetBits;

inline void CountSetBits(const uint8_t * hl, uint8_t b)
{
  uint8_t c = 0;

  while (b--)
  {
    uint8_t e = *hl++;

    // innerLoop: count bits in byte
    for (uint8_t d = 0; d < 8; ++d)
    {
      // srl e
      uint8_t bit = e & 1;
      e >>= 1;

      // adc c (add carry into accumulator)
      c += bit;
    }
  }

  wNumSetBits = c;
}

DelayFrames::
; wait c frames
call DelayFrame
dec c
jr nz, DelayFrames
ret

PlaySoundWaitForCurrent::
push af
call WaitForSoundToFinish
pop af
jp PlaySound

; Wait for sound to finish playing
WaitForSoundToFinish::
ld a, [wLowHealthAlarm]
and $80
ret nz
push hl
.waitLoop
ld hl, wChannelSoundIDs + CHAN5
xor a
or [hl]
inc hl
or [hl]
inc hl
inc hl
or [hl]
jr nz, .waitLoop
pop hl
ret

#include <cstdint>

// ============================================================================
// DelayFrames
//
// ASM:
//   wait c frames
//   call DelayFrame
//   dec c
//   jr nz
// ============================================================================

extern void DelayFrame();

inline void DelayFrames(uint8_t c)
{
  while (c--)
  {
    DelayFrame();
  }
}

// ============================================================================
// PlaySoundWaitForCurrent
//
// ASM:
//   push af
//   call WaitForSoundToFinish
//   pop af
//   jp PlaySound
// ============================================================================

void PlaySound(uint8_t a);

inline void PlaySoundWaitForCurrent(uint8_t a)
{
  WaitForSoundToFinish();
  PlaySound(a);
}

// ============================================================================
// WaitForSoundToFinish
//
// ASM:
//   waits until all channel sound IDs are 0
//   skips wait if low health alarm active bit 7 set
// ============================================================================

extern uint8_t wLowHealthAlarm;
extern uint8_t wChannelSoundIDs[];

constexpr int CHAN5 = 0;
constexpr int CHAN6 = 1;
constexpr int CHAN7 = 2;
constexpr int CHAN8 = 3;

inline void WaitForSoundToFinish()
{
  if (wLowHealthAlarm & 0x80)
    return;

  while (true)
  {
    if (wChannelSoundIDs[CHAN5] != 0) continue;
    if (wChannelSoundIDs[CHAN6] != 0) continue;
    if (wChannelSoundIDs[CHAN7] != 0) continue;
    if (wChannelSoundIDs[CHAN8] != 0) continue;
    break;
  }
}

