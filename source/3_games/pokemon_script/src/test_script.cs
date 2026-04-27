using System;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Loader;

namespace Pokemon
{
  internal static unsafe class TestScript
  {
    [UnmanagedCallersOnly]
    public static void RexEntry()
    {
      Console.WriteLine($"Called into Pokemon!");
    }

    internal static delegate* unmanaged<void> TestDelegate;
  }
}