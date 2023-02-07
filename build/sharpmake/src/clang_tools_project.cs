using System.Collections.Generic;
using System.IO;

class ClangToolsProject
{
  private string Name;
  private string Path;

  public ClangToolsProject(string name, string path)
  {
    Name = name;
    Path = path;
  }

  public List<string> HeaderFilters { get; set; }

  public string ProjectPath
  {
    get
    {
      return System.IO.Path.Combine(Path, $"{Name}.project");
    }
  }
}