using Sharpmake;
using System.IO;

[Generate]
public class Regina : ToolsProject
{
    public Regina() : base()
    {
        // The name of the project in Visual Studio. The default is the name of
        // the class, but you usually want to override that.
        Name = "Regina";
        GenerateTargets();

        string ThisFileFolder = Path.GetDirectoryName(Utils.CurrentFile());
        SourceRootPath = ThisFileFolder;
    }

    public override void Configure(RexConfiguration conf, RexTarget target)
    {
        base.Configure(conf, target);

        conf.Output = Configuration.OutputType.Exe;

        switch (target.Platform)
        {
            case Platform.win32:
            case Platform.win64:
                conf.add_dependency<RexWindows>(target);
                conf.Options.Add(Options.Vc.Linker.SubSystem.Windows);
                break;
        }
    }
}
