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
}