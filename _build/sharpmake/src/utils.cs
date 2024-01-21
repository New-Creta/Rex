using System.IO;
using System.Linq;

public class Utils
{
  // Get the current file that calls this function
  public static string CurrentFile([System.Runtime.CompilerServices.CallerFilePath] string fileName = "")
  {
    return fileName;
  }
  // Looks for a directory or file in the parent directory recursively
  // returns the directory where the directory or file is found
  public static string FindInParent(string startPath, string toFind)
  {
    string current_directory = startPath;

    while (Directory.GetDirectories(current_directory).ToList().FindIndex(x => Path.GetFileName(x) == toFind) == -1 && Directory.GetFiles(current_directory).ToList().FindIndex(x => Path.GetFileName(x) == toFind) == -1)
    {
      if (Directory.GetDirectoryRoot(current_directory) == current_directory)
      {
        return "";
      }
      current_directory = Directory.GetParent(current_directory).FullName;
    }

    return current_directory;
  }

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
}