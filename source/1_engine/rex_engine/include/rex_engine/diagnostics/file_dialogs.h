/* If you are using a C++ compiler to compile tinyfiledialogs.c (maybe renamed with an extension ".cpp")
then comment out << extern "C" >> bellow in this header file) */

/*_________
 /         \ tinyfiledialogs.h v3.13.2 [May 31, 2023] zlib licence
 |tiny file| Unique header file created [November 9, 2014]
 | dialogs | Copyright (c) 2014 - 2023 Guillaume Vareille http://ysengrin.com
 \____  ___/ http://tinyfiledialogs.sourceforge.net
      \|     git clone http://git.code.sf.net/p/tinyfiledialogs/code tinyfd
 ____________________________________________
|                                            |
|   email: tinyfiledialogs at ysengrin.com   |
|____________________________________________|
 ________________________________________________________________________________
|  ____________________________________________________________________________  |
| |                                                                            | |
| |  - in tinyfiledialogs, char is UTF-8 by default (since v3.6)               | |
| |                                                                            | |
| | on windows:                                                                | |
| |  - for UTF-16, use the wchar_t functions at the bottom of the header file  | |
| |  - _wfopen() requires wchar_t                                              | |
| |                                                                            | |
| |  - but fopen() expects MBCS (not UTF-8)                                    | |
| |  - if you want char to be MBCS: set tinyfd_winUtf8 to 0                    | |
| |                                                                            | |
| |  - alternatively, tinyfiledialogs provides                                 | |
| |                        functions to convert between UTF-8, UTF-16 and MBCS | |
| |____________________________________________________________________________| |
|________________________________________________________________________________|

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
*/

// We've changed tinyfiledialogs to make it more performant

#ifndef TINYFILEDIALOGS_H
#define TINYFILEDIALOGS_H

#ifdef	__cplusplus
/* if tinydialogs.c is compiled as C++ code rather than C code, you may need to comment this out
				and the corresponding closing bracket near the end of this file. */
//extern "C" {
#endif

#include "rex_std/bonus/math/color.h"

namespace rex
{
	namespace dialog
	{
		void beep(void);

		enum class IconType
		{
			Info,
			Warning,
			Error,
			Question
		};

		enum class DialogType
		{
			Ok,
			OkCancel,
			YesNo,
			YesNoCancel
		};

		enum class InputType
		{
			Default,
			Password
		};

		enum class DefaultButton
		{
			Cancel,
			Yes,
			No
		};

		int notifyPopup(
			rsl::string_view aTitle, /* NULL or "" */
			rsl::string_view aMessage, /* NULL or "" may contain \n \t */
			rsl::string_view aTip, /* NULL or "" may contain \n \t */
			IconType aIconType); /* "info" "warning" "error" */
				/* return has only meaning for tinyfd_query */

		int messageBox(
			rsl::string_view aTitle, /* NULL or "" */
			rsl::string_view aMessage, /* NULL or "" may contain \n \t */
			DialogType aDialogType, /* "ok" "okcancel" "yesno" "yesnocancel" */
			IconType aIconType, /* "info" "warning" "error" "question" */
			DefaultButton aDefaultButton);
		/* 0 for cancel/no , 1 for ok/yes , 2 for no in yesnocancel */

		rsl::medium_stack_string saveFileDialog(
			rsl::string_view aTitle, /* NULL or "" */
			rsl::string_view aDefaultPathAndFile, /* NULL or "" */
			rsl::initializer_list<rsl::string_view> aFilterPatterns, /* NULL or char const * lFilterPatterns[1]={"*.txt"} */
			rsl::string_view aSingleFilterDescription); /* NULL or "text files" */
				/* returns NULL on cancel */

		rsl::vector<rsl::string> openFileDialog(
			rsl::string_view aTitle, /* NULL or "" */
			rsl::string_view aDefaultPathAndFile, /* NULL or "" */
			rsl::initializer_list<rsl::string_view> aFilterPatterns, /* NULL or char const * lFilterPatterns[2]={"*.png","*.jpg"}; */
			rsl::string_view aSingleFilterDescription, /* NULL or "image files" */
			bool aAllowMultipleSelects); /* 0 or 1 */
				/* in case of multiple files, the separator is | */
				/* returns NULL on cancel */

		rsl::big_stack_string selectFolderDialog(
			rsl::string_view aTitle, /* NULL or "" */
			rsl::string_view aDefaultPath); /* NULL or "" */
				/* returns NULL on cancel */

		rsl::Rgb colorChooser(
			rsl::string_view aTitle, /* NULL or "" */
			rsl::Rgb aDefaultRGB); /* unsigned char lDefaultRGB[3] = { 0 , 128 , 255 }; */
				/* aDefaultRGB is used only if aDefaultHexRGB is absent */
				/* aDefaultRGB and aoResultRGB can be the same array */
				/* returns NULL on cancel */
				/* returns the hexcolor as a string "#FF0000" */
				/* aoResultRGB also contains the result */
	}

}
#ifdef	__cplusplus
//} /*extern "C"*/
#endif

#endif /* TINYFILEDIALOGS_H */

/*
 ________________________________________________________________________________
|  ____________________________________________________________________________  |
| |                                                                            | |
| | on windows:                                                                | |
| |  - for UTF-16, use the wchar_t functions at the bottom of the header file  | |
| |  - _wfopen() requires wchar_t                                              | |
| |                                                                            | |
| |  - in tinyfiledialogs, char is UTF-8 by default (since v3.6)               | |
| |  - but fopen() expects MBCS (not UTF-8)                                    | |
| |  - if you want char to be MBCS: set tinyfd_winUtf8 to 0                    | |
| |                                                                            | |
| |  - alternatively, tinyfiledialogs provides                                 | |
| |                        functions to convert between UTF-8, UTF-16 and MBCS | |
| |____________________________________________________________________________| |
|________________________________________________________________________________|

- This is not for ios nor android (it works in termux though).
- The files can be renamed with extension ".cpp" as the code is 100% compatible C C++
  (just comment out << extern "C" >> in the header file)
- Windows is fully supported from XP to 10 (maybe even older versions)
- C# & LUA via dll, see files in the folder EXTRAS
- OSX supported from 10.4 to latest (maybe even older versions)
- Do not use " and ' as the dialogs will be displayed with a warning
  instead of the title, message, etc...
- There's one file filter only, it may contain several patterns.
- If no filter description is provided,
  the list of patterns will become the description.
- On windows link against Comdlg32.lib and Ole32.lib
  (on windows the no linking claim is a lie)
- On unix: it tries command line calls, so no such need (NO LINKING).
- On unix you need one of the following:
  applescript, kdialog, zenity, matedialog, shellementary, qarma, yad,
  python (2 or 3)/tkinter/python-dbus (optional), Xdialog
  or curses dialogs (opens terminal if running without console).
- One of those is already included on most (if not all) desktops.
- In the absence of those it will use gdialog, gxmessage or whiptail
  with a textinputbox. If nothing is found, it switches to basic console input,
  it opens a console if needed (requires xterm + bash).
- for curses dialogs you must set tinyfd_allowCursesDialogs=1
- You can query the type of dialog that will be used (pass "tinyfd_query" as aTitle)
- String memory is preallocated statically for all the returned values.
- File and path names are tested before return, they should be valid.
- tinyfd_forceConsole=1; at run time, forces dialogs into console mode.
- On windows, console mode only make sense for console applications.
- On windows, console mode is not implemented for wchar_T UTF-16.
- Mutiple selects are not possible in console mode.
- The package dialog must be installed to run in curses dialogs in console mode.
  It is already installed on most unix systems.
- On osx, the package dialog can be installed via
  http://macappstore.org/dialog or http://macports.org
- On windows, for curses dialogs console mode,
  dialog.exe should be copied somewhere on your executable path.
  It can be found at the bottom of the following page:
  http://andrear.altervista.org/home/cdialog.php
*/
