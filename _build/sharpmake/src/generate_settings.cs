using Sharpmake;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

// these are the values that come the json file
public class EnumGenerationConfig
{
  public string ClassName { get; set; }
  public List<string> Values { get; set; }
  public string Filepath { get; set; }
}

public class ArrayGenerationConfig
{
  public string ElementType { get; set; }
  public string Name { get; set; }
  public List<string> Includes { get; set; }
  public List<string> Values { get; set; }
  public string Filepath { get; set; }
}

// this is the object we use
public class EnumGenerationSettings
{
  public string ClassName = "";
  public string Filepath = "";
  public Dictionary<string, List<string>> ProjectToEnumValues = new Dictionary<string, List<string>>();
  public Dictionary<string, string> ProjectToGenerationFile = new Dictionary<string, string>();
}

public class ArrayGenerationSettings
{
  public string ElementType = "";
  public string Name = "";
  public string Filepath = "";
  public List<string> Includes = new List<string>();
  public Dictionary<string, List<string>> ProjectToArrayValues = new Dictionary<string, List<string>>();
  public Dictionary<string, string> ProjectToGenerationFile = new Dictionary<string, string>();
}

public static class GenerationTypes
{
  public enum GraphicsAPI
  {
      Unknown,
      OpenGL,
      DirectX12
  }

  public enum Platform
  {
      Unknown,
      Windows,
      Unix
  }
};

public class GenerateSettings
{
  static public bool EnableVisualStudio = false;
  static public bool NoClangTools = false;
  static public bool PerformAllClangTidyChecks = false;
  static public bool DisableClangTidyForThirdParty = false;
  static public string IntermediateDir = "";

  // Test flags
  static public bool EnableDefaultGeneration = true;
  static public bool GenerateUnitTests = false;
  static public bool CoverageEnabled = false;
  static public bool AddressSanitizerEnabled = false;
  static public bool UndefinedBehaviorSanitizerEnabled = false;
  static public bool GenerateFuzzyTests = false;
  static public bool EnableAutoTests = false;


  static public string ClangTidyRegex { get; set; }
  static public GenerationTypes.GraphicsAPI GraphicsAPI = GenerationTypes.GraphicsAPI.Unknown;
  static public GenerationTypes.Platform Platform = GenerationTypes.Platform.Unknown;
  
  static public List<System.Diagnostics.Process> GenerateCompilerDBProcesses = new List<System.Diagnostics.Process>();
}
