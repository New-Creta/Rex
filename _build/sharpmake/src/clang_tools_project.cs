using System.Collections.Generic;
using System.IO;

// The clang tools project is used by the post build events
// They read the header filters and only enable clang-tidy
// for files who match the regex
class ClangToolsProject
{
  private string Name; // The name of the project
  private string Path; // The directory where it'll be stored

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
      return System.IO.Path.Combine(Path, $"{Name}{Globals.ClangToolsProjectExtension}");
    }
  }
}