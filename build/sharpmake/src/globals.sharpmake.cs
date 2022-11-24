using System.Linq;
using System.IO;

public class Globals
{
  static readonly private string folder_in_root = "source";
  static private string root;
  static private string source_root;

  static public string Root
  {
    get
    {
      return root;
    }
  }
  static public string SourceRoot
  {
    get
    {
      return source_root;
    }
  }

  static public void Init()
  {
    string current_directory = Directory.GetCurrentDirectory();

    while (Directory.GetDirectories(current_directory).ToList().FindIndex(x => Path.GetFileName(x) == folder_in_root) == -1)
    {
      if (Directory.GetDirectoryRoot(current_directory) == current_directory)
      {
        throw new System.Exception("Failed to find root directory");
      }
      current_directory = Directory.GetParent(current_directory).FullName;
    }

    root = current_directory;
    source_root = Path.Combine(root, "source");
    System.Console.WriteLine($"Root path:{root}");
  }
}
