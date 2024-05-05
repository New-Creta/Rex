#pragma once

namespace rex
{
  namespace renderer
  {
    // Specifies a comparison function
    enum class ComparisonFunc
    {
      Never, // Never pass the comparison
      Always, // Always pass the comparison
      Less, // If the source data is less than the destination data, the comparison passes
      LessEqual, // if the source data is less or equal to the destination data, the comparison passes
      Equal, // if the source is equal to the destination data, the comparison passes
      Greater, // if the source is greater than the destination data, the comparison passes
      GreaterEqual, // if the source is greater or equal to the destination data, the comparison passes
      NotEqual // if the source data is not equal to the destination data, the comparison passes
    };
  }
}