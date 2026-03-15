#include "rex_windows/event_system/win_event_handler.h"

#include "rex_engine/app/core_window.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/event_system/event.h"
#include "rex_engine/event_system/event_system.h"
#include "rex_engine/platform/win/diagnostics/win_call.h"
#include "rex_std/bonus/utility.h"
#include "rex_windows/diagnostics/log.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/log.h"
#include "rex_engine/event_system/events/window/window_close.h"
#include "rex_engine/event_system/events/window/window_activated.h"
#include "rex_engine/event_system/events/window/window_deactivated.h"
#include "rex_engine/event_system/events/window/window_resize.h"
#include "rex_engine/event_system/events/window/window_start_resize.h"
#include "rex_engine/event_system/events/window/window_end_resize.h"

#include "rex_engine/input/key_code.h"

#include "rex_engine/event_system/events/input/char_down.h"
#include "rex_engine/event_system/events/input/key_down.h"
#include "rex_engine/event_system/events/input/key_up.h"

#include "rex_engine/event_system/events/input/os_key_up.h"
#include "rex_engine/event_system/events/input/os_key_down.h"

#define NOMINMAX
#include <Windows.h>

#include "rex_engine/platform/win/imgui/win_imgui.h"

// NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)

namespace rex
{
  namespace win
  {
    DEFINE_LOG_CATEGORY(LogWinEventHandler);

    namespace internal
    {
      union KeyMetaData
      {
        KeyMetaData(card32 data)
          : data(data)
        {}
        KeyMetaData(LPARAM data)
          : data((card32)data)
        {}

        card32 data;

        struct Unpacked
        {
          unsigned repeat_count : 16;
          unsigned scan_code : 8;
          unsigned extended : 1;
          unsigned reserved : 4; // do not use
          unsigned context : 1;
          unsigned previous_state : 1;
          unsigned transition_state : 1;
        } unpacked;
      };

      void log_unkown_vk(WPARAM vk, KeyMetaData meta_data)
      {
        BYTE keyboard_state[256];
        WIN_CALL(GetKeyboardState(keyboard_state));
        unsigned short key_name[10] = {              };
        ToAscii((UINT)vk, meta_data.unpacked.scan_code, keyboard_state, key_name, 0);
        int8 c = (int8)key_name[0];

        if (c)
        {
          REX_ERROR(LogWinEventHandler, "Unhandled virtual key: {} -> {}", vk, (const char*)key_name);
        }
        else
        {
          REX_ERROR(LogWinEventHandler, "Unhandled virtual key: {} -> No Key Name", vk);
        }
      }

      // defining these below as they're long functions
      KeyCode keycode_from_vk(WPARAM vk, LPARAM key_state);
      rsl::string_view window_event_to_string(card32 event);
    }

    //-------------------------------------------------------------------------
    EventHandler::EventHandler(IWindow* wnd)
        : m_wnd(wnd)
    {

    }

    //-------------------------------------------------------------------------
    LResult EventHandler::on_event(Hwnd hwnd, card32 msg, WParam wparam, LParam lparam) // NOLINT (readability-convert-member-functions-to-static,-warnings-as-errors)
    {
      REX_ASSERT_X(m_wnd != nullptr, "Window was not given to the Window Event Handler");

      if (imgui_main_window_update((HWND)hwnd, msg, wparam, lparam))
        return true;

      // Sometimes Windows set error states between messages
      // because these aren't our fault, we'll ignore those
      // to make sure our messages are successful
      const DWORD last_windows_error = GetLastError();
      rex::win::clear_win_errors();

      const rsl::scopeguard reset_win_error_scopeguard([=]() { SetLastError(last_windows_error); });

      // As WM_ENTERSIZEMOVE and WM_EXITSIZEMOVE can be called on resizing as well as on window moving
      // we need to track if we're actually resizing or moving
      static bool was_resizing = false;

      switch(msg)
      {
        case WM_CLOSE: REX_WARN(LogWindows, "Verify if the user really wants to close"); break;
        case WM_DESTROY:
          PostQuitMessage(0);
          event_system::instance()->enqueue_event(rex::WindowClose());
          return 0;

        case WM_ACTIVATE:
          if(LOWORD(wparam) == WA_INACTIVE)
          {
            event_system::instance()->enqueue_event(rex::WindowDeactivated());
          }
          else
          {
            event_system::instance()->enqueue_event(rex::WindowActivated());
          }
          return 0;

        case WM_ENTERSIZEMOVE:
          event_system::instance()->enqueue_event(rex::WindowStartResize());
          return 0;

        case WM_EXITSIZEMOVE:
          if (was_resizing)
          {
            event_system::instance()->enqueue_event(rex::WindowEndResize());
          }
          was_resizing = false;
          return 0;

        case WM_MENUCHAR:
          // The WM_MENUCHAR message is sent when a menu is active and the user presses
          // a key that does not correspond to any mnemonic or accelerator key.
          //
          // In other words, don't beep when we alt-enter.
          return MAKELRESULT(0, MNC_CLOSE);

        case WM_GETMINMAXINFO:
          // We catch this message to prevent the window from becoming too small.
          ((MINMAXINFO*)lparam)->ptMinTrackSize.x = m_wnd->min_width();  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast, google-readability-casting, performance-no-int-to-ptr)
          ((MINMAXINFO*)lparam)->ptMinTrackSize.y = m_wnd->min_height(); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast, google-readability-casting, performance-no-int-to-ptr)
          return 0;
        case WM_SIZING:
          was_resizing = true;
          break;
        case WM_SIZE:
        {
          if (was_resizing)
          {
            break;
          }

          s32 width = LOWORD(lparam);
          s32 height = HIWORD(lparam);
          WindowResizeType resize_type = WindowResizeType::Invalid;

          if (wparam == SIZE_MINIMIZED)
          {
            resize_type = WindowResizeType::Minimized;
          }
          else if (wparam == SIZE_MAXIMIZED)
          {
            resize_type = WindowResizeType::Maximized;
          }
          else if (wparam == SIZE_RESTORED)
          {
            resize_type = WindowResizeType::Restored;
          }
          event_system::instance()->enqueue_event(WindowResize(width, height, resize_type));
          return 0;
        }
        case WM_CHAR:
          event_system::instance()->enqueue_event(CharDown((char8)wparam));
          break;

        case WM_KEYDOWN: 
          event_system::instance()->fire_event(OSKeyDown(internal::keycode_from_vk(wparam, lparam)));
          break;
        case WM_KEYUP:
          event_system::instance()->fire_event(
            OSKeyUp(internal::keycode_from_vk(wparam, lparam)));
          break;
        case WM_SHOWWINDOW:
          was_resizing = was_resizing;
          break;
        default:
          // Nothing to implement
          break;
      }
      return DefWindowProc(static_cast<HWND>(hwnd), msg, wparam, lparam);
    }

    namespace internal
    {
      KeyCode keycode_from_vk(WPARAM vk, LPARAM key_state)
      {
        KeyMetaData meta_data((s32)key_state);

        switch (vk)
        {
        case VK_LBUTTON:					REX_ASSERT("Got mouse key where keyboard key was expected"); break;
        case VK_RBUTTON:					REX_ASSERT("Got mouse key where keyboard key was expected"); break;
        case VK_CANCEL:						REX_ASSERT("Got mouse key where keyboard key was expected"); break;
        case VK_MBUTTON:					REX_ASSERT("Got mouse key where keyboard key was expected"); break;
        case VK_XBUTTON1:					REX_ASSERT("Got mouse key where keyboard key was expected"); break;
        case VK_XBUTTON2:					REX_ASSERT("Got mouse key where keyboard key was expected"); break;

        case VK_BACK:						return KeyCode::BackSpace;
        case VK_TAB:						return KeyCode::Tab;
        case VK_CLEAR:					return KeyCode::Clear;
        case VK_RETURN:         return KeyCode::Return;
        case VK_SHIFT:          return KeyCode::Shift;
        case VK_CONTROL:        return KeyCode::Ctrl;
        case VK_MENU:           return KeyCode::Menu;
        case VK_PAUSE:          return KeyCode::Pause;
        case VK_CAPITAL:        return KeyCode::Capital;
          //case VK_KANA:                        						              							
          //case VK_HANGUL:                        					              							
          //case VK_JUNJA:                        					              							
          //case VK_FINAL:                        					              							
          //case VK_HANJA:                        					              							
          //case VK_KANJI:                        					              							
        case VK_ESCAPE:         return KeyCode::Escape;
          //case VK_CONVERT:                        					             							
          //case VK_NONCONVERT:                        				          								
          //case VK_ACCEPT:       
          //case VK_MODECHANGE:																				
        case VK_SPACE:          return KeyCode::Space;
          //case VK_PRIOR:        
          //case VK_NEXT:         
        case VK_END:            return          KeyCode::End;
        case VK_HOME:           return          KeyCode::Home;
        case VK_LEFT:           return          KeyCode::LeftArrow;
        case VK_UP:             return          KeyCode::UpArrow;
        case VK_RIGHT:          return          KeyCode::RightArrow;
        case VK_DOWN:           return          KeyCode::DownArrow;
        case VK_SELECT:         return          KeyCode::Select;
        case VK_PRINT:          return          KeyCode::Print;
          //case VK_EXECUTE:      
        case VK_SNAPSHOT:       return KeyCode::Snapshot;
        case VK_INSERT:         return KeyCode::Insert;
        case VK_DELETE:         return KeyCode::Delete;
        case VK_HELP:           return KeyCode::Help;
        case '0':								return KeyCode::Zero;
        case '1':               return KeyCode::One;
        case '2':               return KeyCode::Two;
        case '3':               return KeyCode::Three;
        case '4':               return KeyCode::Four;
        case '5':               return KeyCode::Five;
        case '6':               return KeyCode::Six;
        case '7':               return KeyCode::Seven;
        case '8':               return KeyCode::Eight;
        case '9':               return KeyCode::Nine;
        case 'A':								return KeyCode::A;
        case 'B':               return KeyCode::B;
        case 'C':               return KeyCode::C;
        case 'D':               return KeyCode::D;
        case 'E':               return KeyCode::E;
        case 'F':               return KeyCode::F;
        case 'G':               return KeyCode::G;
        case 'H':               return KeyCode::H;
        case 'I':               return KeyCode::I;
        case 'J':               return KeyCode::J;
        case 'K':               return KeyCode::K;
        case 'L':               return KeyCode::L;
        case 'M':               return KeyCode::M;
        case 'N':               return KeyCode::N;
        case 'O':               return KeyCode::O;
        case 'P':               return KeyCode::P;
        case 'Q':               return KeyCode::Q;
        case 'R':               return KeyCode::R;
        case 'S':               return KeyCode::S;
        case 'T':               return KeyCode::T;
        case 'U':               return KeyCode::U;
        case 'V':               return KeyCode::V;
        case 'W':               return KeyCode::W;
        case 'X':               return KeyCode::X;
        case 'Y':               return KeyCode::Y;
        case 'Z':               return KeyCode::Z;
        case VK_LWIN:           return KeyCode::LeftPlatformBtn;
        case VK_RWIN:           return KeyCode::RightPlatformBtn;
        case VK_APPS:           return KeyCode::Apps;
        case VK_SLEEP:          return KeyCode::Sleep;
        case VK_NUMPAD0:        return KeyCode::NumPad0;
        case VK_NUMPAD1:        return KeyCode::NumPad1;
        case VK_NUMPAD2:        return KeyCode::NumPad2;
        case VK_NUMPAD3:        return KeyCode::NumPad3;
        case VK_NUMPAD4:        return KeyCode::NumPad4;
        case VK_NUMPAD5:        return KeyCode::NumPad5;
        case VK_NUMPAD6:        return KeyCode::NumPad6;
        case VK_NUMPAD7:        return KeyCode::NumPad7;
        case VK_NUMPAD8:        return KeyCode::NumPad8;
        case VK_NUMPAD9:        return KeyCode::NumPad9;
        case VK_MULTIPLY:       return KeyCode::Star;
        case VK_ADD:            return KeyCode::Plus;
        case VK_SEPARATOR:      return KeyCode::Minus;
        case VK_SUBTRACT:       return KeyCode::Minus;
        case VK_DECIMAL:        return KeyCode::Dot;
        case VK_DIVIDE:         return KeyCode::Slash;
        case VK_F1:							return KeyCode::F1;
        case VK_F2:             return KeyCode::F2;
        case VK_F3:             return KeyCode::F3;
        case VK_F4:             return KeyCode::F4;
        case VK_F5:             return KeyCode::F5;
        case VK_F6:             return KeyCode::F6;
        case VK_F7:             return KeyCode::F7;
        case VK_F8:             return KeyCode::F8;
        case VK_F9:             return KeyCode::F9;
        case VK_F10:            return KeyCode::F10;
        case VK_F11:            return KeyCode::F11;
        case VK_F12:            return KeyCode::F12;
        case VK_F13:            return KeyCode::F13;
        case VK_F14:            return KeyCode::F14;
        case VK_F15:            return KeyCode::F15;
        case VK_F16:            return KeyCode::F16;
        case VK_F17:            return KeyCode::F17;
        case VK_F18:            return KeyCode::F18;
        case VK_F19:            return KeyCode::F19;
        case VK_F20:            return KeyCode::F20;
        case VK_F21:            return KeyCode::F21;
        case VK_F22:            return KeyCode::F22;
        case VK_F23:            return KeyCode::F23;
        case VK_F24:            return KeyCode::F24;
        case VK_NUMLOCK:        return KeyCode::NumLock;
        case VK_SCROLL:         return KeyCode::Scroll;
          //case VK_LSHIFT:             
          //case VK_RSHIFT:             
          //case VK_LCONTROL:           
          //case VK_RCONTROL:           
          //case VK_LMENU:              
          //case VK_RMENU:              
          //case VK_BROWSER_BACK:       
          //case VK_BROWSER_FORWARD:    
          //case VK_BROWSER_REFRESH:    
          //case VK_BROWSER_STOP:       
          //case VK_BROWSER_SEARCH:     
          //case VK_BROWSER_FAVORITES:  
          //case VK_BROWSER_HOME:       
          //case VK_VOLUME_MUTE:        
          //case VK_VOLUME_DOWN:        
          //case VK_VOLUME_UP:          
          //case VK_MEDIA_NEXT_TRACK:   
          //case VK_MEDIA_PREV_TRACK:   
          //case VK_MEDIA_STOP:         
          //case VK_MEDIA_PLAY_PAUSE:   
          //case VK_LAUNCH_MAIL:        
          //case VK_LAUNCH_MEDIA_SELECT:
          //case VK_LAUNCH_APP1:        
          //case VK_LAUNCH_APP2:        
        case VK_OEM_1:										return KeyCode::SemiColon;
        case VK_OEM_PLUS:									return KeyCode::Plus;
        case VK_OEM_COMMA:								return KeyCode::Comma;
        case VK_OEM_MINUS:								return KeyCode::Minus;
        case VK_OEM_PERIOD:								return KeyCode::Dot;
        case VK_OEM_2:										return KeyCode::Slash;
        case VK_OEM_3:										return KeyCode::Grave;
        case VK_OEM_4:										return KeyCode::OpenSquare;
        case VK_OEM_5:										return KeyCode::BackSlash;
        case VK_OEM_6:										return KeyCode::CloseSquare;
        case VK_OEM_7:										return KeyCode::SingleQuote;
          //case VK_OEM_8:									
        case VK_OEM_102:									return KeyCode::GreaterThan;
          //case VK_PROCESSKEY:             
          //case VK_PACKET:                 
          //case VK_ATTN:                   
          //case VK_CRSEL:                  
          //case VK_EXSEL:                  
          //case VK_EREOF:                  
          //case VK_PLAY:                   
          //case VK_ZOOM:                   
          //case VK_NONAME:                 
          //case VK_PA1:                    
          //case VK_OEM_CLEAR:              

        }

        log_unkown_vk(vk, meta_data);
        return KeyCode::Unknown;
      }

      rsl::string_view window_event_to_string(card32 event)
      {
        switch (event)
        {
        case WM_NULL:                            return "WM_NULL";
        case WM_CREATE:                          return "WM_CREATE";
        case WM_DESTROY:                         return "WM_DESTROY";
        case WM_MOVE:                            return "WM_MOVE";
        case WM_SIZE:                            return "WM_SIZE";
        case WM_ACTIVATE:                        return "WM_ACTIVATE";
        case WM_SETFOCUS:                        return "WM_SETFOCUS";
        case WM_KILLFOCUS:                       return "WM_KILLFOCUS";
        case WM_ENABLE:                          return "WM_ENABLE";
        case WM_SETREDRAW:                       return "WM_SETREDRAW";
        case WM_SETTEXT:                         return "WM_SETTEXT";
        case WM_GETTEXT:                         return "WM_GETTEXT";
        case WM_GETTEXTLENGTH:                   return "WM_GETTEXTLENGTH";
        case WM_PAINT:                           return "WM_PAINT";
        case WM_CLOSE:                           return "WM_CLOSE";
        case WM_QUERYENDSESSION:                 return "WM_QUERYENDSESSION";
        case WM_QUERYOPEN:                       return "WM_QUERYOPEN";
        case WM_ENDSESSION:                      return "WM_ENDSESSION";
        case WM_QUIT:                            return "WM_QUIT";
        case WM_ERASEBKGND:                      return "WM_ERASEBKGND";
        case WM_SYSCOLORCHANGE:                  return "WM_SYSCOLORCHANGE";
        case WM_SHOWWINDOW:                      return "WM_SHOWWINDOW";
        case WM_WININICHANGE:                    return "WM_WININICHANGE";
        //case WM_SETTINGCHANGE:                 return "WM_SETTINGCHANGE"; // duplicate
        case WM_DEVMODECHANGE:                   return "WM_DEVMODECHANGE";
        case WM_ACTIVATEAPP:                     return "WM_ACTIVATEAPP";
        case WM_FONTCHANGE:                      return "WM_FONTCHANGE";
        case WM_TIMECHANGE:                      return "WM_TIMECHANGE";
        case WM_CANCELMODE:                      return "WM_CANCELMODE";
        case WM_SETCURSOR:                       return "WM_SETCURSOR";
        case WM_MOUSEACTIVATE:                   return "WM_MOUSEACTIVATE";
        case WM_CHILDACTIVATE:                   return "WM_CHILDACTIVATE";
        case WM_QUEUESYNC:                       return "WM_QUEUESYNC";
        case WM_GETMINMAXINFO:                   return "WM_GETMINMAXINFO";
        case WM_PAINTICON:                       return "WM_PAINTICON";
        case WM_ICONERASEBKGND:                  return "WM_ICONERASEBKGND";
        case WM_NEXTDLGCTL:                      return "WM_NEXTDLGCTL";
        case WM_SPOOLERSTATUS:                   return "WM_SPOOLERSTATUS";
        case WM_DRAWITEM:                        return "WM_DRAWITEM";
        case WM_MEASUREITEM:                     return "WM_MEASUREITEM";
        case WM_DELETEITEM:                      return "WM_DELETEITEM";
        case WM_VKEYTOITEM:                      return "WM_VKEYTOITEM";
        case WM_CHARTOITEM:                      return "WM_CHARTOITEM";
        case WM_SETFONT:                         return "WM_SETFONT";
        case WM_GETFONT:                         return "WM_GETFONT";
        case WM_SETHOTKEY:                       return "WM_SETHOTKEY";
        case WM_GETHOTKEY:                       return "WM_GETHOTKEY";
        case WM_QUERYDRAGICON:                   return "WM_QUERYDRAGICON";
        case WM_COMPAREITEM:                     return "WM_COMPAREITEM";
        case WM_GETOBJECT:                       return "WM_GETOBJECT";
        case WM_COMPACTING:                      return "WM_COMPACTING";
        case WM_COMMNOTIFY:                      return "WM_COMMNOTIFY";
        case WM_WINDOWPOSCHANGING:               return "WM_WINDOWPOSCHANGING";
        case WM_WINDOWPOSCHANGED:                return "WM_WINDOWPOSCHANGED";
        case WM_POWER:                           return "WM_POWER";
        case WM_COPYDATA:                        return "WM_COPYDATA";
        case WM_CANCELJOURNAL:                   return "WM_CANCELJOURNAL";
        case WM_NOTIFY:                          return "WM_NOTIFY";
        case WM_INPUTLANGCHANGEREQUEST:          return "WM_INPUTLANGCHANGEREQUEST";
        case WM_INPUTLANGCHANGE:                 return "WM_INPUTLANGCHANGE";
        case WM_TCARD:                           return "WM_TCARD";
        case WM_HELP:                            return "WM_HELP";
        case WM_USERCHANGED:                     return "WM_USERCHANGED";
        case WM_NOTIFYFORMAT:                    return "WM_NOTIFYFORMAT";
        case WM_CONTEXTMENU:                     return "WM_CONTEXTMENU";
        case WM_STYLECHANGING:                   return "WM_STYLECHANGING";
        case WM_STYLECHANGED:                    return "WM_STYLECHANGED";
        case WM_DISPLAYCHANGE:                   return "WM_DISPLAYCHANGE";
        case WM_GETICON:                         return "WM_GETICON";
        case WM_SETICON:                         return "WM_SETICON";
        case WM_NCCREATE:                        return "WM_NCCREATE";
        case WM_NCDESTROY:                       return "WM_NCDESTROY";
        case WM_NCCALCSIZE:                      return "WM_NCCALCSIZE";
        case WM_NCHITTEST:                       return "WM_NCHITTEST";
        case WM_NCPAINT:                         return "WM_NCPAINT";
        case WM_NCACTIVATE:                      return "WM_NCACTIVATE";
        case WM_GETDLGCODE:                      return "WM_GETDLGCODE";
        case WM_SYNCPAINT:                       return "WM_SYNCPAINT";
        case WM_NCMOUSEMOVE:                     return "WM_NCMOUSEMOVE";
        case WM_NCLBUTTONDOWN:                   return "WM_NCLBUTTONDOWN";
        case WM_NCLBUTTONUP:                     return "WM_NCLBUTTONUP";
        case WM_NCLBUTTONDBLCLK:                 return "WM_NCLBUTTONDBLCLK";
        case WM_NCRBUTTONDOWN:                   return "WM_NCRBUTTONDOWN";
        case WM_NCRBUTTONUP:                     return "WM_NCRBUTTONUP";
        case WM_NCRBUTTONDBLCLK:                 return "WM_NCRBUTTONDBLCLK";
        case WM_NCMBUTTONDOWN:                   return "WM_NCMBUTTONDOWN";
        case WM_NCMBUTTONUP:                     return "WM_NCMBUTTONUP";
        case WM_NCMBUTTONDBLCLK:                 return "WM_NCMBUTTONDBLCLK";
        case WM_NCXBUTTONDOWN:                   return "WM_NCXBUTTONDOWN";
        case WM_NCXBUTTONUP:                     return "WM_NCXBUTTONUP";
        case WM_NCXBUTTONDBLCLK:                 return "WM_NCXBUTTONDBLCLK";
        case WM_INPUT_DEVICE_CHANGE:             return "WM_INPUT_DEVICE_CHANGE";
        case WM_INPUT:                           return "WM_INPUT";
        //case WM_KEYFIRST:                      return "WM_KEYFIRST"; // duplicate
        case WM_KEYDOWN:                         return "WM_KEYDOWN";
        case WM_KEYUP:                           return "WM_KEYUP";
        case WM_CHAR:                            return "WM_CHAR";
        case WM_DEADCHAR:                        return "WM_DEADCHAR";
        case WM_SYSKEYDOWN:                      return "WM_SYSKEYDOWN";
        case WM_SYSKEYUP:                        return "WM_SYSKEYUP";
        case WM_SYSCHAR:                         return "WM_SYSCHAR";
        case WM_SYSDEADCHAR:                     return "WM_SYSDEADCHAR";
        case WM_UNICHAR:                         return "WM_UNICHAR";
        //case WM_KEYLAST :                      return "WM_KEYLAST"; // duplicate
        case WM_IME_STARTCOMPOSITION:            return "WM_IME_STARTCOMPOSITION";
        case WM_IME_ENDCOMPOSITION:              return "WM_IME_ENDCOMPOSITION";
        case WM_IME_COMPOSITION:                 return "WM_IME_COMPOSITION";
        //case WM_IME_KEYLAST:                   return "WM_IME_KEYLAST"; // duplicate
        case WM_INITDIALOG:                      return "WM_INITDIALOG";
        case WM_COMMAND:                         return "WM_COMMAND";
        case WM_SYSCOMMAND:                      return "WM_SYSCOMMAND";
        case WM_TIMER:                           return "WM_TIMER";
        case WM_HSCROLL:                         return "WM_HSCROLL";
        case WM_VSCROLL:                         return "WM_VSCROLL";
        case WM_INITMENU:                        return "WM_INITMENU";
        case WM_INITMENUPOPUP:                   return "WM_INITMENUPOPUP";
        case WM_GESTURE:                         return "WM_GESTURE";
        case WM_GESTURENOTIFY:                   return "WM_GESTURENOTIFY";
        case WM_MENUSELECT:                      return "WM_MENUSELECT";
        case WM_MENUCHAR:                        return "WM_MENUCHAR";
        case WM_ENTERIDLE:                       return "WM_ENTERIDLE";
        case WM_MENURBUTTONUP:                   return "WM_MENURBUTTONUP";
        case WM_MENUDRAG:                        return "WM_MENUDRAG";
        case WM_MENUGETOBJECT:                   return "WM_MENUGETOBJECT";
        case WM_UNINITMENUPOPUP:                 return "WM_UNINITMENUPOPUP";
        case WM_MENUCOMMAND:                     return "WM_MENUCOMMAND";
        case WM_CHANGEUISTATE:                   return "WM_CHANGEUISTATE";
        case WM_UPDATEUISTATE:                   return "WM_UPDATEUISTATE";
        case WM_QUERYUISTATE:                    return "WM_QUERYUISTATE";
        case WM_CTLCOLORMSGBOX:                  return "WM_CTLCOLORMSGBOX";
        case WM_CTLCOLOREDIT:                    return "WM_CTLCOLOREDIT";
        case WM_CTLCOLORLISTBOX:                 return "WM_CTLCOLORLISTBOX";
        case WM_CTLCOLORBTN:                     return "WM_CTLCOLORBTN";
        case WM_CTLCOLORDLG:                     return "WM_CTLCOLORDLG";
        case WM_CTLCOLORSCROLLBAR:               return "WM_CTLCOLORSCROLLBAR";
        case WM_CTLCOLORSTATIC:                  return "WM_CTLCOLORSTATIC";
        //case WM_MOUSEFIRST:                    return "WM_MOUSEFIRST"; // duplicate
        case WM_MOUSEMOVE:                       return "WM_MOUSEMOVE";
        case WM_LBUTTONDOWN:                     return "WM_LBUTTONDOWN";
        case WM_LBUTTONUP:                       return "WM_LBUTTONUP";
        case WM_LBUTTONDBLCLK:                   return "WM_LBUTTONDBLCLK";
        case WM_RBUTTONDOWN:                     return "WM_RBUTTONDOWN";
        case WM_RBUTTONUP:                       return "WM_RBUTTONUP";
        case WM_RBUTTONDBLCLK:                   return "WM_RBUTTONDBLCLK";
        case WM_MBUTTONDOWN:                     return "WM_MBUTTONDOWN";
        case WM_MBUTTONUP:                       return "WM_MBUTTONUP";
        case WM_MBUTTONDBLCLK:                   return "WM_MBUTTONDBLCLK";
        case WM_MOUSEWHEEL:                      return "WM_MOUSEWHEEL";
        case WM_XBUTTONDOWN:                     return "WM_XBUTTONDOWN";
        case WM_XBUTTONUP:                       return "WM_XBUTTONUP";
        case WM_XBUTTONDBLCLK:                   return "WM_XBUTTONDBLCLK";
        case WM_MOUSEHWHEEL:                     return "WM_MOUSEHWHEEL";
        //case WM_MOUSELAST:                     return "WM_MOUSELAST"; // duplicate
        case WM_PARENTNOTIFY:                    return "WM_PARENTNOTIFY";
        case WM_ENTERMENULOOP:                   return "WM_ENTERMENULOOP";
        case WM_EXITMENULOOP:                    return "WM_EXITMENULOOP";
        case WM_NEXTMENU:                        return "WM_NEXTMENU";
        case WM_SIZING:                          return "WM_SIZING";
        case WM_CAPTURECHANGED:                  return "WM_CAPTURECHANGED";
        case WM_MOVING:                          return "WM_MOVING";
        case WM_POWERBROADCAST:                  return "WM_POWERBROADCAST";
        case WM_DEVICECHANGE:                    return "WM_DEVICECHANGE";
        case WM_MDICREATE:                       return "WM_MDICREATE";
        case WM_MDIDESTROY:                      return "WM_MDIDESTROY";
        case WM_MDIACTIVATE:                     return "WM_MDIACTIVATE";
        case WM_MDIRESTORE:                      return "WM_MDIRESTORE";
        case WM_MDINEXT:                         return "WM_MDINEXT";
        case WM_MDIMAXIMIZE:                     return "WM_MDIMAXIMIZE";
        case WM_MDITILE:                         return "WM_MDITILE";
        case WM_MDICASCADE:                      return "WM_MDICASCADE";
        case WM_MDIICONARRANGE:                  return "WM_MDIICONARRANGE";
        case WM_MDIGETACTIVE:                    return "WM_MDIGETACTIVE";
        case WM_MDISETMENU:                      return "WM_MDISETMENU";
        case WM_ENTERSIZEMOVE:                   return "WM_ENTERSIZEMOVE";
        case WM_EXITSIZEMOVE:                    return "WM_EXITSIZEMOVE";
        case WM_DROPFILES:                       return "WM_DROPFILES";
        case WM_MDIREFRESHMENU:                  return "WM_MDIREFRESHMENU";
        case WM_POINTERDEVICECHANGE:             return "WM_POINTERDEVICECHANGE";
        case WM_POINTERDEVICEINRANGE:            return "WM_POINTERDEVICEINRANGE";
        case WM_POINTERDEVICEOUTOFRANGE:         return "WM_POINTERDEVICEOUTOFRANGE";
        case WM_TOUCH:                           return "WM_TOUCH";
        case WM_NCPOINTERUPDATE:                 return "WM_NCPOINTERUPDATE";
        case WM_NCPOINTERDOWN:                   return "WM_NCPOINTERDOWN";
        case WM_NCPOINTERUP:                     return "WM_NCPOINTERUP";
        case WM_POINTERUPDATE:                   return "WM_POINTERUPDATE";
        case WM_POINTERDOWN:                     return "WM_POINTERDOWN";
        case WM_POINTERUP:                       return "WM_POINTERUP";
        case WM_POINTERENTER:                    return "WM_POINTERENTER";
        case WM_POINTERLEAVE:                    return "WM_POINTERLEAVE";
        case WM_POINTERACTIVATE:                 return "WM_POINTERACTIVATE";
        case WM_POINTERCAPTURECHANGED:           return "WM_POINTERCAPTURECHANGED";
        case WM_TOUCHHITTESTING:                 return "WM_TOUCHHITTESTING";
        case WM_POINTERWHEEL:                    return "WM_POINTERWHEEL";
        case WM_POINTERHWHEEL:                   return "WM_POINTERHWHEEL";
        case WM_POINTERROUTEDTO:                 return "WM_POINTERROUTEDTO";
        case WM_POINTERROUTEDAWAY:               return "WM_POINTERROUTEDAWAY";
        case WM_POINTERROUTEDRELEASED:           return "WM_POINTERROUTEDRELEASED";
        case WM_IME_SETCONTEXT:                  return "WM_IME_SETCONTEXT";
        case WM_IME_NOTIFY:                      return "WM_IME_NOTIFY";
        case WM_IME_CONTROL:                     return "WM_IME_CONTROL";
        case WM_IME_COMPOSITIONFULL:             return "WM_IME_COMPOSITIONFULL";
        case WM_IME_SELECT:                      return "WM_IME_SELECT";
        case WM_IME_CHAR:                        return "WM_IME_CHAR";
        case WM_IME_REQUEST:                     return "WM_IME_REQUEST";
        case WM_IME_KEYDOWN:                     return "WM_IME_KEYDOWN";
        case WM_IME_KEYUP:                       return "WM_IME_KEYUP";
        case WM_MOUSEHOVER:                      return "WM_MOUSEHOVER";
        case WM_MOUSELEAVE:                      return "WM_MOUSELEAVE";
        case WM_NCMOUSEHOVER:                    return "WM_NCMOUSEHOVER";
        case WM_NCMOUSELEAVE:                    return "WM_NCMOUSELEAVE";
        case WM_WTSSESSION_CHANGE:               return "WM_WTSSESSION_CHANGE";
        case WM_TABLET_FIRST:                    return "WM_TABLET_FIRST";
        case WM_TABLET_LAST:                     return "WM_TABLET_LAST";
        case WM_DPICHANGED:                      return "WM_DPICHANGED";
        case WM_DPICHANGED_BEFOREPARENT:         return "WM_DPICHANGED_BEFOREPARENT";
        case WM_DPICHANGED_AFTERPARENT:          return "WM_DPICHANGED_AFTERPARENT";
        case WM_GETDPISCALEDSIZE:                return "WM_GETDPISCALEDSIZE";
        case WM_CUT:                             return "WM_CUT";
        case WM_COPY:                            return "WM_COPY";
        case WM_PASTE:                           return "WM_PASTE";
        case WM_CLEAR:                           return "WM_CLEAR";
        case WM_UNDO:                            return "WM_UNDO";
        case WM_RENDERFORMAT:                    return "WM_RENDERFORMAT";
        case WM_RENDERALLFORMATS:                return "WM_RENDERALLFORMATS";
        case WM_DESTROYCLIPBOARD:                return "WM_DESTROYCLIPBOARD";
        case WM_DRAWCLIPBOARD:                   return "WM_DRAWCLIPBOARD";
        case WM_PAINTCLIPBOARD:                  return "WM_PAINTCLIPBOARD";
        case WM_VSCROLLCLIPBOARD:                return "WM_VSCROLLCLIPBOARD";
        case WM_SIZECLIPBOARD:                   return "WM_SIZECLIPBOARD";
        case WM_ASKCBFORMATNAME:                 return "WM_ASKCBFORMATNAME";
        case WM_CHANGECBCHAIN:                   return "WM_CHANGECBCHAIN";
        case WM_HSCROLLCLIPBOARD:                return "WM_HSCROLLCLIPBOARD";
        case WM_QUERYNEWPALETTE:                 return "WM_QUERYNEWPALETTE";
        case WM_PALETTEISCHANGING:               return "WM_PALETTEISCHANGING";
        case WM_PALETTECHANGED:                  return "WM_PALETTECHANGED";
        case WM_HOTKEY:                          return "WM_HOTKEY";
        case WM_PRINT:                           return "WM_PRINT";
        case WM_PRINTCLIENT:                     return "WM_PRINTCLIENT";
        case WM_APPCOMMAND:                      return "WM_APPCOMMAND";
        case WM_THEMECHANGED:                    return "WM_THEMECHANGED";
        case WM_CLIPBOARDUPDATE:                 return "WM_CLIPBOARDUPDATE";
        case WM_DWMCOMPOSITIONCHANGED:           return "WM_DWMCOMPOSITIONCHANGED";
        case WM_DWMNCRENDERINGCHANGED:           return "WM_DWMNCRENDERINGCHANGED";
        case WM_DWMCOLORIZATIONCOLORCHANGED:     return "WM_DWMCOLORIZATIONCOLORCHANGED";
        case WM_DWMWINDOWMAXIMIZEDCHANGE:        return "WM_DWMWINDOWMAXIMIZEDCHANGE";
        case WM_DWMSENDICONICTHUMBNAIL:          return "WM_DWMSENDICONICTHUMBNAIL";
        case WM_DWMSENDICONICLIVEPREVIEWBITMAP:  return "WM_DWMSENDICONICLIVEPREVIEWBITMAP";
        case WM_GETTITLEBARINFOEX:               return "WM_GETTITLEBARINFOEX";
        case WM_HANDHELDFIRST:                   return "WM_HANDHELDFIRST";
        case WM_HANDHELDLAST:                    return "WM_HANDHELDLAST";
        case WM_AFXFIRST:                        return "WM_AFXFIRST";
        case WM_AFXLAST:                         return "WM_AFXLAST";
        case WM_PENWINFIRST:                     return "WM_PENWINFIRST";
        case WM_PENWINLAST:                      return "WM_PENWINLAST";
        case WM_APP:                             return "WM_APP";
        case WM_USER:                            return "WM_USER";
        }

        return "Unknown Window Event";
      }
    }

  } // namespace win
} // namespace rex

  // NOLINTEND(cppcoreguidelines-pro-type-union-access)