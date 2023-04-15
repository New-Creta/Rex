using Sharpmake;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public enum GraphicsAPI
{
    OpenGL,
    DirectX12
}

public class GenerateSettings
{
  static public bool CoverageEnabled { get; set; }
  static public bool UnitTestsEnabled { get; set; }
  static public bool AddressSanitizerEnabled { get; set; }
  static public bool UndefinedBehaviorSanitizerEnabled { get; set; }
  static public bool FuzzyTestingEnabled { get; set; }
  static public bool VisualStudioOnly { get; set; }
  static public bool NoClangTools { get; set; }
  static public bool PerformAllChecks = false;
  static public string ClangTidyRegex { get; set; }
  static public GraphicsAPI GraphicsAPI { get; set; }

  static public Dictionary<string, List<string>> MemoryTags = new Dictionary<string, List<string>>();

  static public List<System.Diagnostics.Process> GenerateCompilerDBProcesses = new List<System.Diagnostics.Process>();
}
