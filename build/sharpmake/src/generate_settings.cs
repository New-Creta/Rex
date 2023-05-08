using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class GenerateSettings
{
  static public bool EnableVisualStudio = false;
  static public bool NoClangTools = false;
  static public bool PerformAllChecks = false;
  static public bool DisableClangTidyForThirdParty = false;
  static public string IntermediateDir = "";

  // Test flags
  static public bool EnableDefaultGeneration = true;
  static public bool GenerateUnitTests = false;
  static public bool CoverageEnabled = false;
  static public bool AddressSanitizerEnabled = false;
  static public bool UndefinedBehaviorSanitizerEnabled = false;
  static public bool GenerateFuzzyTests = false;


  static public string ClangTidyRegex { get; set; }

  static public Dictionary<string, List<string>> MemoryTags = new Dictionary<string, List<string>>();

  static public List<System.Diagnostics.Process> GenerateCompilerDBProcesses = new List<System.Diagnostics.Process>();
}
