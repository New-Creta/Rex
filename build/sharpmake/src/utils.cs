using System.IO;
using System.Linq;

public class Utils
{
  public static int CurrentLine([System.Runtime.CompilerServices.CallerLineNumber] int lineNumber = 0)
  {
    return lineNumber;
  }
  public static string CurrentFile([System.Runtime.CompilerServices.CallerFilePath] string fileName = "")
  {
    return fileName;
  }
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