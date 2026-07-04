#include <cstdint>

// text system terminator (GB-style)
static const char TEXT_END = '@';

const char TextScriptEndingText[] = "@";

// returns pointer to end-of-text script
const char* TextScriptEnd()
{
  const char* hl = TextScriptEndingText;
  return hl;
}

// external text resources
extern const char _ExclamationText[];
extern const char _GroundRoseText[];
extern const char _BoulderText[];
extern const char _MartSignText[];
extern const char _PokeCenterSignText[];

// wrappers for far text
const char* ExclamationText()
{
  return _ExclamationText;
}

const char* GroundRoseText()
{
  return _GroundRoseText;
}

const char* BoulderText()
{
  return _BoulderText;
}

const char* MartSignText()
{
  return _MartSignText;
}

const char* PokeCenterSignText()
{
  return _PokeCenterSignText;
}

// scripted text action
void PickUpItem();

const char* PickUpItemText()
{
  PickUpItem();
  return TextScriptEnd();
}