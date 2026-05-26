void Timer()
{
  // timer interrupt is apparently not invoked anyway
  __asm__("reti");
}