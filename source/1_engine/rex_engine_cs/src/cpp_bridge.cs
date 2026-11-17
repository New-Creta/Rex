using System;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Loader;

namespace Rex
{
  public static class AssemblyLoader
  {
    internal static Assembly? ResolveAssembly(AssemblyLoadContext? InAssemblyLoadContext, AssemblyName InAssemblyName)
    {
      int assemblyId = InAssemblyName.Name!.GetHashCode();

      foreach (var loadContext in AssemblyLoadContext.All)
      {
        foreach (var assembly in loadContext.Assemblies)
        {
          if (assembly.GetName().Name != InAssemblyName.Name)
            continue;

          return assembly;
        }
      }

      return null;
    }
  }

  internal static class TypeInterface
  {
    internal static Type? FindType(string? InTypeName)
    {
      var type = Type.GetType(InTypeName!,
        (name) => AssemblyLoader.ResolveAssembly(null, name),
        (assembly, name, ignore) =>
        {
          return assembly != null ? assembly.GetType(name, false, ignore) : Type.GetType(name, false, ignore);
        }
      );

      return type;
    }
  }

  internal static class DelegateSetter
  {
    [UnmanagedCallersOnly]
    internal static void SetDelegate(IntPtr classNamePtr, IntPtr fieldNamePtr, IntPtr cppFuncPtr)
    {
      string? className = Marshal.PtrToStringUTF8(classNamePtr);
      Type? type = TypeInterface.FindType(className);

      if (type == null)
      {
        Console.WriteLine($"Failed to find type '{type}'. Cannot assign delegate");
        return;
      }

      string? fieldName = Marshal.PtrToStringUTF8(fieldNamePtr);
      if (fieldName == null)
      {
        Console.WriteLine($"Could not generate field name from pointer. Did you pass in a string?");
        return;
      }

      var bindingFlags = BindingFlags.Static | BindingFlags.NonPublic;
      FieldInfo? fieldInfo = type.GetField(fieldName, bindingFlags);
      if (fieldInfo == null)
      {
        Console.WriteLine($"Failed to find field with name '{fieldName}' of type '{className}'");
        return;
      }

      if (!fieldInfo.FieldType.IsFunctionPointer)
      {
        Console.WriteLine($"Field '{fieldName}' is not a function pointer");
        return;
      }

      fieldInfo.SetValue(null, cppFuncPtr);
      Console.WriteLine($"Delegate '{fieldName}' of '{className}' set!");
    }
  }
}