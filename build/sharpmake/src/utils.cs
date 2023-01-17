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
}