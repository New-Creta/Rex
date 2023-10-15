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

  public enum IDE
  {
    VisualStudio,
    VSCode
  }

  public class GenerateCompilerDBCommand
  {
    public string NinjaFile { get; set; }
    public string CompilerDBBuildCommand { get; set; }
    public string OutputPath { get; set; }

    public GenerateCompilerDBCommand(string ninjaFile, string compilerDBBuildCommand, string output)
    {
      NinjaFile = ninjaFile;
      CompilerDBBuildCommand = compilerDBBuildCommand;
      OutputPath = output;
    }
  }
};

public class GenerateSettings
{
  static public bool NoClangTools = false;
  static public bool PerformAllClangTidyChecks = false;
  static public bool DisableClangTidyForThirdParty = false;
  static public string IntermediateDir = "";
  static public string ConfigFileDir = "";

  // Flags for various different tests
  static public bool EnableDefaultGeneration = true;
  static public bool UnitTestsEnabled = false;
  static public bool CoverageEnabled = false;
  static public bool AsanEnabled = false;
  static public bool UbsanEnabled = false;
  static public bool FuzzyTestingEnabled = false;
  static public bool AutoTestsEnabled = false;

  static public GenerationTypes.IDE IDE = GenerationTypes.IDE.VisualStudio;

  static public string ClangTidyRegex { get; set; }
  static public GenerationTypes.GraphicsAPI GraphicsAPI = GenerationTypes.GraphicsAPI.Unknown;
  static public GenerationTypes.Platform Platform = GenerationTypes.Platform.Unknown;
  
  static public List<System.Diagnostics.Process> GenerateCompilerDBProcesses = new List<System.Diagnostics.Process>();
  static public List<GenerationTypes.GenerateCompilerDBCommand> GenerateCompilerDBCommands = new List<GenerationTypes.GenerateCompilerDBCommand>();
}
