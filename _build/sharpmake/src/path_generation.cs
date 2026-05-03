using Sharpmake;
using System.IO;
using System.Xml.Linq;

namespace rex
{
  static class PathGeneration
  {
    // Simple helper function to create a directory name that's unique per configuration
    public static string PerConfigFolderFormat(RexConfiguration conf)
    {
      return Path.Combine(conf.Target.GetFragment<Sharpmake.Compiler>().ToString(), conf.Target.ProjectConfigurationName);
    }
    // Simple helper function to get the path of the compiler db
    public static string GetCompilerDBOutputPath(RexConfiguration config)
    {
      return Path.Combine(GetCompilerDBOutputFolder(config), "compile_commands.json");
    }

    // Simple helper function to get the directory the compiler db will go to.
    public static string GetCompilerDBOutputFolder(RexConfiguration config)
    {
      return Path.Combine(GetClangToolsOutputFolder(config), PerConfigFolderFormat(config));
    }

    // Simple helper function to get the directory clang tools intermediate files get stored
    public static string GetClangToolsOutputFolder(RexConfiguration config)
    {
      return Path.Combine(config.ProjectPath, "clang_tools");
    }
    // Return a filter path for shaders. The filter path is the relative path from the shaders directory
    public static string DataFilterPath(string projectSourceRoot, string projectShaderPath, string relativeFilePath)
    {
      string absPath = Path.GetFullPath(Path.Combine(projectSourceRoot, relativeFilePath));
      string relativeFromShaders = Util.PathGetRelative(projectShaderPath, absPath, true);

      return Path.GetDirectoryName(Path.Combine("data", relativeFromShaders));
    }
    // Return the path of the clang-tools-config file
    public static string ClangToolsConfigFilePath(RexConfiguration conf)
    {
      return System.IO.Path.Combine(GetCompilerDBOutputFolder(conf), $"{conf.Project.Name}{Globals.ClangToolsProjectExtension}");
    }
    // Return the name of the module file, that's based on the configuration
    public static string FilenameForModule()
    {
      // hardcoded to a value with taking config into account
      // this is to make it easy on the runtime to find the file
      return "module.json";
    }
    // Return the path where the module file should be written to
    public static string CreateIntermediateModuleFilePath(RexConfiguration conf)
    {
      return Path.Combine(conf.IntermediatePath, $"{conf.Project.Name}_{conf.Name}_module.json");
    }
    // Return the path where the module file should be written to
    public static string CreateModuleFilePath(RexConfiguration conf)
    {
      return Path.Combine(conf.TargetPath, FilenameForModule());
    }

  }
}