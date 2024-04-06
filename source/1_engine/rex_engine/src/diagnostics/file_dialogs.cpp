/* this file can be renamed with extension ".cpp" and compiled as C++.
The code is 100% compatible C C++
(just comment out << extern "C" >> in the header file) */

/*_________
 /         \ tinyfiledialogs.c v3.13.2 [May 31, 2023] zlib licence
 |tiny file| Unique code file created [November 9, 2014]
 | dialogs | Copyright (c) 2014 - 2023 Guillaume Vareille http://ysengrin.com
 \____  ___/ http://tinyfiledialogs.sourceforge.net
      \|     git clone http://git.code.sf.net/p/tinyfiledialogs/code tinyfd
              ____________________________________________
             |                                            |
             |   email: tinyfiledialogs at ysengrin.com   |
             |____________________________________________|
  _________________________________________________________________________________
 |                                                                                 |
 | the windows only wchar_t UTF-16 prototypes are at the bottom of the header file |
 |_________________________________________________________________________________|
  _________________________________________________________
 |                                                         |
 | on windows: - since v3.6 char is UTF-8 by default       |
 |             - if you want MBCS set tinyfd_winUtf8 to 0  |
 |             - functions like fopen expect MBCS          |
 |_________________________________________________________|

If you like tinyfiledialogs, please upvote my stackoverflow answer
https://stackoverflow.com/a/47651444

- License -
This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:
1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software.  If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
-----------

Thanks for contributions, bug corrections & thorough testing to:
- Don Heyse http://ldglite.sf.net for bug corrections & thorough testing!
- Paul Rouget
*/

#ifndef __sun
  #ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 2 /* to accept POSIX 2 in old ANSI C standards */
  #endif
#endif

#if !defined(_WIN32) && (defined(__GNUC__) || defined(__clang__))
  #if !defined(_GNU_SOURCE)
    #define _GNU_SOURCE /* used only to resolve symbolic links. Can be commented out */
  #endif
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32

// clang-format off
  #include <windows.h>
// clang-format on

  #include <commdlg.h>
  #include <conio.h>
  #include <direct.h>
  #include <shlobj.h>
  #define TINYFD_NOCCSUNICODE
#else
  #include <dirent.h> /* on old systems try <sys/dir.h> instead */
  #include <limits.h>
  #include <signal.h> /* on old systems try <sys/signal.h> instead */
  #include <sys/utsname.h>
  #include <termios.h>
  #include <unistd.h>
  #define SLASH "/"
#endif /* _WIN32 */

#include "rex_engine/diagnostics/file_dialogs.h"
#include "rex_engine/engine/types.h"
#include "rex_engine/filesystem/path.h"
#include "rex_std/algorithm.h"
#include "rex_std/bonus/utility/enum_reflection.h"
#include "rex_std/bonus/utility/yes_no.h"

#ifdef REX_PLATFORM_WINDOWS
  #include "rex_engine/platform/win/win_com_library.h"
#endif

namespace rex
{
  namespace dialog
  {
    namespace internal
    {
      rsl::string_view path_without_final_slash(rsl::string_view path)
      {
        if(path.ends_with('/') || path.ends_with('\\'))
        {
          return path.substr(0, path.length() - 1);
        }

        return path;
      }

      bool quotes_detected(rsl::string_view path)
      {
        if(path.find('\'') != path.npos())
        {
          return true;
        }

        if(path.find('\"') != path.npos())
        {
          return true;
        }

        return false;
      }

      DEFINE_YES_NO_ENUM(AllowMultipleSelection);

      rsl::vector<char> open_file_dialog(rsl::string_view title, rsl::string_view defaultPath, rsl::string_view filter, AllowMultipleSelection allowMultipleSelection)
      {
        if(internal::quotes_detected(title))
        {
          return open_file_dialog("INVALID TITLE WITH QUOTES", defaultPath, filter, allowMultipleSelection);
        }
        if(internal::quotes_detected(defaultPath))
        {
          return open_file_dialog(title, "INVALID DEFAULT_PATH WITH QUOTES", filter, allowMultipleSelection);
        }
        if(internal::quotes_detected(filter))
        {
          return open_file_dialog(title, defaultPath, "INVALID FILTER_DESCRIPTION WITH QUOTES", allowMultipleSelection);
        }

        const s32 max_num_selectable_files = allowMultipleSelection ? 32 // a value taken out of thin air
                                                                    : 1;
        rsl::vector<char> buffer(rsl::Size(max_num_selectable_files * MAX_PATH + 1));

        auto dirname = internal::path_without_final_slash(defaultPath);

        rsl::vector<rsl::string_view> filters = rsl::split(filter, ";");

        rsl::string filter_patterns;
        filter_patterns.reserve(filter.size());
        for(rsl::string_view splitted_filter: filters)
        {
          filter_patterns += splitted_filter;
          filter_patterns += ";";
        }
        filter_patterns += "\n";
        filter_patterns += "All Files\n*.*\n";
        filter_patterns.replace("\n", "\0");

        OPENFILENAMEA ofn     = {0};
        ofn.lStructSize       = sizeof(OPENFILENAME);
        ofn.hwndOwner         = GetForegroundWindow();
        ofn.hInstance         = 0;
        ofn.lpstrFilter       = filter_patterns.size() ? filter_patterns.data() : NULL;
        ofn.lpstrCustomFilter = NULL;
        ofn.nMaxCustFilter    = 0;
        ofn.nFilterIndex      = 1;
        ofn.lpstrFile         = buffer.data();
        ofn.nMaxFile          = buffer.size();
        ofn.lpstrFileTitle    = NULL;
        ofn.nMaxFileTitle     = MAX_PATH / 2;
        ofn.lpstrInitialDir   = dirname.size() ? dirname.data() : nullptr;
        ofn.lpstrTitle        = title.size() ? title.data() : nullptr;
        ofn.Flags             = OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        ofn.nFileOffset       = 0;
        ofn.nFileExtension    = 0;
        ofn.lpstrDefExt       = NULL;
        ofn.lCustData         = 0L;
        ofn.lpfnHook          = NULL;
        ofn.lpTemplateName    = NULL;

        if(allowMultipleSelection)
        {
          ofn.Flags |= OFN_ALLOWMULTISELECT;
        }

        return buffer;
      }
    } // namespace internal

    void beep(void)
    {
      Beep(440, 300);
    }

#ifdef _WIN32

    int msg_box(rsl::string_view title, rsl::string_view msg, DialogType dialogType, IconType iconType, DefaultButton defaultButton)
    {
      // Detecht invalid title or messages
      if(internal::quotes_detected(title))
      {
        return msg_box("INVALID TITLE WITH QUOTES", msg, dialogType, iconType, defaultButton);
      }
      if(internal::quotes_detected(msg))
      {
        return msg_box(title, "INVALID MESSAGE WITH QUOTES", dialogType, iconType, defaultButton);
      }

      // Create the code to be passed in to Win API
      UINT aCode = 0;
      switch(iconType)
      {
        case rex::dialog::IconType::Info: aCode = MB_ICONINFORMATION; break;
        case rex::dialog::IconType::Warning: aCode = MB_ICONWARNING; break;
        case rex::dialog::IconType::Error: aCode = MB_ICONERROR; break;
        case rex::dialog::IconType::Question: aCode = MB_ICONQUESTION; break;
        default: aCode = MB_ICONINFORMATION; break;
      }

      switch(dialogType)
      {
        case rex::dialog::DialogType::Ok: aCode += MB_OK; break;
        case rex::dialog::DialogType::OkCancel:
          aCode += MB_OKCANCEL;
          if(defaultButton == DefaultButton::Cancel)
          {
            aCode += MB_DEFBUTTON2;
          }
          break;
        case rex::dialog::DialogType::YesNo:
          aCode += MB_YESNO;
          if(defaultButton == DefaultButton::Cancel)
          {
            aCode += MB_DEFBUTTON2;
          }
          break;
        case rex::dialog::DialogType::YesNoCancel:
          aCode += MB_YESNOCANCEL;
          if(defaultButton == DefaultButton::Yes)
          {
            aCode += MB_DEFBUTTON1;
          }
          else if(defaultButton == DefaultButton::No)
          {
            aCode += MB_DEFBUTTON2;
          }
          else
          {
            aCode += MB_DEFBUTTON3;
          }
          break;
        default: aCode += MB_OK; break;
      }

      aCode += MB_TOPMOST;

      int lBoxReturnValue = MessageBoxA(GetForegroundWindow(), msg.data(), title.data(), aCode);

      if((lBoxReturnValue == IDNO) && (dialogType == DialogType::YesNoCancel))
      {
        return 2;
      }
      else if((lBoxReturnValue == IDOK) || (lBoxReturnValue == IDYES))
      {
        return 1;
      }
      else
      {
        return 0;
      }
    }

  #define WM_TRAYICON (WM_USER + 1)

    int& new_baloon_tip_id()
    {
      static int x = 0;
      x++;
      return x;
    }

    class BaloonTip
    {
    public:
      BaloonTip(rsl::string_view title, rsl::string_view msg, rsl::string_view tip, IconType iconType)
          : m_nid()
      {
        m_nid.cbSize           = sizeof(NOTIFYICONDATA);
        m_nid.hWnd             = GetForegroundWindow();
        m_nid.uID              = new_baloon_tip_id(); // This can be a random number, as long as there aren't any 2 track icons active with the same
        m_nid.uFlags           = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
        m_nid.uCallbackMessage = WM_TRAYICON;
        m_nid.hIcon            = LoadIcon(NULL, IDI_APPLICATION); // This is the icon shown to the left or your application
        memcpy(m_nid.szTip, tip.data(), tip.length());

        // Balloon tip settings
        m_nid.uTimeout    = 10000;                // 10 seconds
        m_nid.dwInfoFlags = info_flags(iconType); // This is the bigger icon shown in the BaloonTip
        memcpy(m_nid.szInfoTitle, title.data(), title.length());
        memcpy(m_nid.szInfo, msg.data(), msg.length());

        Shell_NotifyIcon(NIM_ADD, &m_nid);
        Shell_NotifyIcon(NIM_MODIFY, &m_nid);
      }

      ~BaloonTip()
      {
        Shell_NotifyIcon(NIM_DELETE, &m_nid);
      }

    private:
      int info_flags(IconType iconType)
      {
        switch(iconType)
        {
          case rex::dialog::IconType::Info: return NIIF_INFO; break;
          case rex::dialog::IconType::Warning: return NIIF_WARNING; break;
          case rex::dialog::IconType::Error: return NIIF_ERROR; break;
          case rex::dialog::IconType::Question: return NIIF_INFO; break;
          default: return NIIF_INFO; break;
        }
      }

    private:
      NOTIFYICONDATA m_nid;
    };

    /* return has only meaning for tinyfd_query */
    int popup(rsl::string_view title, rsl::string_view msg, rsl::string_view tip, IconType iconType)
    {
      BaloonTip baloon_tip(title, msg, tip, iconType);

      return 0;
    }

    rsl::medium_stack_string save_file_dialog(rsl::string_view title, rsl::string_view defaultPath, rsl::string_view filter)
    {
      if(internal::quotes_detected(title))
      {
        return save_file_dialog("INVALID TITLE WITH QUOTES", defaultPath, filter);
      }
      if(internal::quotes_detected(defaultPath))
      {
        return save_file_dialog(title, "INVALID DEFAULT_PATH WITH QUOTES", filter);
      }
      if(internal::quotes_detected(filter))
      {
        return save_file_dialog(title, defaultPath, "INVALID FILTER_DESCRIPTION WITH QUOTES");
      }

      rsl::vector<rsl::string_view> filters = rsl::split(filter, ";");

      rsl::string filter_patterns;
      filter_patterns.reserve(filter.size());
      for(rsl::string_view splitted_filter: filters)
      {
        filter_patterns += splitted_filter;
        filter_patterns += ";";
      }
      filter_patterns += "\n";
      filter_patterns += "All Files\n*.*\n";
      filter_patterns.replace("\n", "\0");

      defaultPath              = internal::path_without_final_slash(defaultPath);
      rsl::string_view dirname = rex::path::dir_name(defaultPath);
      rsl::medium_stack_string result;

      OPENFILENAMEA ofn     = {0};
      ofn.lStructSize       = sizeof(OPENFILENAMEA);
      ofn.hwndOwner         = GetForegroundWindow();
      ofn.hInstance         = 0;
      ofn.lpstrFilter       = filter_patterns.data();
      ofn.lpstrCustomFilter = NULL;
      ofn.nMaxCustFilter    = 0;
      ofn.nFilterIndex      = 1;
      ofn.lpstrFile         = result.data();

      ofn.nMaxFile        = MAX_PATH;
      ofn.lpstrFileTitle  = NULL;
      ofn.nMaxFileTitle   = MAX_PATH / 2;
      ofn.lpstrInitialDir = dirname.data();
      ofn.lpstrTitle      = title.data();
      ofn.Flags           = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;
      ofn.nFileOffset     = 0;
      ofn.nFileExtension  = 0;
      ofn.lpstrDefExt     = filter.size() ? filter.data() : nullptr;
      ofn.lCustData       = 0L;
      ofn.lpfnHook        = NULL;
      ofn.lpTemplateName  = NULL;

      if(GetSaveFileNameA(&ofn))
      {
        result.reset_null_termination_offset();
      }

      return result;
    }

    rsl::vector<rsl::string> open_file_dialog_multiselect(rsl::string_view title, rsl::string_view defaultPath, rsl::string_view filter)
    {
      rsl::vector<char> buffer = internal::open_file_dialog(title, defaultPath, filter, internal::AllowMultipleSelection::yes);

      rsl::vector<rsl::string> selected_files;
      const char* start = buffer.data();
      while(start)
      {
        count_t string_length = rsl::strlen(start);
        rsl::string_view str(start, string_length);
        start += string_length + 1;
        selected_files.emplace_back(str);
      }

      return selected_files;
    }

    rsl::medium_stack_string open_file_dialog(rsl::string_view title, rsl::string_view defaultPath, rsl::string_view filter)
    {
      rsl::vector<char> buffer = internal::open_file_dialog(title, defaultPath, filter, internal::AllowMultipleSelection::no);

      rsl::medium_stack_string result(buffer.data(), buffer.size());

      return result;
    }

    rsl::big_stack_string open_folder_dialog(rsl::string_view title, rsl::string_view defaultPath)
    {
      if(internal::quotes_detected(title))
      {
        return open_folder_dialog("INVALID TITLE WITH QUOTES", defaultPath);
      }
      if(internal::quotes_detected(defaultPath))
      {
        return open_folder_dialog(title, "INVALID DEFAULT_PATH WITH QUOTES");
      }

      auto file_open_dialog = rex::win::com_lib::create_com_object<IFileOpenDialog>(CLSID_FileOpenDialog);
      FILEOPENDIALOGOPTIONS options {};

      options |= FOS_PICKFOLDERS;
      file_open_dialog->SetOptions(options);

      HWND parent_handle = nullptr;
      rsl::big_stack_string result;
      if(HR_FAILED_IGNORE(file_open_dialog->Show(parent_handle), HRESULT_FROM_WIN32(ERROR_CANCELLED)))
      {
        return result;
      }

      rex::wrl::ComPtr<IShellItem> item;
      HR_CALL(file_open_dialog->GetResult(item.GetAddressOf()));

      PWSTR psz_file_path;
      HR_CALL(item->GetDisplayName(SIGDN_FILESYSPATH, &psz_file_path));
      WideCharToMultiByte(CP_UTF8, 0, psz_file_path, -1, result.data(), result.max_size(), "\0", NULL);
      CoTaskMemFree(psz_file_path);

      return result;
    }

    rsl::Rgb color_chooser(rsl::string_view title, rsl::Rgb defaultRgb)
    {
      if(internal::quotes_detected(title))
      {
        return color_chooser("INVALID TITLE WITH QUOTES", defaultRgb);
      }

      // This isn't used, but we need it to call the color picker
      COLORREF crCustColors[16];

      /* we can't use title */
      CHOOSECOLORW cc {};
      cc.lStructSize    = sizeof(CHOOSECOLOR);
      cc.hwndOwner      = GetForegroundWindow();
      cc.hInstance      = NULL;
      cc.rgbResult      = RGB(defaultRgb.red, defaultRgb.green, defaultRgb.blue);
      cc.lpCustColors   = crCustColors;
      cc.Flags          = CC_RGBINIT | CC_FULLOPEN | CC_ANYCOLOR;
      cc.lCustData      = 0;
      cc.lpfnHook       = NULL;
      cc.lpTemplateName = NULL;

      int lRet = ChooseColorW(&cc);

      rsl::Rgb res {};
      if(!lRet)
      {
        return rsl::Rgb();
      }

      res.green = GetRValue(cc.rgbResult);
      res.green = GetGValue(cc.rgbResult);
      res.blue  = GetBValue(cc.rgbResult);

      return res;
    }

#else /* unix */

    static char gPython2Name[16];
    static char gPython3Name[16];
    static char gPythonName[16];

    int tfd_isDarwin(void)
    {
      static int lsIsDarwin = -1;
      struct utsname lUtsname;
      if(lsIsDarwin < 0)
      {
        lsIsDarwin = !uname(&lUtsname) && !strcmp(lUtsname.sysname, "Darwin");
      }
      return lsIsDarwin;
    }

    static int dirExists(const char* aDirPath)
    {
      DIR* lDir;
      if(!aDirPath || !strlen(aDirPath))
        return 0;
      lDir = opendir(aDirPath);
      if(!lDir)
      {
        return 0;
      }
      closedir(lDir);
      return 1;
    }

    static int detectPresence(const char* aExecutable)
    {
      char dirname[MAX_PATH];
      char lTestedString[MAX_PATH] = "which ";
      FILE* lIn;
  #ifdef _GNU_SOURCE
      char* lAllocatedCharString;
      int lSubstringUndetected;
  #endif

      strcat(lTestedString, aExecutable);
      strcat(lTestedString, " 2>/dev/null ");
      lIn = popen(lTestedString, "r");
      if((fgets(dirname, sizeof(dirname), lIn) != NULL) && (!strchr(dirname, ':')) && (strncmp(dirname, "no ", 3)))
      { /* present */
        pclose(lIn);

  #ifdef _GNU_SOURCE /*to bypass this, just comment out "#define _GNU_SOURCE" at the top of the file*/
        if(dirname[strlen(dirname) - 1] == '\n')
          dirname[strlen(dirname) - 1] = '\0';
        lAllocatedCharString = realpath(dirname, NULL); /*same as canonicalize_file_name*/
        lSubstringUndetected = !strstr(lAllocatedCharString, aExecutable);
        free(lAllocatedCharString);
        if(lSubstringUndetected)
        {
          if(tinyfd_verbose)
            printf("detectPresence %s %d\n", aExecutable, 0);
          return 0;
        }
  #endif             /*_GNU_SOURCE*/

        if(tinyfd_verbose)
          printf("detectPresence %s %d\n", aExecutable, 1);
        return 1;
      }
      else
      {
        pclose(lIn);
        if(tinyfd_verbose)
          printf("detectPresence %s %d\n", aExecutable, 0);
        return 0;
      }
    }

    static char* getVersion(const char* aExecutable) /*version must be first numeral*/
    {
      static char dirname[MAX_PATH];
      char lTestedString[MAX_PATH];
      FILE* lIn;
      char* lTmp;

      strcpy(lTestedString, aExecutable);
      strcat(lTestedString, " --version");

      lIn  = popen(lTestedString, "r");
      lTmp = fgets(dirname, sizeof(dirname), lIn);
      pclose(lIn);

      lTmp += strcspn(lTmp, "0123456789");
      /* printf("lTmp:%s\n", lTmp); */
      return lTmp;
    }

    static int* getMajorMinorPatch(const char* aExecutable)
    {
      static int lArray[3];
      char* lTmp;

      lTmp      = (char*)getVersion(aExecutable);
      lArray[0] = atoi(strtok(lTmp, " ,.-"));
      /* printf("lArray0 %d\n", lArray[0]); */
      lArray[1] = atoi(strtok(0, " ,.-"));
      /* printf("lArray1 %d\n", lArray[1]); */
      lArray[2] = atoi(strtok(0, " ,.-"));
      /* printf("lArray2 %d\n", lArray[2]); */

      if(!lArray[0] && !lArray[1] && !lArray[2])
        return NULL;
      return lArray;
    }

    static int tryCommand(const char* aCommand)
    {
      char dirname[MAX_PATH];
      FILE* lIn;

      lIn = popen(aCommand, "r");
      if(fgets(dirname, sizeof(dirname), lIn) == NULL)
      { /* present */
        pclose(lIn);
        return 1;
      }
      else
      {
        pclose(lIn);
        return 0;
      }
    }

    static int isTerminalRunning(void)
    {
      static int lIsTerminalRunning = -1;
      if(lIsTerminalRunning < 0)
      {
        lIsTerminalRunning = isatty(1);
        if(tinyfd_verbose)
          printf("isTerminalRunning %d\n", lIsTerminalRunning);
      }
      return lIsTerminalRunning;
    }

    static char* dialogNameOnly(void)
    {
      static char lDialogName[128] = "*";
      if(lDialogName[0] == '*')
      {
        if(!tinyfd_allowCursesDialogs)
        {
          strcpy(lDialogName, "");
        }
        else if(tfd_isDarwin() && *strcpy(lDialogName, "/opt/local/bin/dialog") && detectPresence(lDialogName))
        {
        }
        else if(*strcpy(lDialogName, "dialog") && detectPresence(lDialogName))
        {
        }
        else
        {
          strcpy(lDialogName, "");
        }
      }
      return lDialogName;
    }

    int isDialogVersionBetter09b(void)
    {
      const char* lDialogName;
      char* lVersion;
      int lMajor;
      int lMinor;
      int lDate;
      int lResult;
      char* lMinorP;
      char* lLetter;
      char dirname[128];

      /*char lTest[128] = " 0.9b-20031126" ;*/

      lDialogName = dialogNameOnly();
      if(!strlen(lDialogName) || !(lVersion = (char*)getVersion(lDialogName)))
        return 0;
      /*lVersion = lTest ;*/
      /*printf("lVersion %s\n", lVersion);*/
      strcpy(dirname, lVersion);
      lMajor = atoi(strtok(lVersion, " ,.-"));
      /*printf("lMajor %d\n", lMajor);*/
      lMinorP = strtok(0, " ,.-abcdefghijklmnopqrstuvxyz");
      lMinor  = atoi(lMinorP);
      /*printf("lMinor %d\n", lMinor );*/
      lDate = atoi(strtok(0, " ,.-"));
      if(lDate < 0)
        lDate = -lDate;
      /*printf("lDate %d\n", lDate);*/
      lLetter = lMinorP + strlen(lMinorP);
      strcpy(lVersion, dirname);
      strtok(lLetter, " ,.-");
      /*printf("lLetter %s\n", lLetter);*/
      lResult = (lMajor > 0) || ((lMinor == 9) && (*lLetter == 'b') && (lDate >= 20031126));
      /*printf("lResult %d\n", lResult);*/
      return lResult;
    }

    static int whiptailPresentOnly(void)
    {
      static int lWhiptailPresent = -1;
      if(!tinyfd_allowCursesDialogs)
        return 0;
      if(lWhiptailPresent < 0)
      {
        lWhiptailPresent = detectPresence("whiptail");
      }
      return lWhiptailPresent;
    }

    static char* terminalName(void)
    {
      static char lTerminalName[128] = "*";
      char lShellName[64]            = "*";
      int* lArray;

      if(lTerminalName[0] == '*')
      {
        if(detectPresence("bash"))
        {
          strcpy(lShellName, "bash -c "); /*good for basic input*/
        }
        else if(strlen(dialogNameOnly()) || whiptailPresentOnly())
        {
          strcpy(lShellName, "sh -c "); /*good enough for dialog & whiptail*/
        }
        else
        {
          strcpy(lTerminalName, "");
          return NULL;
        }

        if(tfd_isDarwin())
        {
          if(*strcpy(lTerminalName, "/opt/X11/bin/xterm") && detectPresence(lTerminalName))
          {
            strcat(lTerminalName, " -fa 'DejaVu Sans Mono' -fs 10 -title tinyfiledialogs -e ");
            strcat(lTerminalName, lShellName);
          }
          else
          {
            strcpy(lTerminalName, "");
          }
        }
        else if(*strcpy(lTerminalName, "xterm") /*good (small without parameters)*/
                && detectPresence(lTerminalName))
        {
          strcat(lTerminalName, " -fa 'DejaVu Sans Mono' -fs 10 -title tinyfiledialogs -e ");
          strcat(lTerminalName, lShellName);
        }
        else if(*strcpy(lTerminalName, "terminator") /*good*/
                && detectPresence(lTerminalName))
        {
          strcat(lTerminalName, " -x ");
          strcat(lTerminalName, lShellName);
        }
        else if(*strcpy(lTerminalName, "lxterminal") /*good*/
                && detectPresence(lTerminalName))
        {
          strcat(lTerminalName, " -e ");
          strcat(lTerminalName, lShellName);
        }
        else if(*strcpy(lTerminalName, "konsole") /*good*/
                && detectPresence(lTerminalName))
        {
          strcat(lTerminalName, " -e ");
          strcat(lTerminalName, lShellName);
        }
        else if(*strcpy(lTerminalName, "kterm") /*good*/
                && detectPresence(lTerminalName))
        {
          strcat(lTerminalName, " -e ");
          strcat(lTerminalName, lShellName);
        }
        else if(*strcpy(lTerminalName, "tilix") /*good*/
                && detectPresence(lTerminalName))
        {
          strcat(lTerminalName, " -e ");
          strcat(lTerminalName, lShellName);
        }
        else if(*strcpy(lTerminalName, "xfce4-terminal") /*good*/
                && detectPresence(lTerminalName))
        {
          strcat(lTerminalName, " -x ");
          strcat(lTerminalName, lShellName);
        }
        else if(*strcpy(lTerminalName, "mate-terminal") /*good*/
                && detectPresence(lTerminalName))
        {
          strcat(lTerminalName, " -x ");
          strcat(lTerminalName, lShellName);
        }
        else if(*strcpy(lTerminalName, "Eterm") /*good*/
                && detectPresence(lTerminalName))
        {
          strcat(lTerminalName, " -e ");
          strcat(lTerminalName, lShellName);
        }
        else if(*strcpy(lTerminalName, "evilvte") /*good*/
                && detectPresence(lTerminalName))
        {
          strcat(lTerminalName, " -e ");
          strcat(lTerminalName, lShellName);
        }
        else if(*strcpy(lTerminalName, "pterm") /*good (only letters)*/
                && detectPresence(lTerminalName))
        {
          strcat(lTerminalName, " -e ");
          strcat(lTerminalName, lShellName);
        }
        else if(*strcpy(lTerminalName, "gnome-terminal") && detectPresence(lTerminalName) && (lArray = getMajorMinorPatch(lTerminalName)) && ((lArray[0] < 3) || (lArray[0] == 3 && lArray[1] <= 6)))
        {
          strcat(lTerminalName, " --disable-factory -x ");
          strcat(lTerminalName, lShellName);
        }
        else
        {
          strcpy(lTerminalName, "");
        }
        /* bad: koi rxterm guake tilda vala-terminal qterminal kgx
        aterm Terminal terminology sakura lilyterm weston-terminal
        roxterm termit xvt rxvt mrxvt urxvt */
      }
      if(strlen(lTerminalName))
      {
        return lTerminalName;
      }
      else
      {
        return NULL;
      }
    }

    static char* dialogName(void)
    {
      char* lDialogName;
      lDialogName = dialogNameOnly();
      if(strlen(lDialogName) && (isTerminalRunning() || terminalName()))
      {
        return lDialogName;
      }
      else
      {
        return NULL;
      }
    }

    static int whiptailPresent(void)
    {
      int lWhiptailPresent;
      lWhiptailPresent = whiptailPresentOnly();
      if(lWhiptailPresent && (isTerminalRunning() || terminalName()))
      {
        return lWhiptailPresent;
      }
      else
      {
        return 0;
      }
    }

    static int graphicMode(void)
    {
      return !(tinyfd_forceConsole && (isTerminalRunning() || terminalName())) && (is_using_gfx_display() || (tfd_isDarwin() && (!getenv("SSH_TTY") || is_using_gfx_display())));
    }

    static int pactlPresent(void)
    {
      static int lPactlPresent = -1;
      if(lPactlPresent < 0)
      {
        lPactlPresent = detectPresence("pactl");
      }
      return lPactlPresent;
    }

    static int speakertestPresent(void)
    {
      static int lSpeakertestPresent = -1;
      if(lSpeakertestPresent < 0)
      {
        lSpeakertestPresent = detectPresence("speaker-test");
      }
      return lSpeakertestPresent;
    }

    static int playPresent()
    {
      static int lPlayPresent = -1;
      if(lPlayPresent < 0)
      {
        lPlayPresent = detectPresence("sox"); /*if sox is present, play is ready*/
      }
      return lPlayPresent;
    }

    static int beepexePresent()
    {
      static int lBeepexePresent = -1;
      if(lBeepexePresent < 0)
      {
        lBeepexePresent = detectPresence("beep.exe");
      }
      return lBeepexePresent;
    }

    static int beepPresent(void)
    {
      static int lBeepPresent = -1;
      if(lBeepPresent < 0)
      {
        lBeepPresent = detectPresence("beep");
      }
      return lBeepPresent;
    }

    static int xmessagePresent(void)
    {
      static int lXmessagePresent = -1;
      if(lXmessagePresent < 0)
      {
        lXmessagePresent = detectPresence("xmessage"); /*if not tty,not on osxpath*/
      }
      return lXmessagePresent && graphicMode();
    }

    static int gxmessagePresent(void)
    {
      static int lGxmessagePresent = -1;
      if(lGxmessagePresent < 0)
      {
        lGxmessagePresent = detectPresence("gxmessage");
      }
      return lGxmessagePresent && graphicMode();
    }

    static int gmessagePresent(void)
    {
      static int lGmessagePresent = -1;
      if(lGmessagePresent < 0)
      {
        lGmessagePresent = detectPresence("gmessage");
      }
      return lGmessagePresent && graphicMode();
    }

    static int notifysendPresent(void)
    {
      static int lNotifysendPresent = -1;
      if(lNotifysendPresent < 0)
      {
        lNotifysendPresent = detectPresence("notify-send");
      }
      return lNotifysendPresent && graphicMode();
    }

    static int perlPresent(void)
    {
      static int lPerlPresent = -1;
      char dirname[MAX_PATH];
      FILE* lIn;

      if(lPerlPresent < 0)
      {
        lPerlPresent = detectPresence("perl");
        if(lPerlPresent)
        {
          lIn = popen("perl -MNet::DBus -e \"Net::DBus->session->get_service('org.freedesktop.Notifications')\" 2>&1", "r");
          if(fgets(dirname, sizeof(dirname), lIn) == NULL)
          {
            lPerlPresent = 2;
          }
          pclose(lIn);
          if(tinyfd_verbose)
            printf("perl-dbus %d\n", lPerlPresent);
        }
      }
      return graphicMode() ? lPerlPresent : 0;
    }

    static int afplayPresent(void)
    {
      static int lAfplayPresent = -1;
      char dirname[MAX_PATH];
      FILE* lIn;

      if(lAfplayPresent < 0)
      {
        lAfplayPresent = detectPresence("afplay");
        if(lAfplayPresent)
        {
          lIn = popen("test -e /System/Library/Sounds/Ping.aiff || echo Ping", "r");
          if(fgets(dirname, sizeof(dirname), lIn) == NULL)
          {
            lAfplayPresent = 2;
          }
          pclose(lIn);
          if(tinyfd_verbose)
            printf("afplay %d\n", lAfplayPresent);
        }
      }
      return graphicMode() ? lAfplayPresent : 0;
    }

    static int xdialogPresent(void)
    {
      static int lXdialogPresent = -1;
      if(lXdialogPresent < 0)
      {
        lXdialogPresent = detectPresence("Xdialog");
      }
      return lXdialogPresent && graphicMode();
    }

    static int gdialogPresent(void)
    {
      static int lGdialoglPresent = -1;
      if(lGdialoglPresent < 0)
      {
        lGdialoglPresent = detectPresence("gdialog");
      }
      return lGdialoglPresent && graphicMode();
    }

    static int osascriptPresent(void)
    {
      static int lOsascriptPresent = -1;
      if(lOsascriptPresent < 0)
      {
        gWarningDisplayed |= !!getenv("SSH_TTY");
        lOsascriptPresent = detectPresence("osascript");
      }
      return lOsascriptPresent && graphicMode() && !getenv("SSH_TTY");
    }

    static int dunstifyPresent(void)
    {
      static int lDunstifyPresent = -1;
      static char dirname[MAX_PATH];
      FILE* lIn;
      char* lTmp;

      if(lDunstifyPresent < 0)
      {
        lDunstifyPresent = detectPresence("dunstify");
        if(lDunstifyPresent)
        {
          lIn  = popen("dunstify -s", "r");
          lTmp = fgets(dirname, sizeof(dirname), lIn);
          pclose(lIn);
          /* printf("lTmp:%s\n", lTmp); */
          lDunstifyPresent = strstr(lTmp, "name:dunst\n") ? 1 : 0;
          if(tinyfd_verbose)
            printf("lDunstifyPresent %d\n", lDunstifyPresent);
        }
      }
      return lDunstifyPresent && graphicMode();
    }

    static int dunstPresent(void)
    {
      static int lDunstPresent = -1;
      static char dirname[MAX_PATH];
      FILE* lIn;
      char* lTmp;

      if(lDunstPresent < 0)
      {
        lDunstPresent = detectPresence("dunst");
        if(lDunstPresent)
        {
          lIn  = popen("ps -e | grep dunst | grep -v grep", "r"); /* add "| wc -l" to receive the number of lines */
          lTmp = fgets(dirname, sizeof(dirname), lIn);
          pclose(lIn);
          /* if ( lTmp ) printf("lTmp:%s\n", lTmp); */
          if(lTmp)
            lDunstPresent = 1;
          else
            lDunstPresent = 0;
          if(tinyfd_verbose)
            printf("lDunstPresent %d\n", lDunstPresent);
        }
      }
      return lDunstPresent && graphicMode();
    }

    int tfd_qarmaPresent(void)
    {
      static int lQarmaPresent = -1;
      if(lQarmaPresent < 0)
      {
        lQarmaPresent = detectPresence("qarma");
      }
      return lQarmaPresent && graphicMode();
    }

    int tfd_matedialogPresent(void)
    {
      static int lMatedialogPresent = -1;
      if(lMatedialogPresent < 0)
      {
        lMatedialogPresent = detectPresence("matedialog");
      }
      return lMatedialogPresent && graphicMode();
    }

    int tfd_shellementaryPresent(void)
    {
      static int lShellementaryPresent = -1;
      if(lShellementaryPresent < 0)
      {
        lShellementaryPresent = 0; /*detectPresence("shellementary"); shellementary is not ready yet */
      }
      return lShellementaryPresent && graphicMode();
    }

    int tfd_xpropPresent(void)
    {
      static int lXpropPresent = -1;
      if(lXpropPresent < 0)
      {
        lXpropPresent = detectPresence("xprop");
      }
      return lXpropPresent && graphicMode();
    }

    int tfd_zenityPresent(void)
    {
      static int lZenityPresent = -1;
      if(lZenityPresent < 0)
      {
        lZenityPresent = detectPresence("zenity");
      }
      return lZenityPresent && graphicMode();
    }

    int tfd_yadPresent(void)
    {
      static int lYadPresent = -1;
      if(lYadPresent < 0)
      {
        lYadPresent = detectPresence("yad");
      }
      return lYadPresent && graphicMode();
    }

    int tfd_zenity3Present(void)
    {
      static int lZenity3Present = -1;
      char dirname[MAX_PATH];
      FILE* lIn;
      int lIntTmp;

      if(lZenity3Present < 0)
      {
        lZenity3Present = 0;
        if(tfd_zenityPresent())
        {
          lIn = popen("zenity --version", "r");
          if(fgets(dirname, sizeof(dirname), lIn) != NULL)
          {
            if(atoi(dirname) >= 3)
            {
              lZenity3Present = 3;
              lIntTmp         = atoi(strtok(dirname, ".") + 2);
              if(lIntTmp >= 18)
              {
                lZenity3Present = 5;
              }
              else if(lIntTmp >= 10)
              {
                lZenity3Present = 4;
              }
            }
            else if((atoi(dirname) == 2) && (atoi(strtok(dirname, ".") + 2) >= 32))
            {
              lZenity3Present = 2;
            }
            if(tinyfd_verbose)
              printf("zenity type %d\n", lZenity3Present);
          }
          pclose(lIn);
        }
      }
      return graphicMode() ? lZenity3Present : 0;
    }

    int tfd_kdialogPresent(void)
    {
      static int lKdialogPresent = -1;
      char dirname[MAX_PATH];
      FILE* lIn;
      char* lDesktop;

      if(lKdialogPresent < 0)
      {
        if(tfd_zenityPresent())
        {
          lDesktop = getenv("XDG_SESSION_DESKTOP");
          if(!lDesktop || (strcmp(lDesktop, "KDE") && strcmp(lDesktop, "lxqt")))
          {
            lKdialogPresent = 0;
            return lKdialogPresent;
          }
        }

        lKdialogPresent = detectPresence("kdialog");
        if(lKdialogPresent && !getenv("SSH_TTY"))
        {
          lIn = popen("kdialog --attach 2>&1", "r");
          if(fgets(dirname, sizeof(dirname), lIn) != NULL)
          {
            if(!strstr("Unknown", dirname))
            {
              lKdialogPresent = 2;
              if(tinyfd_verbose)
                printf("kdialog-attach %d\n", lKdialogPresent);
            }
          }
          pclose(lIn);

          if(lKdialogPresent == 2)
          {
            lKdialogPresent = 1;
            lIn             = popen("kdialog --passiveBaloonTip 2>&1", "r");
            if(fgets(dirname, sizeof(dirname), lIn) != NULL)
            {
              if(!strstr("Unknown", dirname))
              {
                lKdialogPresent = 2;
                if(tinyfd_verbose)
                  printf("kdialog-BaloonTip %d\n", lKdialogPresent);
              }
            }
            pclose(lIn);
          }
        }
      }
      return graphicMode() ? lKdialogPresent : 0;
    }

    static int osx9orBetter(void)
    {
      static int lOsx9orBetter = -1;
      char dirname[MAX_PATH];
      FILE* lIn;
      int V, v;

      if(lOsx9orBetter < 0)
      {
        lOsx9orBetter = 0;
        lIn           = popen("osascript -e 'set osver to system version of (system info)'", "r");
        V             = 0;
        if((fgets(dirname, sizeof(dirname), lIn) != NULL) && (2 == sscanf(dirname, "%d.%d", &V, &v)))
        {
          V = V * 100 + v;
          if(V >= 1009)
          {
            lOsx9orBetter = 1;
          }
        }
        pclose(lIn);
        if(tinyfd_verbose)
          printf("Osx10 = %d, %d = %s\n", lOsx9orBetter, V, dirname);
      }
      return lOsx9orBetter;
    }

    static int python3Present(void)
    {
      static int lPython3Present = -1;
      int i;

      if(lPython3Present < 0)
      {
        lPython3Present = 0;
        strcpy(gPython3Name, "python3");
        if(detectPresence(gPython3Name))
          lPython3Present = 1;
        else
        {
          for(i = 9; i >= 0; i--)
          {
            sprintf(gPython3Name, "python3.%d", i);
            if(detectPresence(gPython3Name))
            {
              lPython3Present = 1;
              break;
            }
          }
        }
        if(tinyfd_verbose)
          printf("lPython3Present %d\n", lPython3Present);
        if(tinyfd_verbose)
          printf("gPython3Name %s\n", gPython3Name);
      }
      return lPython3Present;
    }

    static int python2Present(void)
    {
      static int lPython2Present = -1;

      if(lPython2Present < 0)
      {
        lPython2Present = 0;
        strcpy(gPython2Name, "python2");
        if(detectPresence(gPython2Name))
          lPython2Present = 1;
        /*else
        {
          for ( i = 9 ; i >= 0 ; i -- )
          {
            sprintf( gPython2Name , "python2.%d" , i ) ;
            if ( detectPresence(gPython2Name) )
            {
              lPython2Present = 1;
              break;
            }
          }
        }*/
        if(tinyfd_verbose)
          printf("lPython2Present %d\n", lPython2Present);
        if(tinyfd_verbose)
          printf("gPython2Name %s\n", gPython2Name);
      }
      return lPython2Present;
    }

    static int tkinter3Present(void)
    {
      static int lTkinter3Present = -1;
      char lPythonCommand[256];
      char lPythonParams[128] = "-S -c \"try:\n\timport tkinter;\nexcept:\n\tprint(0);\"";

      if(lTkinter3Present < 0)
      {
        lTkinter3Present = 0;
        if(python3Present())
        {
          sprintf(lPythonCommand, "%s %s", gPython3Name, lPythonParams);
          lTkinter3Present = tryCommand(lPythonCommand);
        }
        if(tinyfd_verbose)
          printf("lTkinter3Present %d\n", lTkinter3Present);
      }
      return lTkinter3Present && graphicMode() && !(tfd_isDarwin() && getenv("SSH_TTY"));
    }

    static int tkinter2Present(void)
    {
      static int lTkinter2Present = -1;
      char lPythonCommand[256];
      char lPythonParams[128] = "-S -c \"try:\n\timport Tkinter;\nexcept:\n\tprint 0;\"";

      if(lTkinter2Present < 0)
      {
        lTkinter2Present = 0;
        if(python2Present())
        {
          sprintf(lPythonCommand, "%s %s", gPython2Name, lPythonParams);
          lTkinter2Present = tryCommand(lPythonCommand);
        }
        if(tinyfd_verbose)
          printf("lTkinter2Present %d graphicMode %d \n", lTkinter2Present, graphicMode());
      }
      return lTkinter2Present && graphicMode() && !(tfd_isDarwin() && getenv("SSH_TTY"));
    }

    static int pythonDbusPresent(void)
    {
      static int lPythonDbusPresent = -1;
      char lPythonCommand[384];
      char lPythonParams[256] = "-c \"try:\n\timport dbus;bus=dbus.SessionBus();\
notif=bus.get_object('org.freedesktop.Notifications','/org/freedesktop/Notifications');\
notify=dbus.Interface(notif,'org.freedesktop.Notifications');\nexcept:\n\tprint(0);\"";

      if(lPythonDbusPresent < 0)
      {
        lPythonDbusPresent = 0;
        if(python2Present())
        {
          strcpy(gPythonName, gPython2Name);
          sprintf(lPythonCommand, "%s %s", gPythonName, lPythonParams);
          lPythonDbusPresent = tryCommand(lPythonCommand);
        }

        if(!lPythonDbusPresent && python3Present())
        {
          strcpy(gPythonName, gPython3Name);
          sprintf(lPythonCommand, "%s %s", gPythonName, lPythonParams);
          lPythonDbusPresent = tryCommand(lPythonCommand);
        }

        if(tinyfd_verbose)
          printf("lPythonDbusPresent %d\n", lPythonDbusPresent);
        if(tinyfd_verbose)
          printf("gPythonName %s\n", gPythonName);
      }
      return lPythonDbusPresent && graphicMode() && !(tfd_isDarwin() && getenv("SSH_TTY"));
    }

    static void sigHandler(int signum)
    {
      FILE* lIn;
      if((lIn = popen("pactl unload-module module-sine", "r")))
      {
        pclose(lIn);
      }
      if(tinyfd_verbose)
        printf("tinyfiledialogs caught signal %d\n", signum);
    }

    void tinyfd_beep(void)
    {
      char lDialogString[256];
      FILE* lIn;

      if(osascriptPresent())
      {
        if(afplayPresent() >= 2)
        {
          strcpy(lDialogString, "afplay /System/Library/Sounds/Ping.aiff");
        }
        else
        {
          strcpy(lDialogString, "osascript -e 'tell application \"System Events\" to beep'");
        }
      }
      else if(pactlPresent())
      {
        signal(SIGINT, sigHandler);
        /*strcpy( lDialogString , "pactl load-module module-sine frequency=440;sleep .3;pactl unload-module module-sine" ) ;*/
        strcpy(lDialogString, "thnum=$(pactl load-module module-sine frequency=440);sleep .3;pactl unload-module $thnum");
      }
      else if(speakertestPresent())
      {
        /*strcpy( lDialogString , "timeout -k .3 .3 speaker-test --frequency 440 --test sine > /dev/tty" ) ;*/
        strcpy(lDialogString, "( speaker-test -t sine -f 440 > /dev/tty )& pid=$!;sleep .5; kill -9 $pid"); /*.3 was too short for mac g3*/
      }
      else if(beepexePresent())
      {
        strcpy(lDialogString, "beep.exe 440 300");
      }
      else if(playPresent()) /* play is part of sox */
      {
        strcpy(lDialogString, "play -q -n synth .3 sine 440");
      }
      else if(beepPresent())
      {
        strcpy(lDialogString, "beep -f 440 -l 300");
      }
      else
      {
        strcpy(lDialogString, "printf '\a' > /dev/tty");
      }

      if(tinyfd_verbose)
        printf("lDialogString: %s\n", lDialogString);

      if((lIn = popen(lDialogString, "r")))
      {
        pclose(lIn);
      }

      if(pactlPresent())
      {
        signal(SIGINT, SIG_DFL);
      }
    }

    int tinyfd_msg_box(const char* title,     /* NULL or "" */
                       const char* msg,       /* NULL or ""  may contain \n and \t */
                       DialogType dialogType, /* "ok" "okcancel" "yesno" "yesnocancel" */
                       IconType iconType,     /* "info" "warning" "error" "question" */
                       int defaultButton)     /* 0 for cancel/no , 1 for ok/yes , 2 for no in yesnocancel */
    {
      char dirname[MAX_PATH];
      char* lDialogString = NULL;
      char* lpDialogString;
      FILE* lIn;
      int lWasGraphicDialog = 0;
      int lWasXterm         = 0;
      int lResult;
      char lChar;
      struct termios infoOri;
      struct termios info;
      size_t lTitleLen;
      size_t lMessageLen;

      dirname[0] = '\0';

      if(tfd_quoteDetected(title))
        return msg_box("INVALID TITLE WITH QUOTES", msg, dialogType, iconType, defaultButton);
      if(tfd_quoteDetected(msg))
        return msg_box(title, "INVALID MESSAGE WITH QUOTES", dialogType, iconType, defaultButton);

      lTitleLen   = title ? strlen(title) : 0;
      lMessageLen = msg ? strlen(msg) : 0;
      if(!title || strcmp(title, "tinyfd_query"))
      {
        lDialogString = (char*)malloc(MAX_PATH + lTitleLen + lMessageLen);
      }

      if(osascriptPresent())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "applescript");
          return 1;
        }

        strcpy(lDialogString, "osascript ");
        if(!osx9orBetter())
          strcat(lDialogString, " -e 'tell application \"System Events\"' -e 'Activate'");
        strcat(lDialogString, " -e 'try' -e 'set {vButton} to {button returned} of ( display dialog \"");
        if(msg && strlen(msg))
        {
          strcat(lDialogString, msg);
        }
        strcat(lDialogString, "\" ");
        if(title && strlen(title))
        {
          strcat(lDialogString, "with title \"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\" ");
        }
        strcat(lDialogString, "with icon ");
        if(iconType && !strcmp("error", iconType))
        {
          strcat(lDialogString, "stop ");
        }
        else if(iconType && !strcmp("warning", iconType))
        {
          strcat(lDialogString, "caution ");
        }
        else /* question or info */
        {
          strcat(lDialogString, "note ");
        }
        if(dialogType && !strcmp("okcancel", dialogType))
        {
          if(!defaultButton)
          {
            strcat(lDialogString, "default button \"Cancel\" ");
          }
        }
        else if(dialogType && !strcmp("yesno", dialogType))
        {
          strcat(lDialogString, "buttons {\"No\", \"Yes\"} ");
          if(defaultButton)
          {
            strcat(lDialogString, "default button \"Yes\" ");
          }
          else
          {
            strcat(lDialogString, "default button \"No\" ");
          }
          strcat(lDialogString, "cancel button \"No\"");
        }
        else if(dialogType && !strcmp("yesnocancel", dialogType))
        {
          strcat(lDialogString, "buttons {\"No\", \"Yes\", \"Cancel\"} ");
          switch(defaultButton)
          {
            case 1: strcat(lDialogString, "default button \"Yes\" "); break;
            case 2: strcat(lDialogString, "default button \"No\" "); break;
            case 0: strcat(lDialogString, "default button \"Cancel\" "); break;
          }
          strcat(lDialogString, "cancel button \"Cancel\"");
        }
        else
        {
          strcat(lDialogString, "buttons {\"OK\"} ");
          strcat(lDialogString, "default button \"OK\" ");
        }
        strcat(lDialogString, ")' ");

        strcat(lDialogString, "-e 'if vButton is \"Yes\" then' -e 'return 1'\
 -e 'else if vButton is \"OK\" then' -e 'return 1'\
 -e 'else if vButton is \"No\" then' -e 'return 2'\
 -e 'else' -e 'return 0' -e 'end if' ");

        strcat(lDialogString, "-e 'on error number -128' ");
        strcat(lDialogString, "-e '0' ");

        strcat(lDialogString, "-e 'end try'");
        if(!osx9orBetter())
          strcat(lDialogString, " -e 'end tell'");
      }
      else if(tfd_kdialogPresent())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "kdialog");
          return 1;
        }

        strcpy(lDialogString, "kdialog");
        if((tfd_kdialogPresent() == 2) && tfd_xpropPresent())
        {
          strcat(lDialogString, " --attach=$(xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)"); /* contribution: Paul Rouget */
        }

        strcat(lDialogString, " --");
        if(dialogType && (!strcmp("okcancel", dialogType) || !strcmp("yesno", dialogType) || !strcmp("yesnocancel", dialogType)))
        {
          if(iconType && (!strcmp("warning", iconType) || !strcmp("error", iconType)))
          {
            strcat(lDialogString, "warning");
          }
          if(!strcmp("yesnocancel", dialogType))
          {
            strcat(lDialogString, "yesnocancel");
          }
          else
          {
            strcat(lDialogString, "yesno");
          }
        }
        else if(iconType && !strcmp("error", iconType))
        {
          strcat(lDialogString, "error");
        }
        else if(iconType && !strcmp("warning", iconType))
        {
          strcat(lDialogString, "sorry");
        }
        else
        {
          strcat(lDialogString, "msgbox");
        }
        strcat(lDialogString, " \"");
        if(msg)
        {
          strcat(lDialogString, msg);
        }
        strcat(lDialogString, "\"");
        if(dialogType && !strcmp("okcancel", dialogType))
        {
          strcat(lDialogString, " --yes-label Ok --no-label Cancel");
        }
        if(title && strlen(title))
        {
          strcat(lDialogString, " --title \"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\"");
        }

        if(!strcmp("yesnocancel", dialogType))
        {
          strcat(lDialogString, "; x=$? ;if [ $x = 0 ] ;then echo 1;elif [ $x = 1 ] ;then echo 2;else echo 0;fi");
        }
        else
        {
          strcat(lDialogString, ";if [ $? = 0 ];then echo 1;else echo 0;fi");
        }
      }
      else if(tfd_zenityPresent() || tfd_matedialogPresent() || tfd_shellementaryPresent() || tfd_qarmaPresent())
      {
        if(tfd_zenityPresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "zenity");
            return 1;
          }
          strcpy(lDialogString, "szAnswer=$(zenity");
          if((tfd_zenity3Present() >= 4) && !getenv("SSH_TTY") && tfd_xpropPresent())
          {
            strcat(lDialogString, " --attach=$(sleep .01;xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)"); /* contribution: Paul Rouget */
          }
        }
        else if(tfd_matedialogPresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "matedialog");
            return 1;
          }
          strcpy(lDialogString, "szAnswer=$(matedialog");
        }
        else if(tfd_shellementaryPresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "shellementary");
            return 1;
          }
          strcpy(lDialogString, "szAnswer=$(shellementary");
        }
        else
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "qarma");
            return 1;
          }
          strcpy(lDialogString, "szAnswer=$(qarma");
          if(!getenv("SSH_TTY") && tfd_xpropPresent())
          {
            strcat(lDialogString, " --attach=$(xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)"); /* contribution: Paul Rouget */
          }
        }
        strcat(lDialogString, " --");

        if(dialogType && !strcmp("okcancel", dialogType))
        {
          strcat(lDialogString, "question --ok-label=Ok --cancel-label=Cancel");
        }
        else if(dialogType && !strcmp("yesno", dialogType))
        {
          strcat(lDialogString, "question");
        }
        else if(dialogType && !strcmp("yesnocancel", dialogType))
        {
          strcat(lDialogString, "list --column \"\" --hide-header \"Yes\" \"No\"");
        }
        else if(iconType && !strcmp("error", iconType))
        {
          strcat(lDialogString, "error");
        }
        else if(iconType && !strcmp("warning", iconType))
        {
          strcat(lDialogString, "warning");
        }
        else
        {
          strcat(lDialogString, "info");
        }

        strcat(lDialogString, " --title=\"");
        if(title && strlen(title))
          strcat(lDialogString, title);
        strcat(lDialogString, "\"");

        if(strcmp("yesnocancel", dialogType))
          strcat(lDialogString, " --no-wrap");

        strcat(lDialogString, " --text=\"");
        if(msg && strlen(msg))
          strcat(lDialogString, msg);
        strcat(lDialogString, "\"");

        if((tfd_zenity3Present() >= 3) || (!tfd_zenityPresent() && (tfd_shellementaryPresent() || tfd_qarmaPresent())))
        {
          strcat(lDialogString, " --icon-name=dialog-");
          if(iconType && (!strcmp("question", iconType) || !strcmp("error", iconType) || !strcmp("warning", iconType)))
          {
            strcat(lDialogString, iconType);
          }
          else
          {
            strcat(lDialogString, "information");
          }
        }

        if(tinyfd_silent)
          strcat(lDialogString, " 2>/dev/null ");

        if(!strcmp("yesnocancel", dialogType))
        {
          strcat(lDialogString, ");if [ $? = 1 ];then echo 0;elif [ $szAnswer = \"No\" ];then echo 2;else echo 1;fi");
        }
        else
        {
          strcat(lDialogString, ");if [ $? = 0 ];then echo 1;else echo 0;fi");
        }
      }

      else if(tfd_yadPresent())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "yad");
          return 1;
        }
        strcpy(lDialogString, "szAnswer=$(yad --");
        if(dialogType && !strcmp("ok", dialogType))
        {
          strcat(lDialogString, "button=Ok:1");
        }
        else if(dialogType && !strcmp("okcancel", dialogType))
        {
          strcat(lDialogString, "button=Ok:1 --button=Cancel:0");
        }
        else if(dialogType && !strcmp("yesno", dialogType))
        {
          strcat(lDialogString, "button=Yes:1 --button=No:0");
        }
        else if(dialogType && !strcmp("yesnocancel", dialogType))
        {
          strcat(lDialogString, "button=Yes:1 --button=No:2 --button=Cancel:0");
        }
        else if(iconType && !strcmp("error", iconType))
        {
          strcat(lDialogString, "error");
        }
        else if(iconType && !strcmp("warning", iconType))
        {
          strcat(lDialogString, "warning");
        }
        else
        {
          strcat(lDialogString, "info");
        }
        if(title && strlen(title))
        {
          strcat(lDialogString, " --title=\"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\"");
        }
        if(msg && strlen(msg))
        {
          strcat(lDialogString, " --text=\"");
          strcat(lDialogString, msg);
          strcat(lDialogString, "\"");
        }

        strcat(lDialogString, " --image=dialog-");
        if(iconType && (!strcmp("question", iconType) || !strcmp("error", iconType) || !strcmp("warning", iconType)))
        {
          strcat(lDialogString, iconType);
        }
        else
        {
          strcat(lDialogString, "information");
        }

        if(tinyfd_silent)
          strcat(lDialogString, " 2>/dev/null ");
        strcat(lDialogString, ");echo $?");
      }

      else if(!gxmessagePresent() && !gmessagePresent() && !gdialogPresent() && !xdialogPresent() && tkinter3Present())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "python3-tkinter");
          return 1;
        }

        strcpy(lDialogString, gPython3Name);
        strcat(lDialogString, " -S -c \"import tkinter;from tkinter import messagebox;root=tkinter.Tk();root.withdraw();");

        strcat(lDialogString, "res=messagebox.");
        if(dialogType && !strcmp("okcancel", dialogType))
        {
          strcat(lDialogString, "askokcancel(");
          if(defaultButton)
          {
            strcat(lDialogString, "default=messagebox.OK,");
          }
          else
          {
            strcat(lDialogString, "default=messagebox.CANCEL,");
          }
        }
        else if(dialogType && !strcmp("yesno", dialogType))
        {
          strcat(lDialogString, "askyesno(");
          if(defaultButton)
          {
            strcat(lDialogString, "default=messagebox.YES,");
          }
          else
          {
            strcat(lDialogString, "default=messagebox.NO,");
          }
        }
        else if(dialogType && !strcmp("yesnocancel", dialogType))
        {
          strcat(lDialogString, "askyesnocancel(");
          switch(defaultButton)
          {
            case 1: strcat(lDialogString, "default=messagebox.YES,"); break;
            case 2: strcat(lDialogString, "default=messagebox.NO,"); break;
            case 0: strcat(lDialogString, "default=messagebox.CANCEL,"); break;
          }
        }
        else
        {
          strcat(lDialogString, "showinfo(");
        }

        strcat(lDialogString, "icon='");
        if(iconType && (!strcmp("question", iconType) || !strcmp("error", iconType) || !strcmp("warning", iconType)))
        {
          strcat(lDialogString, iconType);
        }
        else
        {
          strcat(lDialogString, "info");
        }

        strcat(lDialogString, "',");
        if(title && strlen(title))
        {
          strcat(lDialogString, "title='");
          strcat(lDialogString, title);
          strcat(lDialogString, "',");
        }
        if(msg && strlen(msg))
        {
          strcat(lDialogString, "message='");
          lpDialogString = lDialogString + strlen(lDialogString);
          tfd_replaceSubStr(msg, "\n", "\\n", lpDialogString);
          strcat(lDialogString, "'");
        }

        if(dialogType && !strcmp("yesnocancel", dialogType))
        {
          strcat(lDialogString, ");\n\
if res is None :\n\tprint(0)\n\
elif res is False :\n\tprint(2)\n\
else :\n\tprint (1)\n\"");
        }
        else
        {
          strcat(lDialogString, ");\n\
if res is False :\n\tprint(0)\n\
else :\n\tprint(1)\n\"");
        }
      }
      else if(!gxmessagePresent() && !gmessagePresent() && !gdialogPresent() && !xdialogPresent() && tkinter2Present())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "python2-tkinter");
          return 1;
        }
        strcpy(lDialogString, "export PYTHONIOENCODING=utf-8;");
        strcat(lDialogString, gPython2Name);
        if(!isTerminalRunning() && tfd_isDarwin())
        {
          strcat(lDialogString, " -i"); /* for osx without console */
        }

        strcat(lDialogString, " -S -c \"import Tkinter,tkMessageBox;root=Tkinter.Tk();root.withdraw();");

        if(tfd_isDarwin())
        {
          strcat(lDialogString, "import os;os.system('''/usr/bin/osascript -e 'tell app \\\"Finder\\\" to set \
frontmost of process \\\"Python\\\" to true' ''');");
        }

        strcat(lDialogString, "res=tkMessageBox.");
        if(dialogType && !strcmp("okcancel", dialogType))
        {
          strcat(lDialogString, "askokcancel(");
          if(defaultButton)
          {
            strcat(lDialogString, "default=tkMessageBox.OK,");
          }
          else
          {
            strcat(lDialogString, "default=tkMessageBox.CANCEL,");
          }
        }
        else if(dialogType && !strcmp("yesno", dialogType))
        {
          strcat(lDialogString, "askyesno(");
          if(defaultButton)
          {
            strcat(lDialogString, "default=tkMessageBox.YES,");
          }
          else
          {
            strcat(lDialogString, "default=tkMessageBox.NO,");
          }
        }
        else if(dialogType && !strcmp("yesnocancel", dialogType))
        {
          strcat(lDialogString, "askyesnocancel(");
          switch(defaultButton)
          {
            case 1: strcat(lDialogString, "default=tkMessageBox.YES,"); break;
            case 2: strcat(lDialogString, "default=tkMessageBox.NO,"); break;
            case 0: strcat(lDialogString, "default=tkMessageBox.CANCEL,"); break;
          }
        }
        else
        {
          strcat(lDialogString, "showinfo(");
        }

        strcat(lDialogString, "icon='");
        if(iconType && (!strcmp("question", iconType) || !strcmp("error", iconType) || !strcmp("warning", iconType)))
        {
          strcat(lDialogString, iconType);
        }
        else
        {
          strcat(lDialogString, "info");
        }

        strcat(lDialogString, "',");
        if(title && strlen(title))
        {
          strcat(lDialogString, "title='");
          strcat(lDialogString, title);
          strcat(lDialogString, "',");
        }
        if(msg && strlen(msg))
        {
          strcat(lDialogString, "message='");
          lpDialogString = lDialogString + strlen(lDialogString);
          tfd_replaceSubStr(msg, "\n", "\\n", lpDialogString);
          strcat(lDialogString, "'");
        }

        if(dialogType && !strcmp("yesnocancel", dialogType))
        {
          strcat(lDialogString, ");\n\
if res is None :\n\tprint 0\n\
elif res is False :\n\tprint 2\n\
else :\n\tprint 1\n\"");
        }
        else
        {
          strcat(lDialogString, ");\n\
if res is False :\n\tprint 0\n\
else :\n\tprint 1\n\"");
        }
      }
      else if(gxmessagePresent() || gmessagePresent() || (!gdialogPresent() && !xdialogPresent() && xmessagePresent()))
      {
        if(gxmessagePresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "gxmessage");
            return 1;
          }
          strcpy(lDialogString, "gxmessage");
        }
        else if(gmessagePresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "gmessage");
            return 1;
          }
          strcpy(lDialogString, "gmessage");
        }
        else
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "xmessage");
            return 1;
          }
          strcpy(lDialogString, "xmessage");
        }

        if(dialogType && !strcmp("okcancel", dialogType))
        {
          strcat(lDialogString, " -buttons Ok:1,Cancel:0");
          switch(defaultButton)
          {
            case 1: strcat(lDialogString, " -default Ok"); break;
            case 0: strcat(lDialogString, " -default Cancel"); break;
          }
        }
        else if(dialogType && !strcmp("yesno", dialogType))
        {
          strcat(lDialogString, " -buttons Yes:1,No:0");
          switch(defaultButton)
          {
            case 1: strcat(lDialogString, " -default Yes"); break;
            case 0: strcat(lDialogString, " -default No"); break;
          }
        }
        else if(dialogType && !strcmp("yesnocancel", dialogType))
        {
          strcat(lDialogString, " -buttons Yes:1,No:2,Cancel:0");
          switch(defaultButton)
          {
            case 1: strcat(lDialogString, " -default Yes"); break;
            case 2: strcat(lDialogString, " -default No"); break;
            case 0: strcat(lDialogString, " -default Cancel"); break;
          }
        }
        else
        {
          strcat(lDialogString, " -buttons Ok:1");
          strcat(lDialogString, " -default Ok");
        }

        strcat(lDialogString, " -center \"");
        if(msg && strlen(msg))
        {
          strcat(lDialogString, msg);
        }
        strcat(lDialogString, "\"");
        if(title && strlen(title))
        {
          strcat(lDialogString, " -title  \"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\"");
        }
        strcat(lDialogString, " ; echo $? ");
      }
      else if(xdialogPresent() || gdialogPresent() || dialogName() || whiptailPresent())
      {
        if(gdialogPresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "gdialog");
            return 1;
          }
          lWasGraphicDialog = 1;
          strcpy(lDialogString, "(gdialog ");
        }
        else if(xdialogPresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "xdialog");
            return 1;
          }
          lWasGraphicDialog = 1;
          strcpy(lDialogString, "(Xdialog ");
        }
        else if(dialogName())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "dialog");
            return 0;
          }
          if(isTerminalRunning())
          {
            strcpy(lDialogString, "(dialog ");
          }
          else
          {
            lWasXterm = 1;
            strcpy(lDialogString, terminalName());
            strcat(lDialogString, "'(");
            strcat(lDialogString, dialogName());
            strcat(lDialogString, " ");
          }
        }
        else if(isTerminalRunning())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "whiptail");
            return 0;
          }
          strcpy(lDialogString, "(whiptail ");
        }
        else
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "whiptail");
            return 0;
          }
          lWasXterm = 1;
          strcpy(lDialogString, terminalName());
          strcat(lDialogString, "'(whiptail ");
        }

        if(title && strlen(title))
        {
          strcat(lDialogString, "--title \"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\" ");
        }

        if(!xdialogPresent() && !gdialogPresent())
        {
          if(dialogType && (!strcmp("okcancel", dialogType) || !strcmp("yesno", dialogType) || !strcmp("yesnocancel", dialogType)))
          {
            strcat(lDialogString, "--backtitle \"");
            strcat(lDialogString, "tab: move focus");
            strcat(lDialogString, "\" ");
          }
        }

        if(dialogType && !strcmp("okcancel", dialogType))
        {
          if(!defaultButton)
          {
            strcat(lDialogString, "--defaultno ");
          }
          strcat(lDialogString, "--yes-label \"Ok\" --no-label \"Cancel\" --yesno ");
        }
        else if(dialogType && !strcmp("yesno", dialogType))
        {
          if(!defaultButton)
          {
            strcat(lDialogString, "--defaultno ");
          }
          strcat(lDialogString, "--yesno ");
        }
        else if(dialogType && !strcmp("yesnocancel", dialogType))
        {
          if(!defaultButton)
          {
            strcat(lDialogString, "--defaultno ");
          }
          strcat(lDialogString, "--menu ");
        }
        else
        {
          strcat(lDialogString, "--msgbox ");
        }
        strcat(lDialogString, "\"");
        if(msg && strlen(msg))
        {
          strcat(lDialogString, msg);
        }
        strcat(lDialogString, "\" ");

        if(lWasGraphicDialog)
        {
          if(dialogType && !strcmp("yesnocancel", dialogType))
          {
            strcat(lDialogString, "0 60 0 Yes \"\" No \"\") 2>/tmp/tinyfd.txt;\
if [ $? = 0 ];then tinyfdBool=1;else tinyfdBool=0;fi;\
tinyfdRes=$(cat /tmp/tinyfd.txt);echo $tinyfdBool$tinyfdRes");
          }
          else
          {
            strcat(lDialogString, "10 60 ) 2>&1;if [ $? = 0 ];then echo 1;else echo 0;fi");
          }
        }
        else
        {
          if(dialogType && !strcmp("yesnocancel", dialogType))
          {
            strcat(lDialogString, "0 60 0 Yes \"\" No \"\" >/dev/tty ) 2>/tmp/tinyfd.txt;\
                if [ $? = 0 ];then tinyfdBool=1;else tinyfdBool=0;fi;\
                tinyfdRes=$(cat /tmp/tinyfd.txt);echo $tinyfdBool$tinyfdRes");

            if(lWasXterm)
            {
              strcat(lDialogString, " >/tmp/tinyfd0.txt';cat /tmp/tinyfd0.txt");
            }
            else
            {
              strcat(lDialogString, "; clear >/dev/tty");
            }
          }
          else
          {
            strcat(lDialogString, "10 60 >/dev/tty) 2>&1;if [ $? = 0 ];");
            if(lWasXterm)
            {
              strcat(lDialogString, "then\n\techo 1\nelse\n\techo 0\nfi >/tmp/tinyfd.txt';cat /tmp/tinyfd.txt;rm /tmp/tinyfd.txt");
            }
            else
            {
              strcat(lDialogString, "then echo 1;else echo 0;fi;clear >/dev/tty");
            }
          }
        }
      }
      else if(!isTerminalRunning() && terminalName())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "basicinput");
          return 0;
        }
        strcpy(lDialogString, terminalName());
        strcat(lDialogString, "'");
        if(!gWarningDisplayed && !tinyfd_forceConsole)
        {
          gWarningDisplayed = 1;
          strcat(lDialogString, "echo \"");
          strcat(lDialogString, gTitle);
          strcat(lDialogString, "\";");
          strcat(lDialogString, "echo \"");
          strcat(lDialogString, tinyfd_needs);
          strcat(lDialogString, "\";echo;echo;");
        }
        if(title && strlen(title))
        {
          strcat(lDialogString, "echo \"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\";echo;");
        }
        if(msg && strlen(msg))
        {
          strcat(lDialogString, "echo \"");
          strcat(lDialogString, msg);
          strcat(lDialogString, "\"; ");
        }
        if(dialogType && !strcmp("yesno", dialogType))
        {
          strcat(lDialogString, "echo -n \"y/n: \"; ");
          strcat(lDialogString, "stty sane -echo;");
          strcat(lDialogString, "answer=$( while ! head -c 1 | grep -i [ny];do true ;done);");
          strcat(lDialogString, "if echo \"$answer\" | grep -iq \"^y\";then\n");
          strcat(lDialogString, "\techo 1\nelse\n\techo 0\nfi");
        }
        else if(dialogType && !strcmp("okcancel", dialogType))
        {
          strcat(lDialogString, "echo -n \"[O]kay/[C]ancel: \"; ");
          strcat(lDialogString, "stty sane -echo;");
          strcat(lDialogString, "answer=$( while ! head -c 1 | grep -i [oc];do true ;done);");
          strcat(lDialogString, "if echo \"$answer\" | grep -iq \"^o\";then\n");
          strcat(lDialogString, "\techo 1\nelse\n\techo 0\nfi");
        }
        else if(dialogType && !strcmp("yesnocancel", dialogType))
        {
          strcat(lDialogString, "echo -n \"[Y]es/[N]o/[C]ancel: \"; ");
          strcat(lDialogString, "stty sane -echo;");
          strcat(lDialogString, "answer=$( while ! head -c 1 | grep -i [nyc];do true ;done);");
          strcat(lDialogString, "if echo \"$answer\" | grep -iq \"^y\";then\n\techo 1\n");
          strcat(lDialogString, "elif echo \"$answer\" | grep -iq \"^n\";then\n\techo 2\n");
          strcat(lDialogString, "else\n\techo 0\nfi");
        }
        else
        {
          strcat(lDialogString, "echo -n \"press enter to continue \"; ");
          strcat(lDialogString, "stty sane -echo;");
          strcat(lDialogString, "answer=$( while ! head -c 1;do true ;done);echo 1");
        }
        strcat(lDialogString, " >/tmp/tinyfd.txt';cat /tmp/tinyfd.txt;rm /tmp/tinyfd.txt");
      }
      else if(!isTerminalRunning() && pythonDbusPresent() && !strcmp("ok", dialogType))
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "python-dbus");
          return 1;
        }
        strcpy(lDialogString, gPythonName);
        strcat(lDialogString, " -c \"import dbus;bus=dbus.SessionBus();");
        strcat(lDialogString, "notif=bus.get_object('org.freedesktop.Notifications','/org/freedesktop/Notifications');");
        strcat(lDialogString, "notify=dbus.Interface(notif,'org.freedesktop.Notifications');");
        strcat(lDialogString, "notify.Notify('',0,'");
        if(iconType && strlen(iconType))
        {
          strcat(lDialogString, iconType);
        }
        strcat(lDialogString, "','");
        if(title && strlen(title))
        {
          strcat(lDialogString, title);
        }
        strcat(lDialogString, "','");
        if(msg && strlen(msg))
        {
          lpDialogString = lDialogString + strlen(lDialogString);
          tfd_replaceSubStr(msg, "\n", "\\n", lpDialogString);
        }
        strcat(lDialogString, "','','',5000)\"");
      }
      else if(!isTerminalRunning() && (perlPresent() >= 2) && !strcmp("ok", dialogType))
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "perl-dbus");
          return 1;
        }

        strcpy(lDialogString, "perl -e \"use Net::DBus;\
my \\$sessionBus = Net::DBus->session;\
my \\$notificationsService = \\$sessionBus->get_service('org.freedesktop.Notifications');\
my \\$notificationsObject = \\$notificationsService->get_object('/org/freedesktop/Notifications',\
'org.freedesktop.Notifications');");

        sprintf(lDialogString + strlen(lDialogString), "my \\$notificationId;\\$notificationId = \\$notificationsObject->Notify(shift, 0, '%s', '%s', '%s', [], {}, -1);\" ", iconType ? iconType : "", title ? title : "", msg ? msg : "");
      }
      else if(!isTerminalRunning() && notifysendPresent() && !strcmp("ok", dialogType))
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "notifysend");
          return 1;
        }
        strcpy(lDialogString, "notify-send");
        if(iconType && strlen(iconType))
        {
          strcat(lDialogString, " -i '");
          strcat(lDialogString, iconType);
          strcat(lDialogString, "'");
        }
        strcat(lDialogString, " \"");
        if(title && strlen(title))
        {
          strcat(lDialogString, title);
          strcat(lDialogString, " | ");
        }
        if(msg && strlen(msg))
        {
          tfd_replaceSubStr(msg, "\n\t", " |  ", dirname);
          tfd_replaceSubStr(msg, "\n", " | ", dirname);
          tfd_replaceSubStr(msg, "\t", "  ", dirname);
          strcat(lDialogString, dirname);
        }
        strcat(lDialogString, "\"");
      }
      else
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "basicinput");
          return 0;
        }
        if(!gWarningDisplayed && !tinyfd_forceConsole)
        {
          gWarningDisplayed = 1;
          printf("\n\n%s\n", gTitle);
          printf("%s\n\n", tinyfd_needs);
        }
        if(title && strlen(title))
        {
          printf("\n%s\n", title);
        }

        tcgetattr(0, &infoOri);
        tcgetattr(0, &info);
        info.c_lflag &= ~ICANON;
        info.c_cc[VMIN]  = 1;
        info.c_cc[VTIME] = 0;
        tcsetattr(0, TCSANOW, &info);
        if(dialogType && !strcmp("yesno", dialogType))
        {
          do
          {
            if(msg && strlen(msg))
            {
              printf("\n%s\n", msg);
            }
            printf("y/n: ");
            fflush(stdout);
            lChar = (char)tolower(getchar());
            printf("\n\n");
          } while(lChar != 'y' && lChar != 'n');
          lResult = lChar == 'y' ? 1 : 0;
        }
        else if(dialogType && !strcmp("okcancel", dialogType))
        {
          do
          {
            if(msg && strlen(msg))
            {
              printf("\n%s\n", msg);
            }
            printf("[O]kay/[C]ancel: ");
            fflush(stdout);
            lChar = (char)tolower(getchar());
            printf("\n\n");
          } while(lChar != 'o' && lChar != 'c');
          lResult = lChar == 'o' ? 1 : 0;
        }
        else if(dialogType && !strcmp("yesnocancel", dialogType))
        {
          do
          {
            if(msg && strlen(msg))
            {
              printf("\n%s\n", msg);
            }
            printf("[Y]es/[N]o/[C]ancel: ");
            fflush(stdout);
            lChar = (char)tolower(getchar());
            printf("\n\n");
          } while(lChar != 'y' && lChar != 'n' && lChar != 'c');
          lResult = (lChar == 'y') ? 1 : (lChar == 'n') ? 2 : 0;
        }
        else
        {
          if(msg && strlen(msg))
          {
            printf("\n%s\n\n", msg);
          }
          printf("press enter to continue ");
          fflush(stdout);
          getchar();
          printf("\n\n");
          lResult = 1;
        }
        tcsetattr(0, TCSANOW, &infoOri);
        free(lDialogString);
        return lResult;
      }

      if(tinyfd_verbose)
        printf("lDialogString: %s\n", lDialogString);

      if(!(lIn = popen(lDialogString, "r")))
      {
        free(lDialogString);
        return 0;
      }
      while(fgets(dirname, sizeof(dirname), lIn) != NULL)
      {
      }

      pclose(lIn);

      /* printf( "dirname: %s len: %lu \n" , dirname , strlen(dirname) ) ; */
      if(dirname[strlen(dirname) - 1] == '\n')
      {
        dirname[strlen(dirname) - 1] = '\0';
      }
      /* printf( "dirname1: %s len: %lu \n" , dirname , strlen(dirname) ) ; */

      if(dialogType && !strcmp("yesnocancel", dialogType))
      {
        if(dirname[0] == '1')
        {
          if(!strcmp(dirname + 1, "Yes"))
            strcpy(dirname, "1");
          else if(!strcmp(dirname + 1, "No"))
            strcpy(dirname, "2");
        }
      }
      /* printf( "dirname2: %s len: %lu \n" , dirname , strlen(dirname) ) ; */

      lResult = !strcmp(dirname, "2") ? 2 : !strcmp(dirname, "1") ? 1 : 0;

      /* printf( "lResult: %d\n" , lResult ) ; */
      free(lDialogString);
      return lResult;
    }

    /* return has only meaning for tinyfd_query */
    int tinyfd_notifyBaloonTip(const char* title, /* NULL or "" */
                               const char* msg,   /* NULL or ""  may contain \n and \t */
                               IconType iconType) /* "info" "warning" "error" */
    {
      char dirname[MAX_PATH];
      char* lDialogString = NULL;
      char* lpDialogString;
      FILE* lIn;
      size_t lTitleLen;
      size_t lMessageLen;

      if(tfd_quoteDetected(title))
        return notifyPopup("INVALID TITLE WITH QUOTES", msg, iconType);
      if(tfd_quoteDetected(msg))
        return notifyPopup(title, "INVALID MESSAGE WITH QUOTES", iconType);

      if(getenv("SSH_TTY") && !dunstifyPresent() && !dunstPresent())
      {
        return msg_box(title, msg, "ok", iconType, 0);
      }

      lTitleLen   = title ? strlen(title) : 0;
      lMessageLen = msg ? strlen(msg) : 0;
      if(!title || strcmp(title, "tinyfd_query"))
      {
        lDialogString = (char*)malloc(MAX_PATH + lTitleLen + lMessageLen);
      }

      if(getenv("SSH_TTY"))
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "dunst");
          return 1;
        }
        strcpy(lDialogString, "notify-send \"");
        if(title && strlen(title))
        {
          strcat(lDialogString, title);
          strcat(lDialogString, "\" \"");
        }
        if(msg && strlen(msg))
        {
          strcat(lDialogString, msg);
        }
        strcat(lDialogString, "\"");
      }
      else if(osascriptPresent())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "applescript");
          return 1;
        }

        strcpy(lDialogString, "osascript ");
        if(!osx9orBetter())
          strcat(lDialogString, " -e 'tell application \"System Events\"' -e 'Activate'");
        strcat(lDialogString, " -e 'try' -e 'display notification \"");
        if(msg && strlen(msg))
        {
          strcat(lDialogString, msg);
        }
        strcat(lDialogString, " \" ");
        if(title && strlen(title))
        {
          strcat(lDialogString, "with title \"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\" ");
        }

        strcat(lDialogString, "' -e 'end try'");
        if(!osx9orBetter())
          strcat(lDialogString, " -e 'end tell'");
      }
      else if(tfd_kdialogPresent())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "kdialog");
          return 1;
        }
        strcpy(lDialogString, "kdialog");

        if(iconType && strlen(iconType))
        {
          strcat(lDialogString, " --icon '");
          strcat(lDialogString, iconType);
          strcat(lDialogString, "'");
        }
        if(title && strlen(title))
        {
          strcat(lDialogString, " --title \"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\"");
        }

        strcat(lDialogString, " --passiveBaloonTip");
        strcat(lDialogString, " \"");
        if(msg)
        {
          strcat(lDialogString, msg);
        }
        strcat(lDialogString, " \" 5");
      }
      else if(tfd_yadPresent())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "yad");
          return 1;
        }
        strcpy(lDialogString, "yad --notification");

        if(iconType && strlen(iconType))
        {
          strcat(lDialogString, " --image=\"");
          strcat(lDialogString, iconType);
          strcat(lDialogString, "\"");
        }

        strcat(lDialogString, " --text=\"");
        if(title && strlen(title))
        {
          strcat(lDialogString, title);
          strcat(lDialogString, "\n");
        }
        if(msg && strlen(msg))
        {
          strcat(lDialogString, msg);
        }
        strcat(lDialogString, " \"");
      }
      else if(perlPresent() >= 2)
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "perl-dbus");
          return 1;
        }

        strcpy(lDialogString, "perl -e \"use Net::DBus;\
my \\$sessionBus = Net::DBus->session;\
my \\$notificationsService = \\$sessionBus->get_service('org.freedesktop.Notifications');\
my \\$notificationsObject = \\$notificationsService->get_object('/org/freedesktop/Notifications',\
'org.freedesktop.Notifications');");

        sprintf(lDialogString + strlen(lDialogString), "my \\$notificationId;\\$notificationId = \\$notificationsObject->Notify(shift, 0, '%s', '%s', '%s', [], {}, -1);\" ", iconType ? iconType : "", title ? title : "", msg ? msg : "");
      }
      else if(pythonDbusPresent())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "python-dbus");
          return 1;
        }
        strcpy(lDialogString, gPythonName);
        strcat(lDialogString, " -c \"import dbus;bus=dbus.SessionBus();");
        strcat(lDialogString, "notif=bus.get_object('org.freedesktop.Notifications','/org/freedesktop/Notifications');");
        strcat(lDialogString, "notify=dbus.Interface(notif,'org.freedesktop.Notifications');");
        strcat(lDialogString, "notify.Notify('',0,'");
        if(iconType && strlen(iconType))
        {
          strcat(lDialogString, iconType);
        }
        strcat(lDialogString, "','");
        if(title && strlen(title))
        {
          strcat(lDialogString, title);
        }
        strcat(lDialogString, "','");
        if(msg && strlen(msg))
        {
          lpDialogString = lDialogString + strlen(lDialogString);
          tfd_replaceSubStr(msg, "\n", "\\n", lpDialogString);
        }
        strcat(lDialogString, "','','',5000)\"");
      }
      else if(notifysendPresent())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "notifysend");
          return 1;
        }
        strcpy(lDialogString, "notify-send");
        if(iconType && strlen(iconType))
        {
          strcat(lDialogString, " -i '");
          strcat(lDialogString, iconType);
          strcat(lDialogString, "'");
        }
        strcat(lDialogString, " \"");
        if(title && strlen(title))
        {
          strcat(lDialogString, title);
          strcat(lDialogString, " | ");
        }
        if(msg && strlen(msg))
        {
          tfd_replaceSubStr(msg, "\n\t", " |  ", dirname);
          tfd_replaceSubStr(msg, "\n", " | ", dirname);
          tfd_replaceSubStr(msg, "\t", "  ", dirname);
          strcat(lDialogString, dirname);
        }
        strcat(lDialogString, "\"");
      }
      else if((tfd_zenity3Present() >= 5))
      {
        /* zenity 2.32 & 3.14 has the notification but with a bug: it doesnt return from it */
        /* zenity 3.8 show the notification as an alert ok cancel box */
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "zenity");
          return 1;
        }
        strcpy(lDialogString, "zenity --notification");

        if(iconType && strlen(iconType))
        {
          strcat(lDialogString, " --window-icon '");
          strcat(lDialogString, iconType);
          strcat(lDialogString, "'");
        }

        strcat(lDialogString, " --text \"");
        if(title && strlen(title))
        {
          strcat(lDialogString, title);
          strcat(lDialogString, "\n");
        }
        if(msg && strlen(msg))
        {
          strcat(lDialogString, msg);
        }
        strcat(lDialogString, " \"");
      }
      else
      {
        if(lDialogString)
          free(lDialogString);
        return msg_box(title, msg, "ok", iconType, 0);
      }

      if(tinyfd_verbose)
        printf("lDialogString: %s\n", lDialogString);

      if(!(lIn = popen(lDialogString, "r")))
      {
        free(lDialogString);
        return 0;
      }

      pclose(lIn);
      free(lDialogString);
      return 1;
    }

    /* returns NULL on cancel */
    char* tinyfd_inputBox(const char* title,         /* NULL or "" */
                          const char* msg,           /* NULL or "" (\n and \t have no effect) */
                          const char* aDefaultInput) /* "" , if NULL it's a passwordBox */
    {
      static char dirname[MAX_PATH];
      char* lDialogString = NULL;
      char* lpDialogString;
      FILE* lIn;
      int lResult;
      int lWasGdialog       = 0;
      int lWasGraphicDialog = 0;
      int lWasXterm         = 0;
      int lWasBasicXterm    = 0;
      struct termios oldt;
      struct termios newt;
      char* lEOF;
      size_t lTitleLen;
      size_t lMessageLen;

      if(!title && !msg && !aDefaultInput)
        return dirname; /* now I can fill dirname from outside */

      dirname[0] = '\0';

      if(tfd_quoteDetected(title))
        return inputBox("INVALID TITLE WITH QUOTES", msg, aDefaultInput);
      if(tfd_quoteDetected(msg))
        return inputBox(title, "INVALID MESSAGE WITH QUOTES", aDefaultInput);
      if(tfd_quoteDetected(aDefaultInput))
        return inputBox(title, msg, "INVALID DEFAULT_INPUT WITH QUOTES");

      lTitleLen   = title ? strlen(title) : 0;
      lMessageLen = msg ? strlen(msg) : 0;
      if(!title || strcmp(title, "tinyfd_query"))
      {
        lDialogString = (char*)malloc(MAX_PATH + lTitleLen + lMessageLen);
      }

      if(osascriptPresent())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "applescript");
          return (char*)1;
        }
        strcpy(lDialogString, "osascript ");
        if(!osx9orBetter())
          strcat(lDialogString, " -e 'tell application \"System Events\"' -e 'Activate'");
        strcat(lDialogString, " -e 'try' -e 'display dialog \"");
        if(msg && strlen(msg))
        {
          strcat(lDialogString, msg);
        }
        strcat(lDialogString, "\" ");
        strcat(lDialogString, "default answer \"");
        if(aDefaultInput && strlen(aDefaultInput))
        {
          strcat(lDialogString, aDefaultInput);
        }
        strcat(lDialogString, "\" ");
        if(!aDefaultInput)
        {
          strcat(lDialogString, "hidden answer true ");
        }
        if(title && strlen(title))
        {
          strcat(lDialogString, "with title \"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\" ");
        }
        strcat(lDialogString, "with icon note' ");
        strcat(lDialogString, "-e '\"1\" & text returned of result' ");
        strcat(lDialogString, "-e 'on error number -128' ");
        strcat(lDialogString, "-e '0' ");
        strcat(lDialogString, "-e 'end try'");
        if(!osx9orBetter())
          strcat(lDialogString, " -e 'end tell'");
      }
      else if(tfd_kdialogPresent())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "kdialog");
          return (char*)1;
        }
        strcpy(lDialogString, "szAnswer=$(kdialog");

        if((tfd_kdialogPresent() == 2) && tfd_xpropPresent())
        {
          strcat(lDialogString, " --attach=$(xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)"); /* contribution: Paul Rouget */
        }

        if(!aDefaultInput)
        {
          strcat(lDialogString, " --password ");
        }
        else
        {
          strcat(lDialogString, " --inputbox ");
        }
        strcat(lDialogString, "\"");
        if(msg && strlen(msg))
        {
          strcat(lDialogString, msg);
        }
        strcat(lDialogString, "\" \"");
        if(aDefaultInput && strlen(aDefaultInput))
        {
          strcat(lDialogString, aDefaultInput);
        }
        strcat(lDialogString, "\"");
        if(title && strlen(title))
        {
          strcat(lDialogString, " --title \"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\"");
        }
        strcat(lDialogString, ");if [ $? = 0 ];then echo 1$szAnswer;else echo 0$szAnswer;fi");
      }
      else if(tfd_zenityPresent() || tfd_matedialogPresent() || tfd_shellementaryPresent() || tfd_qarmaPresent())
      {
        if(tfd_zenityPresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "zenity");
            return (char*)1;
          }
          strcpy(lDialogString, "szAnswer=$(zenity");
          if((tfd_zenity3Present() >= 4) && !getenv("SSH_TTY") && tfd_xpropPresent())
          {
            strcat(lDialogString, " --attach=$(sleep .01;xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)"); /* contribution: Paul Rouget */
          }
        }
        else if(tfd_matedialogPresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "matedialog");
            return (char*)1;
          }
          strcpy(lDialogString, "szAnswer=$(matedialog");
        }
        else if(tfd_shellementaryPresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "shellementary");
            return (char*)1;
          }
          strcpy(lDialogString, "szAnswer=$(shellementary");
        }
        else
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "qarma");
            return (char*)1;
          }
          strcpy(lDialogString, "szAnswer=$(qarma");
          if(!getenv("SSH_TTY") && tfd_xpropPresent())
          {
            strcat(lDialogString, " --attach=$(xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)"); /* contribution: Paul Rouget */
          }
        }
        strcat(lDialogString, " --entry");

        strcat(lDialogString, " --title=\"");
        if(title && strlen(title))
          strcat(lDialogString, title);
        strcat(lDialogString, "\"");

        strcat(lDialogString, " --text=\"");
        if(msg && strlen(msg))
          strcat(lDialogString, msg);
        strcat(lDialogString, "\"");

        if(aDefaultInput)
        {
          strcat(lDialogString, " --entry-text=\"");
          strcat(lDialogString, aDefaultInput);
          strcat(lDialogString, "\"");
        }
        else
        {
          strcat(lDialogString, " --hide-text");
        }
        if(tinyfd_silent)
          strcat(lDialogString, " 2>/dev/null ");
        strcat(lDialogString, ");if [ $? = 0 ];then echo 1$szAnswer;else echo 0$szAnswer;fi");
      }
      else if(tfd_yadPresent())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "yad");
          return (char*)1;
        }
        strcpy(lDialogString, "szAnswer=$(yad --entry");
        if(title && strlen(title))
        {
          strcat(lDialogString, " --title=\"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\"");
        }
        if(msg && strlen(msg))
        {
          strcat(lDialogString, " --text=\"");
          strcat(lDialogString, msg);
          strcat(lDialogString, "\"");
        }
        if(aDefaultInput && strlen(aDefaultInput))
        {
          strcat(lDialogString, " --entry-text=\"");
          strcat(lDialogString, aDefaultInput);
          strcat(lDialogString, "\"");
        }
        else
        {
          strcat(lDialogString, " --hide-text");
        }
        if(tinyfd_silent)
          strcat(lDialogString, " 2>/dev/null ");
        strcat(lDialogString, ");if [ $? = 0 ];then echo 1$szAnswer;else echo 0$szAnswer;fi");
      }
      else if(gxmessagePresent() || gmessagePresent())
      {
        if(gxmessagePresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "gxmessage");
            return (char*)1;
          }
          strcpy(lDialogString, "szAnswer=$(gxmessage -buttons Ok:1,Cancel:0 -center \"");
        }
        else
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "gmessage");
            return (char*)1;
          }
          strcpy(lDialogString, "szAnswer=$(gmessage -buttons Ok:1,Cancel:0 -center \"");
        }

        if(msg && strlen(msg))
        {
          strcat(lDialogString, msg);
        }
        strcat(lDialogString, "\"");
        if(title && strlen(title))
        {
          strcat(lDialogString, " -title  \"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\" ");
        }
        strcat(lDialogString, " -entrytext \"");
        if(aDefaultInput && strlen(aDefaultInput))
        {
          strcat(lDialogString, aDefaultInput);
        }
        strcat(lDialogString, "\"");
        strcat(lDialogString, ");echo $?$szAnswer");
      }
      else if(!gdialogPresent() && !xdialogPresent() && tkinter3Present())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "python3-tkinter");
          return (char*)1;
        }
        strcpy(lDialogString, gPython3Name);
        strcat(lDialogString, " -S -c \"import tkinter; from tkinter import simpledialog;root=tkinter.Tk();root.withdraw();");
        strcat(lDialogString, "res=simpledialog.askstring(");
        if(title && strlen(title))
        {
          strcat(lDialogString, "title='");
          strcat(lDialogString, title);
          strcat(lDialogString, "',");
        }
        if(msg && strlen(msg))
        {
          strcat(lDialogString, "prompt='");
          lpDialogString = lDialogString + strlen(lDialogString);
          tfd_replaceSubStr(msg, "\n", "\\n", lpDialogString);
          strcat(lDialogString, "',");
        }
        if(aDefaultInput)
        {
          if(strlen(aDefaultInput))
          {
            strcat(lDialogString, "initialvalue='");
            strcat(lDialogString, aDefaultInput);
            strcat(lDialogString, "',");
          }
        }
        else
        {
          strcat(lDialogString, "show='*'");
        }
        strcat(lDialogString, ");\nif res is None :\n\tprint(0)");
        strcat(lDialogString, "\nelse :\n\tprint('1'+res)\n\"");
      }
      else if(!gdialogPresent() && !xdialogPresent() && tkinter2Present())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "python2-tkinter");
          return (char*)1;
        }
        strcpy(lDialogString, "export PYTHONIOENCODING=utf-8;");
        strcat(lDialogString, gPython2Name);
        if(!isTerminalRunning() && tfd_isDarwin())
        {
          strcat(lDialogString, " -i"); /* for osx without console */
        }

        strcat(lDialogString, " -S -c \"import Tkinter,tkSimpleDialog;root=Tkinter.Tk();root.withdraw();");

        if(tfd_isDarwin())
        {
          strcat(lDialogString, "import os;os.system('''/usr/bin/osascript -e 'tell app \\\"Finder\\\" to set \
frontmost of process \\\"Python\\\" to true' ''');");
        }

        strcat(lDialogString, "res=tkSimpleDialog.askstring(");
        if(title && strlen(title))
        {
          strcat(lDialogString, "title='");
          strcat(lDialogString, title);
          strcat(lDialogString, "',");
        }
        if(msg && strlen(msg))
        {
          strcat(lDialogString, "prompt='");
          lpDialogString = lDialogString + strlen(lDialogString);
          tfd_replaceSubStr(msg, "\n", "\\n", lpDialogString);
          strcat(lDialogString, "',");
        }
        if(aDefaultInput)
        {
          if(strlen(aDefaultInput))
          {
            strcat(lDialogString, "initialvalue='");
            strcat(lDialogString, aDefaultInput);
            strcat(lDialogString, "',");
          }
        }
        else
        {
          strcat(lDialogString, "show='*'");
        }
        strcat(lDialogString, ");\nif res is None :\n\tprint 0");
        strcat(lDialogString, "\nelse :\n\tprint '1'+res\n\"");
      }
      else if(gdialogPresent() || xdialogPresent() || dialogName() || whiptailPresent())
      {
        if(gdialogPresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "gdialog");
            return (char*)1;
          }
          lWasGraphicDialog = 1;
          lWasGdialog       = 1;
          strcpy(lDialogString, "(gdialog ");
        }
        else if(xdialogPresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "xdialog");
            return (char*)1;
          }
          lWasGraphicDialog = 1;
          strcpy(lDialogString, "(Xdialog ");
        }
        else if(dialogName())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "dialog");
            return (char*)0;
          }
          if(isTerminalRunning())
          {
            strcpy(lDialogString, "(dialog ");
          }
          else
          {
            lWasXterm = 1;
            strcpy(lDialogString, terminalName());
            strcat(lDialogString, "'(");
            strcat(lDialogString, dialogName());
            strcat(lDialogString, " ");
          }
        }
        else if(isTerminalRunning())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "whiptail");
            return (char*)0;
          }
          strcpy(lDialogString, "(whiptail ");
        }
        else
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "whiptail");
            return (char*)0;
          }
          lWasXterm = 1;
          strcpy(lDialogString, terminalName());
          strcat(lDialogString, "'(whiptail ");
        }

        if(title && strlen(title))
        {
          strcat(lDialogString, "--title \"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\" ");
        }

        if(!xdialogPresent() && !gdialogPresent())
        {
          strcat(lDialogString, "--backtitle \"");
          strcat(lDialogString, "tab: move focus");
          if(!aDefaultInput && !lWasGdialog)
          {
            strcat(lDialogString, " (sometimes nothing, no blink nor star, is shown in text field)");
          }
          strcat(lDialogString, "\" ");
        }

        if(aDefaultInput || lWasGdialog)
        {
          strcat(lDialogString, "--inputbox");
        }
        else
        {
          if(!lWasGraphicDialog && dialogName() && isDialogVersionBetter09b())
          {
            strcat(lDialogString, "--insecure ");
          }
          strcat(lDialogString, "--passwordbox");
        }
        strcat(lDialogString, " \"");
        if(msg && strlen(msg))
        {
          strcat(lDialogString, msg);
        }
        strcat(lDialogString, "\" 10 60 ");
        if(aDefaultInput && strlen(aDefaultInput))
        {
          strcat(lDialogString, "\"");
          strcat(lDialogString, aDefaultInput);
          strcat(lDialogString, "\" ");
        }
        if(lWasGraphicDialog)
        {
          strcat(lDialogString, ") 2>/tmp/tinyfd.txt;\
        if [ $? = 0 ];then tinyfdBool=1;else tinyfdBool=0;fi;\
        tinyfdRes=$(cat /tmp/tinyfd.txt);echo $tinyfdBool$tinyfdRes");
        }
        else
        {
          strcat(lDialogString, ">/dev/tty ) 2>/tmp/tinyfd.txt;\
        if [ $? = 0 ];then tinyfdBool=1;else tinyfdBool=0;fi;\
        tinyfdRes=$(cat /tmp/tinyfd.txt);echo $tinyfdBool$tinyfdRes");

          if(lWasXterm)
          {
            strcat(lDialogString, " >/tmp/tinyfd0.txt';cat /tmp/tinyfd0.txt");
          }
          else
          {
            strcat(lDialogString, "; clear >/dev/tty");
          }
        }
      }
      else if(!isTerminalRunning() && terminalName())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "basicinput");
          return (char*)0;
        }
        lWasBasicXterm = 1;
        strcpy(lDialogString, terminalName());
        strcat(lDialogString, "'");
        if(!gWarningDisplayed && !tinyfd_forceConsole)
        {
          gWarningDisplayed = 1;
          tinyfd_msg_box(gTitle, tinyfd_needs, "ok", "warning", 0);
        }
        if(title && strlen(title) && !tinyfd_forceConsole)
        {
          strcat(lDialogString, "echo \"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\";echo;");
        }

        strcat(lDialogString, "echo \"");
        if(msg && strlen(msg))
        {
          strcat(lDialogString, msg);
        }
        strcat(lDialogString, "\";read ");
        if(!aDefaultInput)
        {
          strcat(lDialogString, "-s ");
        }
        strcat(lDialogString, "-p \"");
        strcat(lDialogString, "(esc+enter to cancel): \" ANSWER ");
        strcat(lDialogString, ";echo 1$ANSWER >/tmp/tinyfd.txt';");
        strcat(lDialogString, "cat -v /tmp/tinyfd.txt");
      }
      else if(!gWarningDisplayed && !isTerminalRunning() && !terminalName())
      {
        gWarningDisplayed = 1;
        tinyfd_msg_box(gTitle, tinyfd_needs, "ok", "warning", 0);
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "no_solution");
          return (char*)0;
        }
        free(lDialogString);
        return NULL;
      }
      else
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "basicinput");
          return (char*)0;
        }
        if(!gWarningDisplayed && !tinyfd_forceConsole)
        {
          gWarningDisplayed = 1;
          tinyfd_msg_box(gTitle, tinyfd_needs, "ok", "warning", 0);
        }
        if(title && strlen(title))
        {
          printf("\n%s\n", title);
        }
        if(msg && strlen(msg))
        {
          printf("\n%s\n", msg);
        }
        printf("(esc+enter to cancel): ");
        fflush(stdout);
        if(!aDefaultInput)
        {
          tcgetattr(STDIN_FILENO, &oldt);
          newt = oldt;
          newt.c_lflag &= ~ECHO;
          tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        }

        lEOF = fgets(dirname, MAX_PATH, stdin);
        /* printf("lbuff<%c><%d>\n",dirname[0],dirname[0]); */
        if(!lEOF || (dirname[0] == '\0'))
        {
          free(lDialogString);
          return NULL;
        }

        if(dirname[0] == '\n')
        {
          lEOF = fgets(dirname, MAX_PATH, stdin);
          /* printf("lbuff<%c><%d>\n",dirname[0],dirname[0]); */
          if(!lEOF || (dirname[0] == '\0'))
          {
            free(lDialogString);
            return NULL;
          }
        }

        if(!aDefaultInput)
        {
          tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
          printf("\n");
        }
        printf("\n");
        if(strchr(dirname, 27))
        {
          free(lDialogString);
          return NULL;
        }
        if(dirname[strlen(dirname) - 1] == '\n')
        {
          dirname[strlen(dirname) - 1] = '\0';
        }
        free(lDialogString);
        return dirname;
      }

      if(tinyfd_verbose)
        printf("lDialogString: %s\n", lDialogString);
      lIn = popen(lDialogString, "r");
      if(!lIn)
      {
        if(fileExists("/tmp/tinyfd.txt"))
        {
          wipefile("/tmp/tinyfd.txt");
          remove("/tmp/tinyfd.txt");
        }
        if(fileExists("/tmp/tinyfd0.txt"))
        {
          wipefile("/tmp/tinyfd0.txt");
          remove("/tmp/tinyfd0.txt");
        }
        free(lDialogString);
        return NULL;
      }
      while(fgets(dirname, sizeof(dirname), lIn) != NULL)
      {
      }

      pclose(lIn);

      if(fileExists("/tmp/tinyfd.txt"))
      {
        wipefile("/tmp/tinyfd.txt");
        remove("/tmp/tinyfd.txt");
      }
      if(fileExists("/tmp/tinyfd0.txt"))
      {
        wipefile("/tmp/tinyfd0.txt");
        remove("/tmp/tinyfd0.txt");
      }

      /* printf( "len Buff: %lu\n" , strlen(dirname) ) ; */
      /* printf( "dirname0: %s\n" , dirname ) ; */
      if(dirname[strlen(dirname) - 1] == '\n')
      {
        dirname[strlen(dirname) - 1] = '\0';
      }
      /* printf( "dirname1: %s len: %lu \n" , dirname , strlen(dirname) ) ; */
      if(lWasBasicXterm)
      {
        if(strstr(dirname, "^[")) /* esc was pressed */
        {
          free(lDialogString);
          return NULL;
        }
      }

      lResult = strncmp(dirname, "1", 1) ? 0 : 1;
      /* printf( "lResult: %d \n" , lResult ) ; */
      if(!lResult)
      {
        free(lDialogString);
        return NULL;
      }

      /* printf( "dirname+1: %s\n" , dirname+1 ) ; */
      free(lDialogString);
      return dirname + 1;
    }

    char* tinyfd_save_file_dialog(const char* title,                    /* NULL or "" */
                                  const char* defaultPath,              /* NULL or "" */
                                  int aNumOfFilterPatterns,             /* 0 */
                                  const char* const* aFilterPatterns,   /* NULL or {"*.txt","*.doc"} */
                                  const char* aSingleFilterDescription) /* NULL or "text files" */
    {
      static char dirname[MAX_PATH];
      char lDialogString[MAX_PATH];
      char lString[MAX_PATH];
      int i;
      int lWasGraphicDialog = 0;
      int lWasXterm         = 0;
      char* p;
      char* lPointerInputBox;
      FILE* lIn;
      dirname[0] = '\0';

      if(!aFilterPatterns)
        aNumOfFilterPatterns = 0;
      if(tfd_quoteDetected(title))
        return save_file_dialog("INVALID TITLE WITH QUOTES", defaultPath, aNumOfFilterPatterns, aFilterPatterns, aSingleFilterDescription);
      if(tfd_quoteDetected(defaultPath))
        return save_file_dialog(title, "INVALID DEFAULT_PATH WITH QUOTES", aNumOfFilterPatterns, aFilterPatterns, aSingleFilterDescription);
      if(tfd_quoteDetected(aSingleFilterDescription))
        return save_file_dialog(title, defaultPath, aNumOfFilterPatterns, aFilterPatterns, "INVALID FILTER_DESCRIPTION WITH QUOTES");
      for(i = 0; i < aNumOfFilterPatterns; i++)
      {
        if(tfd_quoteDetected(aFilterPatterns[i]))
          return save_file_dialog("INVALID FILTER_PATTERN WITH QUOTES", defaultPath, 0, NULL, NULL);
      }

      if(osascriptPresent())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "applescript");
          return (char*)1;
        }
        strcpy(lDialogString, "osascript ");
        if(!osx9orBetter())
          strcat(lDialogString, " -e 'tell application \"Finder\"' -e 'Activate'");
        strcat(lDialogString, " -e 'try' -e 'POSIX path of ( choose file name ");
        if(title && strlen(title))
        {
          strcat(lDialogString, "with prompt \"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\" ");
        }
        getPathWithoutFinalSlash(lString, defaultPath);
        if(strlen(lString))
        {
          strcat(lDialogString, "default location \"");
          strcat(lDialogString, lString);
          strcat(lDialogString, "\" ");
        }
        getLastName(lString, defaultPath);
        if(strlen(lString))
        {
          strcat(lDialogString, "default name \"");
          strcat(lDialogString, lString);
          strcat(lDialogString, "\" ");
        }
        strcat(lDialogString, ")' ");
        strcat(lDialogString, "-e 'on error number -128' ");
        strcat(lDialogString, "-e 'end try'");
        if(!osx9orBetter())
          strcat(lDialogString, " -e 'end tell'");
      }
      else if(tfd_kdialogPresent())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "kdialog");
          return (char*)1;
        }

        strcpy(lDialogString, "kdialog");
        if((tfd_kdialogPresent() == 2) && tfd_xpropPresent())
        {
          strcat(lDialogString, " --attach=$(xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)"); /* contribution: Paul Rouget */
        }
        strcat(lDialogString, " --getsavefilename ");

        if(defaultPath && strlen(defaultPath))
        {
          if(defaultPath[0] != '/')
          {
            strcat(lDialogString, "$PWD/");
          }
          strcat(lDialogString, "\"");
          strcat(lDialogString, defaultPath);
          strcat(lDialogString, "\"");
        }
        else
        {
          strcat(lDialogString, "$PWD/");
        }

        if(aNumOfFilterPatterns > 0)
        {
          strcat(lDialogString, " \"");
          strcat(lDialogString, aFilterPatterns[0]);
          for(i = 1; i < aNumOfFilterPatterns; i++)
          {
            strcat(lDialogString, " ");
            strcat(lDialogString, aFilterPatterns[i]);
          }
          if(aSingleFilterDescription && strlen(aSingleFilterDescription))
          {
            strcat(lDialogString, " | ");
            strcat(lDialogString, aSingleFilterDescription);
          }
          strcat(lDialogString, "\"");
        }
        if(title && strlen(title))
        {
          strcat(lDialogString, " --title \"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\"");
        }
      }
      else if(tfd_zenityPresent() || tfd_matedialogPresent() || tfd_shellementaryPresent() || tfd_qarmaPresent())
      {
        if(tfd_zenityPresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "zenity");
            return (char*)1;
          }
          strcpy(lDialogString, "zenity");
          if((tfd_zenity3Present() >= 4) && !getenv("SSH_TTY") && tfd_xpropPresent())
          {
            strcat(lDialogString, " --attach=$(sleep .01;xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)"); /* contribution: Paul Rouget */
          }
        }
        else if(tfd_matedialogPresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "matedialog");
            return (char*)1;
          }
          strcpy(lDialogString, "matedialog");
        }
        else if(tfd_shellementaryPresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "shellementary");
            return (char*)1;
          }
          strcpy(lDialogString, "shellementary");
        }
        else
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "qarma");
            return (char*)1;
          }
          strcpy(lDialogString, "qarma");
          if(!getenv("SSH_TTY") && tfd_xpropPresent())
          {
            strcat(lDialogString, " --attach=$(xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)"); /* contribution: Paul Rouget */
          }
        }
        strcat(lDialogString, " --file-selection --save --confirm-overwrite");

        strcat(lDialogString, " --title=\"");
        if(title && strlen(title))
          strcat(lDialogString, title);
        strcat(lDialogString, "\"");

        if(defaultPath && strlen(defaultPath))
        {
          strcat(lDialogString, " --filename=\"");
          strcat(lDialogString, defaultPath);
          strcat(lDialogString, "\"");
        }
        if(aNumOfFilterPatterns > 0)
        {
          strcat(lDialogString, " --file-filter='");
          if(aSingleFilterDescription && strlen(aSingleFilterDescription))
          {
            strcat(lDialogString, aSingleFilterDescription);
            strcat(lDialogString, " |");
          }
          for(i = 0; i < aNumOfFilterPatterns; i++)
          {
            strcat(lDialogString, " ");
            strcat(lDialogString, aFilterPatterns[i]);
          }
          strcat(lDialogString, "' --file-filter='All files | *'");
        }
        if(tinyfd_silent)
          strcat(lDialogString, " 2>/dev/null ");
      }
      else if(tfd_yadPresent())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "yad");
          return (char*)1;
        }
        strcpy(lDialogString, "yad --file --save --confirm-overwrite");
        if(title && strlen(title))
        {
          strcat(lDialogString, " --title=\"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\"");
        }
        if(defaultPath && strlen(defaultPath))
        {
          strcat(lDialogString, " --filename=\"");
          strcat(lDialogString, defaultPath);
          strcat(lDialogString, "\"");
        }
        if(aNumOfFilterPatterns > 0)
        {
          strcat(lDialogString, " --file-filter='");
          if(aSingleFilterDescription && strlen(aSingleFilterDescription))
          {
            strcat(lDialogString, aSingleFilterDescription);
            strcat(lDialogString, " |");
          }
          for(i = 0; i < aNumOfFilterPatterns; i++)
          {
            strcat(lDialogString, " ");
            strcat(lDialogString, aFilterPatterns[i]);
          }
          strcat(lDialogString, "' --file-filter='All files | *'");
        }
        if(tinyfd_silent)
          strcat(lDialogString, " 2>/dev/null ");
      }
      else if(!xdialogPresent() && tkinter3Present())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "python3-tkinter");
          return (char*)1;
        }
        strcpy(lDialogString, gPython3Name);
        strcat(lDialogString, " -S -c \"import tkinter;from tkinter import filedialog;root=tkinter.Tk();root.withdraw();");
        strcat(lDialogString, "res=filedialog.asksaveasfilename(");
        if(title && strlen(title))
        {
          strcat(lDialogString, "title='");
          strcat(lDialogString, title);
          strcat(lDialogString, "',");
        }
        if(defaultPath && strlen(defaultPath))
        {
          getPathWithoutFinalSlash(lString, defaultPath);
          if(strlen(lString))
          {
            strcat(lDialogString, "initialdir='");
            strcat(lDialogString, lString);
            strcat(lDialogString, "',");
          }
          getLastName(lString, defaultPath);
          if(strlen(lString))
          {
            strcat(lDialogString, "initialfile='");
            strcat(lDialogString, lString);
            strcat(lDialogString, "',");
          }
        }
        if((aNumOfFilterPatterns > 1) || ((aNumOfFilterPatterns == 1) /* test because poor osx behaviour */
                                          && (aFilterPatterns[0][strlen(aFilterPatterns[0]) - 1] != '*')))
        {
          strcat(lDialogString, "filetypes=(");
          strcat(lDialogString, "('");
          if(aSingleFilterDescription && strlen(aSingleFilterDescription))
          {
            strcat(lDialogString, aSingleFilterDescription);
          }
          strcat(lDialogString, "',(");
          for(i = 0; i < aNumOfFilterPatterns; i++)
          {
            strcat(lDialogString, "'");
            strcat(lDialogString, aFilterPatterns[i]);
            strcat(lDialogString, "',");
          }
          strcat(lDialogString, ")),");
          strcat(lDialogString, "('All files','*'))");
        }
        strcat(lDialogString, ");\nif not isinstance(res, tuple):\n\tprint(res)\n\"");
      }
      else if(!xdialogPresent() && tkinter2Present())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "python2-tkinter");
          return (char*)1;
        }
        strcpy(lDialogString, "export PYTHONIOENCODING=utf-8;");
        strcat(lDialogString, gPython2Name);
        if(!isTerminalRunning() && tfd_isDarwin())
        {
          strcat(lDialogString, " -i"); /* for osx without console */
        }
        strcat(lDialogString, " -S -c \"import Tkinter,tkFileDialog;root=Tkinter.Tk();root.withdraw();");

        if(tfd_isDarwin())
        {
          strcat(lDialogString, "import os;os.system('''/usr/bin/osascript -e 'tell app \\\"Finder\\\" to set\
 frontmost of process \\\"Python\\\" to true' ''');");
        }

        strcat(lDialogString, "res=tkFileDialog.asksaveasfilename(");
        if(title && strlen(title))
        {
          strcat(lDialogString, "title='");
          strcat(lDialogString, title);
          strcat(lDialogString, "',");
        }
        if(defaultPath && strlen(defaultPath))
        {
          getPathWithoutFinalSlash(lString, defaultPath);
          if(strlen(lString))
          {
            strcat(lDialogString, "initialdir='");
            strcat(lDialogString, lString);
            strcat(lDialogString, "',");
          }
          getLastName(lString, defaultPath);
          if(strlen(lString))
          {
            strcat(lDialogString, "initialfile='");
            strcat(lDialogString, lString);
            strcat(lDialogString, "',");
          }
        }
        if((aNumOfFilterPatterns > 1) || ((aNumOfFilterPatterns == 1) /* test because poor osx behaviour */
                                          && (aFilterPatterns[0][strlen(aFilterPatterns[0]) - 1] != '*')))
        {
          strcat(lDialogString, "filetypes=(");
          strcat(lDialogString, "('");
          if(aSingleFilterDescription && strlen(aSingleFilterDescription))
          {
            strcat(lDialogString, aSingleFilterDescription);
          }
          strcat(lDialogString, "',(");
          for(i = 0; i < aNumOfFilterPatterns; i++)
          {
            strcat(lDialogString, "'");
            strcat(lDialogString, aFilterPatterns[i]);
            strcat(lDialogString, "',");
          }
          strcat(lDialogString, ")),");
          strcat(lDialogString, "('All files','*'))");
        }
        strcat(lDialogString, ");\nif not isinstance(res, tuple):\n\tprint res \n\"");
      }
      else if(xdialogPresent() || dialogName())
      {
        if(xdialogPresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "xdialog");
            return (char*)1;
          }
          lWasGraphicDialog = 1;
          strcpy(lDialogString, "(Xdialog ");
        }
        else if(isTerminalRunning())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "dialog");
            return (char*)0;
          }
          strcpy(lDialogString, "(dialog ");
        }
        else
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "dialog");
            return (char*)0;
          }
          lWasXterm = 1;
          strcpy(lDialogString, terminalName());
          strcat(lDialogString, "'(");
          strcat(lDialogString, dialogName());
          strcat(lDialogString, " ");
        }

        if(title && strlen(title))
        {
          strcat(lDialogString, "--title \"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\" ");
        }

        if(!xdialogPresent() && !gdialogPresent())
        {
          strcat(lDialogString, "--backtitle \"");
          strcat(lDialogString, "tab: focus | /: populate | spacebar: fill text field | ok: TEXT FIELD ONLY");
          strcat(lDialogString, "\" ");
        }

        strcat(lDialogString, "--fselect \"");
        if(defaultPath && strlen(defaultPath))
        {
          if(!strchr(defaultPath, '/'))
          {
            strcat(lDialogString, "./");
          }
          strcat(lDialogString, defaultPath);
        }
        else if(!isTerminalRunning() && !lWasGraphicDialog)
        {
          strcat(lDialogString, getenv("HOME"));
          strcat(lDialogString, "/");
        }
        else
        {
          strcat(lDialogString, "./");
        }

        if(lWasGraphicDialog)
        {
          strcat(lDialogString, "\" 0 60 ) 2>&1 ");
        }
        else
        {
          strcat(lDialogString, "\" 0 60  >/dev/tty) ");
          if(lWasXterm)
          {
            strcat(lDialogString, "2>/tmp/tinyfd.txt';cat /tmp/tinyfd.txt;rm /tmp/tinyfd.txt");
          }
          else
          {
            strcat(lDialogString, "2>&1 ; clear >/dev/tty");
          }
        }
      }
      else
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          return inputBox(title, NULL, NULL);
        }
        strcpy(dirname, "Save file in ");
        strcat(dirname, getCurDir());
        lPointerInputBox = inputBox(NULL, NULL, NULL); /* obtain a pointer on the current content of tinyfd_inputBox */
        if(lPointerInputBox)
          strcpy(lString, lPointerInputBox);           /* preserve the current content of tinyfd_inputBox */
        p = inputBox(title, dirname, "");
        if(p)
          strcpy(dirname, p);
        else
          dirname[0] = '\0';
        if(lPointerInputBox)
          strcpy(lPointerInputBox, lString); /* restore its previous content to tinyfd_inputBox */
        p = dirname;

        getPathWithoutFinalSlash(lString, p);
        if(strlen(lString) && !dirExists(lString))
        {
          return NULL;
        }
        getLastName(lString, p);
        if(!strlen(lString))
        {
          return NULL;
        }
        return p;
      }

      if(tinyfd_verbose)
        printf("lDialogString: %s\n", lDialogString);
      if(!(lIn = popen(lDialogString, "r")))
      {
        return NULL;
      }
      while(fgets(dirname, sizeof(dirname), lIn) != NULL)
      {
      }
      pclose(lIn);
      if(dirname[strlen(dirname) - 1] == '\n')
      {
        dirname[strlen(dirname) - 1] = '\0';
      }
      /* printf( "dirname: %s\n" , dirname ) ; */
      if(!strlen(dirname))
      {
        return NULL;
      }
      getPathWithoutFinalSlash(lString, dirname);
      if(strlen(lString) && !dirExists(lString))
      {
        return NULL;
      }
      getLastName(lString, dirname);
      if(!filenameValid(lString))
      {
        return NULL;
      }
      return dirname;
    }

    /* in case of multiple files, the separator is | */
    char* tinyfd_open_file_dialog_multiselect(const char* title,                    /* NULL or "" */
                                              const char* defaultPath,              /* NULL or "" */
                                              int aNumOfFilterPatterns,             /* 0 */
                                              const char* const* aFilterPatterns,   /* NULL or {"*.jpg","*.png"} */
                                              const char* aSingleFilterDescription, /* NULL or "image files" */
                                              int aAllowMultipleSelects)            /* 0 or 1 */
    {
      char lDialogString[MAX_PATH];
      char lString[MAX_PATH];
      int i;
      FILE* lIn;
      char* p;
      char* lPointerInputBox;
      int lWasKdialog       = 0;
      int lWasGraphicDialog = 0;
      int lWasXterm         = 0;
      size_t lFuldirnameLen;
      static char* dirname = NULL;

      if(!aFilterPatterns)
        aNumOfFilterPatterns = 0;
      if(tfd_quoteDetected(title))
        return open_file_dialog_multiselect("INVALID TITLE WITH QUOTES", defaultPath, aNumOfFilterPatterns, aFilterPatterns, aSingleFilterDescription, aAllowMultipleSelects);
      if(tfd_quoteDetected(defaultPath))
        return open_file_dialog_multiselect(title, "INVALID DEFAULT_PATH WITH QUOTES", aNumOfFilterPatterns, aFilterPatterns, aSingleFilterDescription, aAllowMultipleSelects);
      if(tfd_quoteDetected(aSingleFilterDescription))
        return open_file_dialog_multiselect(title, defaultPath, aNumOfFilterPatterns, aFilterPatterns, "INVALID FILTER_DESCRIPTION WITH QUOTES", aAllowMultipleSelects);
      for(i = 0; i < aNumOfFilterPatterns; i++)
      {
        if(tfd_quoteDetected(aFilterPatterns[i]))
          return open_file_dialog_multiselect("INVALID FILTER_PATTERN WITH QUOTES", defaultPath, 0, NULL, NULL, aAllowMultipleSelects);
      }

      free(dirname);
      if(title && !strcmp(title, "tinyfd_query"))
      {
        dirname = NULL;
      }
      else
      {
        if(aAllowMultipleSelects)
        {
          lFuldirnameLen = MAX_MULTIPLE_FILES * MAX_PATH + 1;
          dirname        = (char*)(malloc(lFuldirnameLen * sizeof(char)));
          if(!dirname)
          {
            lFuldirnameLen = LOW_MULTIPLE_FILES * MAX_PATH + 1;
            dirname        = (char*)(malloc(lFuldirnameLen * sizeof(char)));
          }
        }
        else
        {
          lFuldirnameLen = MAX_PATH + 1;
          dirname        = (char*)(malloc(lFuldirnameLen * sizeof(char)));
        }
        if(!dirname)
          return NULL;
        dirname[0] = '\0';
      }

      if(osascriptPresent())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "applescript");
          return (char*)1;
        }
        strcpy(lDialogString, "osascript ");
        if(!osx9orBetter())
          strcat(lDialogString, " -e 'tell application \"System Events\"' -e 'Activate'");
        strcat(lDialogString, " -e 'try' -e '");
        if(!aAllowMultipleSelects)
        {
          strcat(lDialogString, "POSIX path of ( ");
        }
        else
        {
          strcat(lDialogString, "set mylist to ");
        }
        strcat(lDialogString, "choose file ");
        if(title && strlen(title))
        {
          strcat(lDialogString, "with prompt \"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\" ");
        }
        getPathWithoutFinalSlash(lString, defaultPath);
        if(strlen(lString))
        {
          strcat(lDialogString, "default location \"");
          strcat(lDialogString, lString);
          strcat(lDialogString, "\" ");
        }
        if(aNumOfFilterPatterns > 0)
        {
          strcat(lDialogString, "of type {\"");
          strcat(lDialogString, aFilterPatterns[0] + 2);
          strcat(lDialogString, "\"");
          for(i = 1; i < aNumOfFilterPatterns; i++)
          {
            strcat(lDialogString, ",\"");
            strcat(lDialogString, aFilterPatterns[i] + 2);
            strcat(lDialogString, "\"");
          }
          strcat(lDialogString, "} ");
        }
        if(aAllowMultipleSelects)
        {
          strcat(lDialogString, "multiple selections allowed true ' ");
          strcat(lDialogString, "-e 'set mystring to POSIX path of item 1 of mylist' ");
          strcat(lDialogString, "-e 'repeat with  i from 2 to the count of mylist' ");
          strcat(lDialogString, "-e 'set mystring to mystring & \"|\"' ");
          strcat(lDialogString, "-e 'set mystring to mystring & POSIX path of item i of mylist' ");
          strcat(lDialogString, "-e 'end repeat' ");
          strcat(lDialogString, "-e 'mystring' ");
        }
        else
        {
          strcat(lDialogString, ")' ");
        }
        strcat(lDialogString, "-e 'on error number -128' ");
        strcat(lDialogString, "-e 'end try'");
        if(!osx9orBetter())
          strcat(lDialogString, " -e 'end tell'");
      }
      else if(tfd_kdialogPresent())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "kdialog");
          return (char*)1;
        }
        lWasKdialog = 1;

        strcpy(lDialogString, "kdialog");
        if((tfd_kdialogPresent() == 2) && tfd_xpropPresent())
        {
          strcat(lDialogString, " --attach=$(xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)"); /* contribution: Paul Rouget */
        }
        strcat(lDialogString, " --getopenfilename ");

        if(defaultPath && strlen(defaultPath))
        {
          if(defaultPath[0] != '/')
          {
            strcat(lDialogString, "$PWD/");
          }
          strcat(lDialogString, "\"");
          strcat(lDialogString, defaultPath);
          strcat(lDialogString, "\"");
        }
        else
        {
          strcat(lDialogString, "$PWD/");
        }

        if(aNumOfFilterPatterns > 0)
        {
          strcat(lDialogString, " \"");
          strcat(lDialogString, aFilterPatterns[0]);
          for(i = 1; i < aNumOfFilterPatterns; i++)
          {
            strcat(lDialogString, " ");
            strcat(lDialogString, aFilterPatterns[i]);
          }
          if(aSingleFilterDescription && strlen(aSingleFilterDescription))
          {
            strcat(lDialogString, " | ");
            strcat(lDialogString, aSingleFilterDescription);
          }
          strcat(lDialogString, "\"");
        }
        if(aAllowMultipleSelects)
        {
          strcat(lDialogString, " --multiple --separate-output");
        }
        if(title && strlen(title))
        {
          strcat(lDialogString, " --title \"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\"");
        }
      }
      else if(tfd_zenityPresent() || tfd_matedialogPresent() || tfd_shellementaryPresent() || tfd_qarmaPresent())
      {
        if(tfd_zenityPresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "zenity");
            return (char*)1;
          }
          strcpy(lDialogString, "zenity");
          if((tfd_zenity3Present() >= 4) && !getenv("SSH_TTY") && tfd_xpropPresent())
          {
            strcat(lDialogString, " --attach=$(sleep .01;xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)"); /* contribution: Paul Rouget */
          }
        }
        else if(tfd_matedialogPresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "matedialog");
            return (char*)1;
          }
          strcpy(lDialogString, "matedialog");
        }
        else if(tfd_shellementaryPresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "shellementary");
            return (char*)1;
          }
          strcpy(lDialogString, "shellementary");
        }
        else
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "qarma");
            return (char*)1;
          }
          strcpy(lDialogString, "qarma");
          if(!getenv("SSH_TTY") && tfd_xpropPresent())
          {
            strcat(lDialogString, " --attach=$(xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)"); /* contribution: Paul Rouget */
          }
        }
        strcat(lDialogString, " --file-selection");

        if(aAllowMultipleSelects)
        {
          strcat(lDialogString, " --multiple");
        }

        strcat(lDialogString, " --title=\"");
        if(title && strlen(title))
          strcat(lDialogString, title);
        strcat(lDialogString, "\"");

        if(defaultPath && strlen(defaultPath))
        {
          strcat(lDialogString, " --filename=\"");
          strcat(lDialogString, defaultPath);
          strcat(lDialogString, "\"");
        }
        if(aNumOfFilterPatterns > 0)
        {
          strcat(lDialogString, " --file-filter='");
          if(aSingleFilterDescription && strlen(aSingleFilterDescription))
          {
            strcat(lDialogString, aSingleFilterDescription);
            strcat(lDialogString, " |");
          }
          for(i = 0; i < aNumOfFilterPatterns; i++)
          {
            strcat(lDialogString, " ");
            strcat(lDialogString, aFilterPatterns[i]);
          }
          strcat(lDialogString, "' --file-filter='All files | *'");
        }
        if(tinyfd_silent)
          strcat(lDialogString, " 2>/dev/null ");
      }
      else if(tfd_yadPresent())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "yad");
          return (char*)1;
        }
        strcpy(lDialogString, "yad --file");
        if(aAllowMultipleSelects)
        {
          strcat(lDialogString, " --multiple");
        }
        if(title && strlen(title))
        {
          strcat(lDialogString, " --title=\"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\"");
        }
        if(defaultPath && strlen(defaultPath))
        {
          strcat(lDialogString, " --filename=\"");
          strcat(lDialogString, defaultPath);
          strcat(lDialogString, "\"");
        }
        if(aNumOfFilterPatterns > 0)
        {
          strcat(lDialogString, " --file-filter='");
          if(aSingleFilterDescription && strlen(aSingleFilterDescription))
          {
            strcat(lDialogString, aSingleFilterDescription);
            strcat(lDialogString, " |");
          }
          for(i = 0; i < aNumOfFilterPatterns; i++)
          {
            strcat(lDialogString, " ");
            strcat(lDialogString, aFilterPatterns[i]);
          }
          strcat(lDialogString, "' --file-filter='All files | *'");
        }
        if(tinyfd_silent)
          strcat(lDialogString, " 2>/dev/null ");
      }
      else if(tkinter3Present())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "python3-tkinter");
          return (char*)1;
        }
        strcpy(lDialogString, gPython3Name);
        strcat(lDialogString, " -S -c \"import tkinter;from tkinter import filedialog;root=tkinter.Tk();root.withdraw();");
        strcat(lDialogString, "lFiles=filedialog.askopenfilename(");
        if(aAllowMultipleSelects)
        {
          strcat(lDialogString, "multiple=1,");
        }
        if(title && strlen(title))
        {
          strcat(lDialogString, "title='");
          strcat(lDialogString, title);
          strcat(lDialogString, "',");
        }
        if(defaultPath && strlen(defaultPath))
        {
          getPathWithoutFinalSlash(lString, defaultPath);
          if(strlen(lString))
          {
            strcat(lDialogString, "initialdir='");
            strcat(lDialogString, lString);
            strcat(lDialogString, "',");
          }
          getLastName(lString, defaultPath);
          if(strlen(lString))
          {
            strcat(lDialogString, "initialfile='");
            strcat(lDialogString, lString);
            strcat(lDialogString, "',");
          }
        }
        if((aNumOfFilterPatterns > 1) || ((aNumOfFilterPatterns == 1) /*test because poor osx behaviour*/
                                          && (aFilterPatterns[0][strlen(aFilterPatterns[0]) - 1] != '*')))
        {
          strcat(lDialogString, "filetypes=(");
          strcat(lDialogString, "('");
          if(aSingleFilterDescription && strlen(aSingleFilterDescription))
          {
            strcat(lDialogString, aSingleFilterDescription);
          }
          strcat(lDialogString, "',(");
          for(i = 0; i < aNumOfFilterPatterns; i++)
          {
            strcat(lDialogString, "'");
            strcat(lDialogString, aFilterPatterns[i]);
            strcat(lDialogString, "',");
          }
          strcat(lDialogString, ")),");
          strcat(lDialogString, "('All files','*'))");
        }
        strcat(lDialogString, ");\
\nif not isinstance(lFiles, tuple):\n\tprint(lFiles)\nelse:\
\n\tlFilesString=''\n\tfor lFile in lFiles:\n\t\tlFilesString+=str(lFile)+'|'\
\n\tprint(lFilesString[:-1])\n\"");
      }
      else if(tkinter2Present())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "python2-tkinter");
          return (char*)1;
        }
        strcpy(lDialogString, "export PYTHONIOENCODING=utf-8;");
        strcat(lDialogString, gPython2Name);
        if(!isTerminalRunning() && tfd_isDarwin())
        {
          strcat(lDialogString, " -i"); /* for osx without console */
        }
        strcat(lDialogString, " -S -c \"import Tkinter,tkFileDialog;root=Tkinter.Tk();root.withdraw();");

        if(tfd_isDarwin())
        {
          strcat(lDialogString, "import os;os.system('''/usr/bin/osascript -e 'tell app \\\"Finder\\\" to set \
frontmost of process \\\"Python\\\" to true' ''');");
        }
        strcat(lDialogString, "lFiles=tkFileDialog.askopenfilename(");
        if(aAllowMultipleSelects)
        {
          strcat(lDialogString, "multiple=1,");
        }
        if(title && strlen(title))
        {
          strcat(lDialogString, "title='");
          strcat(lDialogString, title);
          strcat(lDialogString, "',");
        }
        if(defaultPath && strlen(defaultPath))
        {
          getPathWithoutFinalSlash(lString, defaultPath);
          if(strlen(lString))
          {
            strcat(lDialogString, "initialdir='");
            strcat(lDialogString, lString);
            strcat(lDialogString, "',");
          }
          getLastName(lString, defaultPath);
          if(strlen(lString))
          {
            strcat(lDialogString, "initialfile='");
            strcat(lDialogString, lString);
            strcat(lDialogString, "',");
          }
        }
        if((aNumOfFilterPatterns > 1) || ((aNumOfFilterPatterns == 1) /*test because poor osx behaviour*/
                                          && (aFilterPatterns[0][strlen(aFilterPatterns[0]) - 1] != '*')))
        {
          strcat(lDialogString, "filetypes=(");
          strcat(lDialogString, "('");
          if(aSingleFilterDescription && strlen(aSingleFilterDescription))
          {
            strcat(lDialogString, aSingleFilterDescription);
          }
          strcat(lDialogString, "',(");
          for(i = 0; i < aNumOfFilterPatterns; i++)
          {
            strcat(lDialogString, "'");
            strcat(lDialogString, aFilterPatterns[i]);
            strcat(lDialogString, "',");
          }
          strcat(lDialogString, ")),");
          strcat(lDialogString, "('All files','*'))");
        }
        strcat(lDialogString, ");\
\nif not isinstance(lFiles, tuple):\n\tprint lFiles\nelse:\
\n\tlFilesString=''\n\tfor lFile in lFiles:\n\t\tlFilesString+=str(lFile)+'|'\
\n\tprint lFilesString[:-1]\n\"");
      }
      else if(xdialogPresent() || dialogName())
      {
        if(xdialogPresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "xdialog");
            return (char*)1;
          }
          lWasGraphicDialog = 1;
          strcpy(lDialogString, "(Xdialog ");
        }
        else if(isTerminalRunning())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "dialog");
            return (char*)0;
          }
          strcpy(lDialogString, "(dialog ");
        }
        else
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "dialog");
            return (char*)0;
          }
          lWasXterm = 1;
          strcpy(lDialogString, terminalName());
          strcat(lDialogString, "'(");
          strcat(lDialogString, dialogName());
          strcat(lDialogString, " ");
        }

        if(title && strlen(title))
        {
          strcat(lDialogString, "--title \"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\" ");
        }

        if(!xdialogPresent() && !gdialogPresent())
        {
          strcat(lDialogString, "--backtitle \"");
          strcat(lDialogString, "tab: focus | /: populate | spacebar: fill text field | ok: TEXT FIELD ONLY");
          strcat(lDialogString, "\" ");
        }

        strcat(lDialogString, "--fselect \"");
        if(defaultPath && strlen(defaultPath))
        {
          if(!strchr(defaultPath, '/'))
          {
            strcat(lDialogString, "./");
          }
          strcat(lDialogString, defaultPath);
        }
        else if(!isTerminalRunning() && !lWasGraphicDialog)
        {
          strcat(lDialogString, getenv("HOME"));
          strcat(lDialogString, "/");
        }
        else
        {
          strcat(lDialogString, "./");
        }

        if(lWasGraphicDialog)
        {
          strcat(lDialogString, "\" 0 60 ) 2>&1 ");
        }
        else
        {
          strcat(lDialogString, "\" 0 60  >/dev/tty) ");
          if(lWasXterm)
          {
            strcat(lDialogString, "2>/tmp/tinyfd.txt';cat /tmp/tinyfd.txt;rm /tmp/tinyfd.txt");
          }
          else
          {
            strcat(lDialogString, "2>&1 ; clear >/dev/tty");
          }
        }
      }
      else
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          return inputBox(title, NULL, NULL);
        }
        strcpy(dirname, "Open file from ");
        strcat(dirname, getCurDir());
        lPointerInputBox = inputBox(NULL, NULL, NULL); /* obtain a pointer on the current content of tinyfd_inputBox */
        if(lPointerInputBox)
          strcpy(lDialogString, lPointerInputBox);     /* preserve the current content of tinyfd_inputBox */
        p = inputBox(title, dirname, "");
        if(p)
          strcpy(dirname, p);
        else
          dirname[0] = '\0';
        if(lPointerInputBox)
          strcpy(lPointerInputBox, lDialogString); /* restore its previous content to tinyfd_inputBox */
        if(!fileExists(dirname))
        {
          free(dirname);
          dirname = NULL;
        }
        else
        {
          dirname = (char*)(realloc(dirname, (strlen(dirname) + 1) * sizeof(char)));
        }
        return dirname;
      }

      if(tinyfd_verbose)
        printf("lDialogString: %s\n", lDialogString);
      if(!(lIn = popen(lDialogString, "r")))
      {
        free(dirname);
        dirname = NULL;
        return NULL;
      }
      dirname[0] = '\0';
      p          = dirname;
      while(fgets(p, sizeof(dirname), lIn) != NULL)
      {
        p += strlen(p);
      }
      pclose(lIn);

      if(strlen(dirname) && dirname[strlen(dirname) - 1] == '\n')
      {
        dirname[strlen(dirname) - 1] = '\0';
      }
      /* printf( "strlen dirname: %d\n" , strlen( dirname ) ) ; */
      if(lWasKdialog && aAllowMultipleSelects)
      {
        p = dirname;
        while((p = strchr(p, '\n')))
          *p = '|';
      }
      /* printf( "dirname2: %s\n" , dirname ) ; */
      if(!strlen(dirname))
      {
        free(dirname);
        dirname = NULL;
        return NULL;
      }
      if(aAllowMultipleSelects && strchr(dirname, '|'))
      {
        if(!ensureFilesExist(dirname, dirname))
        {
          free(dirname);
          dirname = NULL;
          return NULL;
        }
      }
      else if(!fileExists(dirname))
      {
        free(dirname);
        dirname = NULL;
        return NULL;
      }

      dirname = (char*)(realloc(dirname, (strlen(dirname) + 1) * sizeof(char)));

      /*printf( "dirname3 [%lu]: %s\n" , strlen(dirname) , dirname ) ; */
      return dirname;
    }

    char* tinyfd_open_folder_dialog(const char* title,       /* "" */
                                    const char* defaultPath) /* "" */
    {
      static char dirname[MAX_PATH];
      char lDialogString[MAX_PATH];
      FILE* lIn;
      char* p;
      char* lPointerInputBox;
      int lWasGraphicDialog = 0;
      int lWasXterm         = 0;
      dirname[0]            = '\0';

      if(tfd_quoteDetected(title))
        return open_folder_dialog("INVALID TITLE WITH QUOTES", defaultPath);
      if(tfd_quoteDetected(defaultPath))
        return open_folder_dialog(title, "INVALID DEFAULT_PATH WITH QUOTES");

      if(osascriptPresent())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "applescript");
          return (char*)1;
        }
        strcpy(lDialogString, "osascript ");
        if(!osx9orBetter())
          strcat(lDialogString, " -e 'tell application \"System Events\"' -e 'Activate'");
        strcat(lDialogString, " -e 'try' -e 'POSIX path of ( choose folder ");
        if(title && strlen(title))
        {
          strcat(lDialogString, "with prompt \"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\" ");
        }
        if(defaultPath && strlen(defaultPath))
        {
          strcat(lDialogString, "default location \"");
          strcat(lDialogString, defaultPath);
          strcat(lDialogString, "\" ");
        }
        strcat(lDialogString, ")' ");
        strcat(lDialogString, "-e 'on error number -128' ");
        strcat(lDialogString, "-e 'end try'");
        if(!osx9orBetter())
          strcat(lDialogString, " -e 'end tell'");
      }
      else if(tfd_kdialogPresent())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "kdialog");
          return (char*)1;
        }
        strcpy(lDialogString, "kdialog");
        if((tfd_kdialogPresent() == 2) && tfd_xpropPresent())
        {
          strcat(lDialogString, " --attach=$(xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)"); /* contribution: Paul Rouget */
        }
        strcat(lDialogString, " --getexistingdirectory ");

        if(defaultPath && strlen(defaultPath))
        {
          if(defaultPath[0] != '/')
          {
            strcat(lDialogString, "$PWD/");
          }
          strcat(lDialogString, "\"");
          strcat(lDialogString, defaultPath);
          strcat(lDialogString, "\"");
        }
        else
        {
          strcat(lDialogString, "$PWD/");
        }

        if(title && strlen(title))
        {
          strcat(lDialogString, " --title \"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\"");
        }
      }
      else if(tfd_zenityPresent() || tfd_matedialogPresent() || tfd_shellementaryPresent() || tfd_qarmaPresent())
      {
        if(tfd_zenityPresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "zenity");
            return (char*)1;
          }
          strcpy(lDialogString, "zenity");
          if((tfd_zenity3Present() >= 4) && !getenv("SSH_TTY") && tfd_xpropPresent())
          {
            strcat(lDialogString, " --attach=$(sleep .01;xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)"); /* contribution: Paul Rouget */
          }
        }
        else if(tfd_matedialogPresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "matedialog");
            return (char*)1;
          }
          strcpy(lDialogString, "matedialog");
        }
        else if(tfd_shellementaryPresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "shellementary");
            return (char*)1;
          }
          strcpy(lDialogString, "shellementary");
        }
        else
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "qarma");
            return (char*)1;
          }
          strcpy(lDialogString, "qarma");
          if(!getenv("SSH_TTY") && tfd_xpropPresent())
          {
            strcat(lDialogString, " --attach=$(xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)"); /* contribution: Paul Rouget */
          }
        }
        strcat(lDialogString, " --file-selection --directory");

        strcat(lDialogString, " --title=\"");
        if(title && strlen(title))
          strcat(lDialogString, title);
        strcat(lDialogString, "\"");

        if(defaultPath && strlen(defaultPath))
        {
          strcat(lDialogString, " --filename=\"");
          strcat(lDialogString, defaultPath);
          strcat(lDialogString, "\"");
        }
        if(tinyfd_silent)
          strcat(lDialogString, " 2>/dev/null ");
      }
      else if(tfd_yadPresent())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "yad");
          return (char*)1;
        }
        strcpy(lDialogString, "yad --file --directory");
        if(title && strlen(title))
        {
          strcat(lDialogString, " --title=\"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\"");
        }
        if(defaultPath && strlen(defaultPath))
        {
          strcat(lDialogString, " --filename=\"");
          strcat(lDialogString, defaultPath);
          strcat(lDialogString, "\"");
        }
        if(tinyfd_silent)
          strcat(lDialogString, " 2>/dev/null ");
      }
      else if(!xdialogPresent() && tkinter3Present())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "python3-tkinter");
          return (char*)1;
        }
        strcpy(lDialogString, gPython3Name);
        strcat(lDialogString, " -S -c \"import tkinter;from tkinter import filedialog;root=tkinter.Tk();root.withdraw();");
        strcat(lDialogString, "res=filedialog.askdirectory(");
        if(title && strlen(title))
        {
          strcat(lDialogString, "title='");
          strcat(lDialogString, title);
          strcat(lDialogString, "',");
        }
        if(defaultPath && strlen(defaultPath))
        {
          strcat(lDialogString, "initialdir='");
          strcat(lDialogString, defaultPath);
          strcat(lDialogString, "'");
        }
        strcat(lDialogString, ");\nif not isinstance(res, tuple):\n\tprint(res)\n\"");
      }
      else if(!xdialogPresent() && tkinter2Present())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "python2-tkinter");
          return (char*)1;
        }
        strcpy(lDialogString, "export PYTHONIOENCODING=utf-8;");
        strcat(lDialogString, gPython2Name);
        if(!isTerminalRunning() && tfd_isDarwin())
        {
          strcat(lDialogString, " -i"); /* for osx without console */
        }
        strcat(lDialogString, " -S -c \"import Tkinter,tkFileDialog;root=Tkinter.Tk();root.withdraw();");

        if(tfd_isDarwin())
        {
          strcat(lDialogString, "import os;os.system('''/usr/bin/osascript -e 'tell app \\\"Finder\\\" to set \
frontmost of process \\\"Python\\\" to true' ''');");
        }

        strcat(lDialogString, "print tkFileDialog.askdirectory(");
        if(title && strlen(title))
        {
          strcat(lDialogString, "title='");
          strcat(lDialogString, title);
          strcat(lDialogString, "',");
        }
        if(defaultPath && strlen(defaultPath))
        {
          strcat(lDialogString, "initialdir='");
          strcat(lDialogString, defaultPath);
          strcat(lDialogString, "'");
        }
        strcat(lDialogString, ")\"");
      }
      else if(xdialogPresent() || dialogName())
      {
        if(xdialogPresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "xdialog");
            return (char*)1;
          }
          lWasGraphicDialog = 1;
          strcpy(lDialogString, "(Xdialog ");
        }
        else if(isTerminalRunning())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "dialog");
            return (char*)0;
          }
          strcpy(lDialogString, "(dialog ");
        }
        else
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "dialog");
            return (char*)0;
          }
          lWasXterm = 1;
          strcpy(lDialogString, terminalName());
          strcat(lDialogString, "'(");
          strcat(lDialogString, dialogName());
          strcat(lDialogString, " ");
        }

        if(title && strlen(title))
        {
          strcat(lDialogString, "--title \"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\" ");
        }

        if(!xdialogPresent() && !gdialogPresent())
        {
          strcat(lDialogString, "--backtitle \"");
          strcat(lDialogString, "tab: focus | /: populate | spacebar: fill text field | ok: TEXT FIELD ONLY");
          strcat(lDialogString, "\" ");
        }

        strcat(lDialogString, "--dselect \"");
        if(defaultPath && strlen(defaultPath))
        {
          strcat(lDialogString, defaultPath);
          ensureFinalSlash(lDialogString);
        }
        else if(!isTerminalRunning() && !lWasGraphicDialog)
        {
          strcat(lDialogString, getenv("HOME"));
          strcat(lDialogString, "/");
        }
        else
        {
          strcat(lDialogString, "./");
        }

        if(lWasGraphicDialog)
        {
          strcat(lDialogString, "\" 0 60 ) 2>&1 ");
        }
        else
        {
          strcat(lDialogString, "\" 0 60  >/dev/tty) ");
          if(lWasXterm)
          {
            strcat(lDialogString, "2>/tmp/tinyfd.txt';cat /tmp/tinyfd.txt;rm /tmp/tinyfd.txt");
          }
          else
          {
            strcat(lDialogString, "2>&1 ; clear >/dev/tty");
          }
        }
      }
      else
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          return inputBox(title, NULL, NULL);
        }
        strcpy(dirname, "Select folder from ");
        strcat(dirname, getCurDir());
        lPointerInputBox = inputBox(NULL, NULL, NULL); /* obtain a pointer on the current content of tinyfd_inputBox */
        if(lPointerInputBox)
          strcpy(lDialogString, lPointerInputBox);     /* preserve the current content of tinyfd_inputBox */
        p = inputBox(title, dirname, "");
        if(p)
          strcpy(dirname, p);
        else
          dirname[0] = '\0';
        if(lPointerInputBox)
          strcpy(lPointerInputBox, lDialogString); /* restore its previous content to tinyfd_inputBox */
        p = dirname;

        if(!p || !strlen(p) || !dirExists(p))
        {
          return NULL;
        }
        return p;
      }
      if(tinyfd_verbose)
        printf("lDialogString: %s\n", lDialogString);
      if(!(lIn = popen(lDialogString, "r")))
      {
        return NULL;
      }
      while(fgets(dirname, sizeof(dirname), lIn) != NULL)
      {
      }
      pclose(lIn);
      if(dirname[strlen(dirname) - 1] == '\n')
      {
        dirname[strlen(dirname) - 1] = '\0';
      }
      /* printf( "dirname: %s\n" , dirname ) ; */
      if(!strlen(dirname) || !dirExists(dirname))
      {
        return NULL;
      }
      return dirname;
    }

    /* aDefaultRGB is used only if aDefaultHexRGB is absent */
    /* aDefaultRGB and aoResultRGB can be the same array */
    /* returns NULL on cancel */
    /* returns the hexcolor as a string "#FF0000" */
    /* aoResultRGB also contains the result */
    char* tinyfd_color_chooser(const char* title,                  /* NULL or "" */
                               const char* aDefaultHexRGB,         /* NULL or "#FF0000"*/
                               const unsigned char aDefaultRGB[3], /* { 0 , 255 , 255 } */
                               unsigned char aoResultRGB[3])       /* { 0 , 0 , 0 } */
    {
      static char lDefaultHexRGB[16];
      char dirname[128];

      char lTmp[128];
  #if !((defined(__cplusplus) && __cplusplus >= 201103L) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || defined(__clang__))
      char* lTmp2;
  #endif
      char lDialogString[MAX_PATH];
      unsigned char lDefaultRGB[3];
      char* p;
      char* lPointerInputBox;
      FILE* lIn;
      int i;
      int lWasZenity3   = 0;
      int lWasOsascript = 0;
      int lWasXdialog   = 0;
      dirname[0]        = '\0';

      if(tfd_quoteDetected(title))
        return color_chooser("INVALID TITLE WITH QUOTES", aDefaultHexRGB, aDefaultRGB, aoResultRGB);
      if(tfd_quoteDetected(aDefaultHexRGB))
        return color_chooser(title, "INVALID DEFAULT_HEX_RGB WITH QUOTES", aDefaultRGB, aoResultRGB);

      if(aDefaultHexRGB && (strlen(aDefaultHexRGB) == 7))
      {
        hex_to_rgb(aDefaultHexRGB, lDefaultRGB);
        strcpy(lDefaultHexRGB, aDefaultHexRGB);
      }
      else
      {
        lDefaultRGB[0] = aDefaultRGB[0];
        lDefaultRGB[1] = aDefaultRGB[1];
        lDefaultRGB[2] = aDefaultRGB[2];
        rgb_to_hex(aDefaultRGB, lDefaultHexRGB);
      }

      if(osascriptPresent())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "applescript");
          return (char*)1;
        }
        lWasOsascript = 1;
        strcpy(lDialogString, "osascript");

        if(!osx9orBetter())
        {
          strcat(lDialogString, " -e 'tell application \"System Events\"' -e 'Activate'");
          strcat(lDialogString, " -e 'try' -e 'set mycolor to choose color default color {");
        }
        else
        {
          strcat(lDialogString, " -e 'try' -e 'tell app (path to frontmost application as Unicode text) \
to set mycolor to choose color default color {");
        }

        sprintf(lTmp, "%d", 256 * lDefaultRGB[0]);
        strcat(lDialogString, lTmp);
        strcat(lDialogString, ",");
        sprintf(lTmp, "%d", 256 * lDefaultRGB[1]);
        strcat(lDialogString, lTmp);
        strcat(lDialogString, ",");
        sprintf(lTmp, "%d", 256 * lDefaultRGB[2]);
        strcat(lDialogString, lTmp);
        strcat(lDialogString, "}' ");
        strcat(lDialogString, "-e 'set mystring to ((item 1 of mycolor) div 256 as integer) as string' ");
        strcat(lDialogString, "-e 'repeat with i from 2 to the count of mycolor' ");
        strcat(lDialogString, "-e 'set mystring to mystring & \" \" & ((item i of mycolor) div 256 as integer) as string' ");
        strcat(lDialogString, "-e 'end repeat' ");
        strcat(lDialogString, "-e 'mystring' ");
        strcat(lDialogString, "-e 'on error number -128' ");
        strcat(lDialogString, "-e 'end try'");
        if(!osx9orBetter())
          strcat(lDialogString, " -e 'end tell'");
      }
      else if(tfd_kdialogPresent())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "kdialog");
          return (char*)1;
        }
        strcpy(lDialogString, "kdialog");
        if((tfd_kdialogPresent() == 2) && tfd_xpropPresent())
        {
          strcat(lDialogString, " --attach=$(xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)"); /* contribution: Paul Rouget */
        }
        sprintf(lDialogString + strlen(lDialogString), " --getcolor --default '%s'", lDefaultHexRGB);

        if(title && strlen(title))
        {
          strcat(lDialogString, " --title \"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\"");
        }
      }
      else if(tfd_zenity3Present() || tfd_matedialogPresent() || tfd_shellementaryPresent() || tfd_qarmaPresent())
      {
        lWasZenity3 = 1;
        if(tfd_zenity3Present())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "zenity3");
            return (char*)1;
          }
          strcpy(lDialogString, "zenity");
          if((tfd_zenity3Present() >= 4) && !getenv("SSH_TTY") && tfd_xpropPresent())
          {
            strcat(lDialogString, " --attach=$(sleep .01;xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)"); /* contribution: Paul Rouget */
          }
        }
        else if(tfd_matedialogPresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "matedialog");
            return (char*)1;
          }
          strcpy(lDialogString, "matedialog");
        }
        else if(tfd_shellementaryPresent())
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "shellementary");
            return (char*)1;
          }
          strcpy(lDialogString, "shellementary");
        }
        else
        {
          if(title && !strcmp(title, "tinyfd_query"))
          {
            strcpy(tinyfd_response, "qarma");
            return (char*)1;
          }
          strcpy(lDialogString, "qarma");
          if(!getenv("SSH_TTY") && tfd_xpropPresent())
          {
            strcat(lDialogString, " --attach=$(xprop -root 32x '\t$0' _NET_ACTIVE_WINDOW | cut -f 2)"); /* contribution: Paul Rouget */
          }
        }
        strcat(lDialogString, " --color-selection --show-palette");
        sprintf(lDialogString + strlen(lDialogString), " --color=%s", lDefaultHexRGB);

        strcat(lDialogString, " --title=\"");
        if(title && strlen(title))
          strcat(lDialogString, title);
        strcat(lDialogString, "\"");

        if(tinyfd_silent)
          strcat(lDialogString, " 2>/dev/null ");
      }
      else if(tfd_yadPresent())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "yad");
          return (char*)1;
        }
        strcpy(lDialogString, "yad --color");
        sprintf(lDialogString + strlen(lDialogString), " --init-color=%s", lDefaultHexRGB);
        if(title && strlen(title))
        {
          strcat(lDialogString, " --title=\"");
          strcat(lDialogString, title);
          strcat(lDialogString, "\"");
        }
        if(tinyfd_silent)
          strcat(lDialogString, " 2>/dev/null ");
      }
      else if(xdialogPresent())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "xdialog");
          return (char*)1;
        }
        lWasXdialog = 1;
        strcpy(lDialogString, "Xdialog --colorsel \"");
        if(title && strlen(title))
        {
          strcat(lDialogString, title);
        }
        strcat(lDialogString, "\" 0 60 ");
  #if(defined(__cplusplus) && __cplusplus >= 201103L) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || defined(__clang__)
        sprintf(lTmp, "%hhu %hhu %hhu", lDefaultRGB[0], lDefaultRGB[1], lDefaultRGB[2]);
  #else
        sprintf(lTmp, "%hu %hu %hu", lDefaultRGB[0], lDefaultRGB[1], lDefaultRGB[2]);
  #endif
        strcat(lDialogString, lTmp);
        strcat(lDialogString, " 2>&1");
      }
      else if(tkinter3Present())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "python3-tkinter");
          return (char*)1;
        }
        strcpy(lDialogString, gPython3Name);
        strcat(lDialogString, " -S -c \"import tkinter;from tkinter import colorchooser;root=tkinter.Tk();root.withdraw();");
        strcat(lDialogString, "res=colorchooser.askcolor(color='");
        strcat(lDialogString, lDefaultHexRGB);
        strcat(lDialogString, "'");

        if(title && strlen(title))
        {
          strcat(lDialogString, ",title='");
          strcat(lDialogString, title);
          strcat(lDialogString, "'");
        }
        strcat(lDialogString, ");\
\nif res[1] is not None:\n\tprint(res[1])\"");
      }
      else if(tkinter2Present())
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          strcpy(tinyfd_response, "python2-tkinter");
          return (char*)1;
        }
        strcpy(lDialogString, "export PYTHONIOENCODING=utf-8;");
        strcat(lDialogString, gPython2Name);
        if(!isTerminalRunning() && tfd_isDarwin())
        {
          strcat(lDialogString, " -i"); /* for osx without console */
        }

        strcat(lDialogString, " -S -c \"import Tkinter,tkColorChooser;root=Tkinter.Tk();root.withdraw();");

        if(tfd_isDarwin())
        {
          strcat(lDialogString, "import os;os.system('''osascript -e 'tell app \\\"Finder\\\" to set \
frontmost of process \\\"Python\\\" to true' ''');");
        }

        strcat(lDialogString, "res=tkColorChooser.askcolor(color='");
        strcat(lDialogString, lDefaultHexRGB);
        strcat(lDialogString, "'");

        if(title && strlen(title))
        {
          strcat(lDialogString, ",title='");
          strcat(lDialogString, title);
          strcat(lDialogString, "'");
        }
        strcat(lDialogString, ");\
\nif res[1] is not None:\n\tprint res[1]\"");
      }
      else
      {
        if(title && !strcmp(title, "tinyfd_query"))
        {
          return inputBox(title, NULL, NULL);
        }
        lPointerInputBox = inputBox(NULL, NULL, NULL); /* obtain a pointer on the current content of tinyfd_inputBox */
        if(lPointerInputBox)
          strcpy(lDialogString, lPointerInputBox);     /* preserve the current content of tinyfd_inputBox */
        p = inputBox(title, "Enter hex rgb color (i.e. #f5ca20)", lDefaultHexRGB);

        if(!p || (strlen(p) != 7) || (p[0] != '#'))
        {
          return NULL;
        }
        for(i = 1; i < 7; i++)
        {
          if(!isxdigit((int)p[i]))
          {
            return NULL;
          }
        }
        hex_to_rgb(p, aoResultRGB);
        strcpy(lDefaultHexRGB, p);
        if(lPointerInputBox)
          strcpy(lPointerInputBox, lDialogString); /* restore its previous content to tinyfd_inputBox */
        return lDefaultHexRGB;
      }

      if(tinyfd_verbose)
        printf("lDialogString: %s\n", lDialogString);
      if(!(lIn = popen(lDialogString, "r")))
      {
        return NULL;
      }
      while(fgets(dirname, sizeof(dirname), lIn) != NULL)
      {
      }
      pclose(lIn);
      if(!strlen(dirname))
      {
        return NULL;
      }
      /* printf( "len Buff: %lu\n" , strlen(dirname) ) ; */
      /* printf( "dirname0: %s\n" , dirname ) ; */
      if(dirname[strlen(dirname) - 1] == '\n')
      {
        dirname[strlen(dirname) - 1] = '\0';
      }

      if(lWasZenity3)
      {
        if(dirname[0] == '#')
        {
          if(strlen(dirname) > 7)
          {
            dirname[3] = dirname[5];
            dirname[4] = dirname[6];
            dirname[5] = dirname[9];
            dirname[6] = dirname[10];
            dirname[7] = '\0';
          }
          hex_to_rgb(dirname, aoResultRGB);
        }
        else if(dirname[3] == '(')
        {
  #if(defined(__cplusplus) && __cplusplus >= 201103L) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || defined(__clang__)
          sscanf(dirname, "rgb(%hhu,%hhu,%hhu", &aoResultRGB[0], &aoResultRGB[1], &aoResultRGB[2]);
  #else
          aoResultRGB[0] = strtol(dirname + 4, &lTmp2, 10);
          aoResultRGB[1] = strtol(lTmp2 + 1, &lTmp2, 10);
          aoResultRGB[2] = strtol(lTmp2 + 1, NULL, 10);
  #endif
          rgb_to_hex(aoResultRGB, dirname);
        }
        else if(dirname[4] == '(')
        {
  #if(defined(__cplusplus) && __cplusplus >= 201103L) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || defined(__clang__)
          sscanf(dirname, "rgba(%hhu,%hhu,%hhu", &aoResultRGB[0], &aoResultRGB[1], &aoResultRGB[2]);
  #else
          aoResultRGB[0] = strtol(dirname + 5, &lTmp2, 10);
          aoResultRGB[1] = strtol(lTmp2 + 1, &lTmp2, 10);
          aoResultRGB[2] = strtol(lTmp2 + 1, NULL, 10);
  #endif
          rgb_to_hex(aoResultRGB, dirname);
        }
      }
      else if(lWasOsascript || lWasXdialog)
      {
        /* printf( "dirname: %s\n" , dirname ) ; */
  #if(defined(__cplusplus) && __cplusplus >= 201103L) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || defined(__clang__)
        sscanf(dirname, "%hhu %hhu %hhu", &aoResultRGB[0], &aoResultRGB[1], &aoResultRGB[2]);
  #else
        aoResultRGB[0] = strtol(dirname, &lTmp2, 10);
        aoResultRGB[1] = strtol(lTmp2 + 1, &lTmp2, 10);
        aoResultRGB[2] = strtol(lTmp2 + 1, NULL, 10);
  #endif
        rgb_to_hex(aoResultRGB, dirname);
      }
      else
      {
        hex_to_rgb(dirname, aoResultRGB);
      }
      /* printf("%d %d %d\n", aoResultRGB[0],aoResultRGB[1],aoResultRGB[2]); */
      /* printf( "dirname: %s\n" , dirname ) ; */

      strcpy(lDefaultHexRGB, dirname);
      return lDefaultHexRGB;
    }

#endif /* _WIN32 */

  }    // namespace dialog

} // namespace rex
