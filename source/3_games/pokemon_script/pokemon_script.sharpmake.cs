using Sharpmake;
using System.IO;

[Sharpmake.Generate]
public class PokemonCSharp : BasicCSProject
{
  public PokemonCSharp()
  {
    Name = "PokemonCSharp";
    SourceFilesExtensions.Clear();
    SourceFilesExtensions.Add(".cs");

    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = Path.Combine(ThisFileFolder, "src");

    // Ignore the data folder, this folder can become very big
    // and will never contain any sharpmake files
    SourceFilesExcludeRegex.Add("data");

    // ignore the templates folder, these are used for auto generation, shouldn't be added to the project itself
    SourceFilesExcludeRegex.Add("templates");

    AddTargets(RexTarget.CreateTargets().ToArray());
  }

  protected override void SetupSolutionFolder(RexConfiguration conf, RexTarget target)
  {
    conf.SolutionFolder = "3_games";
  }

  protected override void SetupOutputType(RexConfiguration conf, RexTarget target)
  {
    conf.Output = Configuration.OutputType.DotNetClassLibrary;
  }

  protected override void SetupLibDependencies(RexConfiguration conf, RexTarget target)
  {
    base.SetupLibDependencies(conf, target);

    conf.AddPublicDependency<PokemonProject>(target);

    conf.ReferencesByName.AddRange(new Strings("System",
                                               "System.Core",
                                               "System.Memory",
                                               "System.Runtime.Loader"
                                               ));

    conf.ReferencesByNuGetPackage.Add("System.Text.Json", "5.0.2"); // same version sharpmake uses
  }
}