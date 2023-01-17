using System.Linq;
using System.IO;
using System.Collections.Generic;
using System.Text.Json;

[module: Sharpmake.Reference("System.Text.Json.dll")]

public class Globals
{
  static readonly private string folder_in_root = "source";
  static private string root;
  static private string source_root;
  static private string thirdparty_root;
  static private string sharpmake_root;
  static private string tools_root;
  static private string libs_root;

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
  static public string ThirdpartyRoot
  {
    get
    {
      return thirdparty_root;
    }
  }
  static public string SharpmakeRoot
  {
    get
    {
      return sharpmake_root;
    }
  }
  static public string ToolsRoot
  {
    get
    {
      return tools_root;
    }
  }
  static public string LibsRoot
  {
    get
    {
      return libs_root;
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

    string settings_json_path = Path.Combine(root, "build", "config", "settings.json");
    string json_blob = File.ReadAllText(settings_json_path);
    Dictionary<string, string> settings = JsonSerializer.Deserialize<Dictionary<string, string>>(json_blob);


    source_root = Path.Combine(root, settings["source_folder"]);
    thirdparty_root = Path.Combine(source_root, "0_thirdparty");
    sharpmake_root = Path.Combine(root, "build", "sharpmake");
    tools_root = Path.Combine(root, settings["intermediate_folder"], settings["tools_folder"]);
    libs_root = Path.Combine(root, settings["intermediate_folder"], settings["libs_folder"]);
    System.Console.WriteLine($"Root path:{root}");
  }
}
