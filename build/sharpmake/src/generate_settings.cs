using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class GenerateSettings
{
  static public bool CoverageEnabled { get; set; }
  static public bool UnitTestsEnabled { get; set; }
  static public bool AddressSanitizerEnabled { get; set; }
  static public bool UndefinedBehaviorSanitizerEnabled { get; set; }
  static public bool FuzzyTestingEnabled { get; set; }
  static public bool VisualStudioOnly { get; set; }
}
