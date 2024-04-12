using Sharpmake;
using System.IO;

[Generate]
public class NtfsJournalParser : ToolsProject
{
  public NtfsJournalParser() : base()
  {
    Name = "NtfsJournalParser";
    GenerateTargets();

    string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
    SourceRootPath = ThisFileFolder;
  }

  public override void Configure(RexConfiguration conf, RexTarget target)
  {
    base.Configure(conf, target);

    conf.AddPublicDependency<RexWindows>(target);

    conf.Output = Configuration.OutputType.Exe;
    conf.Options.Add(Options.Vc.Linker.SubSystem.Console);
  }
}