using System.Linq;
using System.IO;
using System.Collections.Generic;
using System.Text.Json;

[module: Sharpmake.Reference("System.Text.Json.dll")]

public class BuildSettings
{
  public string intermediate_folder { get; set; }
  public string[] misc_folders { get; set; }
  public string[] misc_extensions { get; set; }
  public string build_folder { get; set; }
  public string test_folder { get; set; }
  public string tools_folder { get; set; }
  public string libs_folder { get; set; }
  public string source_folder { get; set; }
  public string coverage_folder { get; set; }
  public string asan_folder { get; set; }
  public string ubsan_folder { get; set; }
}

public class Globals
{
  static readonly private string folder_in_root = "source";
  static private string root;
  static private string source_root;
  static private string thirdparty_root;
  static private string sharpmake_root;
  static private BuildSettings settings = null;

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
      return Path.Combine(root, settings.source_folder);
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
      return Path.Combine(root, settings.intermediate_folder, settings.tools_folder);
    }
  }
  static public string LibsRoot
  {
    get
    {
      return Path.Combine(root, settings.intermediate_folder, settings.libs_folder);
    }
  }

  static public void Init()
  {
    root = Utils.FindInParent(Directory.GetCurrentDirectory(), folder_in_root);

    string settings_json_path = Path.Combine(root, "build", "config", "settings.json");
    string json_blob = File.ReadAllText(settings_json_path);
    settings = JsonSerializer.Deserialize<BuildSettings>(json_blob);

    source_root = Path.Combine(root, settings.source_folder);
    thirdparty_root = Path.Combine(source_root, "0_thirdparty");
    sharpmake_root = Path.Combine(root, "build", "sharpmake");

    System.Console.WriteLine($"Root path:{root}");
  }
}
