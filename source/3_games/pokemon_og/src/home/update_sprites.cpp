void UpdateSprites() {
  if (--wUpdateSpritesEnabled != 0)
    return;

  extern void _UpdateSprites();
  _UpdateSprites();
}