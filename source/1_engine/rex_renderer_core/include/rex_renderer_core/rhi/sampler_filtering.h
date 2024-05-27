#pragma once

namespace rex
{
  namespace renderer
  {
    // Specifies filtering options during texture sampling
    enum class SamplerFiltering
    {
      MinMagMipPoint,                            // Use point sampling for minifaction, magnification and mip-level sampling
      MinMagPointMipLinear,                      // Use point sampling for minifcation and magnification. Use linear interpolation for mip-level sampling
      MinPointMagLinearMipPoint,                 // Use point sampling for minifaction. Use linear interpolation for magnification use point sampling for mip-level sampling
      MinPointMagMipLinear,                      // Use point sampling for minification. Use linear interpolation for magnification and mip-level sampling.
      MinLinearMagMipPoint,                      // Use linear interpolation for minifcation. Use point sampling for magnification and mip-level sampling
      MinLinearMagPointMipLinear,                // Use linear interpolation for minification. Use point sampling for magnification. Use linear interpolation for mip-level sampling.
      MinMagLinearMipPoint,                      // Use linear interpolation for minification and magnification. Use point sampling for mip-level sampling
      MinMagMipLinear,                           // Use linear interpolation for minification, magnification and mip-level sampling
      Anisotropic,                               // Use anisotropic interpolation for minification, magnification and mip-level sampling.
      
      ComparisonMinMagMipPoint,                  // Use point sampling for minification, magnification and mip-level sampling. Compare the result to the comparison value.
      ComparisonMinMagPointMipLinear,            // Use point sampling for minification and magnification. Use linear interpolation for mip-level sampling. Compare the result to the comparison value.
      ComparisonMinPointMagLinearMipPoint,       // Use point sampling for minification. Use linear interpolation for magnification. Use point sampling for mip-level sampling. compare the result to the comparison value.
      ComparisonMinPointMagMipLinear,            // Use point sampling for minification. Use linear interpolation for magnification and mip-level sampling. Compare the result to the comparison value.
      ComparisonMinLinearMagMipPoint,            // Use linear interpolation for minification. Use point sampling for magnification and mip-level sampling. Compare the result to the comparison value.
      ComparisonMinLinearMagPointMipLinear,      // Use linear interpolation for minification. Use point sampling for magnification. Use linear interpolation for mip-level sampling. Compare the result to the comparison value.
      ComparisonMinMagLinearMipPoint,            // Use linear interpolation for minification and magnification. Use point sampling for mip-level sampling. Compare the result to the comparison value.
      ComparisonMinMagMipLinear,                 // Use linrar interpolation for minification, magnification and mip-level sampling. Compare the result to the comparison value.
      ComparisonAnisotropic,                     // Use anisotropic interpolation for minification, magnification and mip-level sampling. Compare the result to the comparison value.
      
      MinimumMinMagMipPoint,                     // Fetch the same set of texels as MinMagMipPoint and instead of filtering them return the minimum of the texels. Texels that are weighted 0 during filtering aren't counted towards the minimum.
      MinimumMinMagPointMipLinear,               // Fetch the same set of texels as MinMagPointMipLinear and instead of filtering them return the minimum of the texels. Texels that are weighted 0 during filtering aren't counted towards the minimum. 
      MinimumMinPointMagLinearMipPoint,          // Fetch the same set of texels as MinPointMagLinearMipPoint and instead of filtering them return the minimum of the texels. Texels that are weighted 0 during filtering aren't counted towards the minimum.
      MinimumMinPointMagMipLinear,               // Fetch the same set of texels as MinPointMagMipLinear and instead of filtering them return the minimum of the texels. Texels that are weighted 0 during filtering aren't counted towards the minimum. 
      MinimumMinLinearMagMipPoint,               // Fetch the same set of texels as MinLinearMagMipPoint and instead of filtering them return the minimum of the texels. Texels that are weighted 0 during filtering aren't counted towards the minimum. 
      MinimumMinLinearMagPointMipLinear,         // Fetch the same set of texels as MinLinearMagPointMipLinear and instead of filtering them return the minimum of the texels. Texels that are weighted 0 during filtering aren't counted towards the minimum. 
      MinimumMinMagLinearMipPoint,               // Fetch the same set of texels as MinMagLinearMipPoint and instead of filtering them return the minimum of the texels. Texels that are weighted 0 during filtering aren't counted towards the minimum. 
      MinimumMinMagMipLinear,                    // Fetch the same set of texels as MinMagMipLinear and instead of filtering them return the minimum of the texels. Texels that are weighted 0 during filtering aren't counted towards the minimum. 
      MinimumAnisotropic,                        // Fetch the same set of texels as Anisotropic and instead of filtering them return the minimum of the texels. Texels that are weighted 0 during filtering aren't counted towards the minimum. 
      
      MaximumMinMagMipPoint,                     // Fetch the same set of texels as MinMagMipPoint and instead of filtering them return the maximum of the texels. Texels that are weighted 0 during filtering aren't counted towards the maximum. 
      MaximumMinMagPointMipLinear,               // Fetch the same set of texels as MinMagPointMipLinear and instead of filtering them return the maximum of the texels. Texels that are weighted 0 during filtering aren't counted towards the maximum. 
      MaximumMinPointMagLinearMipPoint,          // Fetch the same set of texels as MinPointMagLinearMipPoint and instead of filtering them return the maximum of the texels. Texels that are weighted 0 during filtering aren't counted towards the maximum. 
      MaximumMinPointMagMipLinear,               // Fetch the same set of texels as MinPointMagMipLinear and instead of filtering them return the maximum of the texels. Texels that are weighted 0 during filtering aren't counted towards the maximum.
      MaximumMinLinearMagMipPoint,               // Fetch the same set of texels as MinLinearMagMipPoint and instead of filtering them return the maximum of the texels. Texels that are weighted 0 during filtering aren't counted towards the maximum.
      MaximumMinLinearMagPointMipLinear,         // Fetch the same set of texels as MinLinearMagPointMipLinear and instead of filtering them return the maximum of the texels. Texels that are weighted 0 during filtering aren't counted towards the maximum.
      MaximumMinMagLinearMipPoint,               // Fetch the same set of texels as MinMagLinearMipPoint and instead of filtering them return the maximum of the texels. Texels that are weighted 0 during filtering aren't counted towards the maximum.
      MaximumMinMagMipLinear,                    // Fetch the same set of texels as MinMagMipLinear and instead of filtering them return the maximum of the texels. Texels that are weighted 0 during filtering aren't counted towards the maximum.
      MaximumAnisotropic,                        // Fetch the same set of texels as Anisotropic and instead of filtering them return the maximum of the texels. Texels that are weighted 0 during filtering aren't counted towards the maximum.
    };
  }
}