#pragma once

#include "rex_std/string_view.h"

#include "rex_engine/dotnet/dotnet_bridge.h"

namespace rex
{
	// Wrapper around a dotnet library
	// Each wrapped dotnet assembly must have a setter function
	// for functions / delegates in C# that allows them to call back into C++
	// This API should use the follow code design
	// namespace Rex
	// {
  //  public static class AssemblyLoader
  //  {
  //    internal static Assembly ? ResolveAssembly(AssemblyLoadContext ? InAssemblyLoadContext, AssemblyName InAssemblyName)
  //    {
  //      int assemblyId = InAssemblyName.Name!.GetHashCode();
  //
  //      foreach(var loadContext in AssemblyLoadContext.All)
  //      {
  //        foreach(var assembly in loadContext.Assemblies)
  //        {
  //          if (assembly.GetName().Name != InAssemblyName.Name)
  //            continue;
  //
  //          return assembly;
  //        }
  //      }
  //
  //      return null;
  //    }
  //  }
  //
  //  internal static class TypeInterface
  //  {
  //    internal static Type ? FindType(string ? InTypeName)
  //    {
  //      var type = Type.GetType(InTypeName!,
  //        (name) = > AssemblyLoader.ResolveAssembly(null, name),
  //        (assembly, name, ignore) = >
  //      {
  //        return assembly != null ? assembly.GetType(name, false, ignore) : Type.GetType(name, false, ignore);
  //      }
  //      );
  //
  //      return type;
  //    }
  //  }
  //
  //  internal static class DelegateSetter
	// }
	class DotNetAssembly
	{
	public:
		DotNetAssembly(rsl::string_view assemblyPath);

		template <typename T>
		T get_function(rsl::string_view classPath, rsl::string_view methodName)
		{
			return (T)dotnet::instance()->load_function(m_assembly_path, classPath, methodName);
		}

		void set_function(rsl::string_view classPath, rsl::string_view delegateName, void* func);

	private:
		rsl::string m_assembly_path;

		using set_function_fn = void(*)(const char*, const char*, void*);
		set_function_fn m_set_function;
	};
}