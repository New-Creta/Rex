#pragma once

namespace rex
{
  struct DefaultTargetsInfo;
  struct DefaultDepthInfo;

  namespace globals
  {
    const DefaultTargetsInfo& default_targets_info();
    const DefaultDepthInfo& default_depth_info();
  } // namespace globals
} // namespace rex