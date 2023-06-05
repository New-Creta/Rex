#pragma once

#ifndef REX_PATH_DEFINES
  #define REX_PATH_DEFINES

  //-------------------------------------------------------------------------
  // File path separator
  #ifndef REX_FOLDER_SEPS
    #ifdef REX_PLATFORM_WINDOWS
      #define REX_FOLDER_SEPS "\\/"
    #else
      #define REX_FOLDER_SEPS "/"
    #endif
  #endif

#endif