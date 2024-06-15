#include "rex_windows/event_system/win_event_handler.h"

#include "rex_engine/app/core_window.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/event_system/event.h"
#include "rex_engine/event_system/event_system.h"
#include "rex_engine/platform/win/diagnostics/win_call.h"
#include "rex_std/bonus/utility.h"
#include "rex_windows/diagnostics/log.h"

#include "rex_engine/event_system/events/window/window_close.h"
#include "rex_engine/event_system/events/window/window_activated.h"
#include "rex_engine/event_system/events/window/window_deactivated.h"
#include "rex_engine/event_system/events/window/window_resize.h"
#include "rex_engine/event_system/events/window/window_start_resize.h"
#include "rex_engine/event_system/events/window/window_end_resize.h"
#include "rex_engine/diagnostics/assert.h"

#include "rex_engine/input/key_code.h"

#include "rex_engine/event_system/events/input/char_down.h"
#include "rex_engine/event_system/events/input/key_down.h"
#include "rex_engine/event_system/events/input/key_up.h"

#define NOMINMAX
#include <Windows.h>

#include "rex_windows/imgui/win_imgui.h"

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
        case VK_TAB:						return KeyCode::Tab				;
        case VK_CLEAR:					return KeyCode::Clear			;
        case VK_RETURN:         return KeyCode::Return		;
        case VK_SHIFT:          return KeyCode::Shift			;
        case VK_CONTROL:        return KeyCode::Ctrl			;
        case VK_MENU:           return KeyCode::Menu			;
        case VK_PAUSE:          return KeyCode::Pause			;
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
        case VK_END:            return          KeyCode::End				;
        case VK_HOME:           return          KeyCode::Home				;
        case VK_LEFT:           return          KeyCode::LeftArrow	;
        case VK_UP:             return          KeyCode::UpArrow		;
        case VK_RIGHT:          return          KeyCode::RightArrow	;
        case VK_DOWN:           return          KeyCode::DownArrow	;
        case VK_SELECT:         return          KeyCode::Select			;
        case VK_PRINT:          return          KeyCode::Print			;
        //case VK_EXECUTE:      
        case VK_SNAPSHOT:       return KeyCode::Snapshot			;
        case VK_INSERT:         return KeyCode::Insert				;
        case VK_DELETE:         return KeyCode::Delete				;
        case VK_HELP:           return KeyCode::Help					;
        case '0':								return KeyCode::Zero					;
        case '1':               return KeyCode::One						;
        case '2':               return KeyCode::Two						;
        case '3':               return KeyCode::Three					;
        case '4':               return KeyCode::Four					;
        case '5':               return KeyCode::Five					;
        case '6':               return KeyCode::Six						;
        case '7':               return KeyCode::Seven					;
        case '8':               return KeyCode::Eight					;
        case '9':               return KeyCode::Nine					;
        case 'A':								return KeyCode::A							;
        case 'B':               return KeyCode::B							;
        case 'C':               return KeyCode::C							;
        case 'D':               return KeyCode::D							;
        case 'E':               return KeyCode::E							;
        case 'F':               return KeyCode::F							;
        case 'G':               return KeyCode::G							;
        case 'H':               return KeyCode::H							;
        case 'I':               return KeyCode::I							;
        case 'J':               return KeyCode::J							;
        case 'K':               return KeyCode::K							;
        case 'L':               return KeyCode::L							;
        case 'M':               return KeyCode::M							;
        case 'N':               return KeyCode::N							;
        case 'O':               return KeyCode::O							;
        case 'P':               return KeyCode::P							;
        case 'Q':               return KeyCode::Q							;
        case 'R':               return KeyCode::R							;
        case 'S':               return KeyCode::S							;
        case 'T':               return KeyCode::T							;
        case 'U':               return KeyCode::U							;
        case 'V':               return KeyCode::V							;
        case 'W':               return KeyCode::W							;
        case 'X':               return KeyCode::X							;
        case 'Y':               return KeyCode::Y							;
        case 'Z':               return KeyCode::Z							;
        case VK_LWIN:           return KeyCode::LeftPlatformBtn;
        case VK_RWIN:           return KeyCode::RightPlatformBtn;
        case VK_APPS:           return KeyCode::Apps		;
        case VK_SLEEP:          return KeyCode::Sleep		;
        case VK_NUMPAD0:        return KeyCode::NumPad0	;
        case VK_NUMPAD1:        return KeyCode::NumPad1	;
        case VK_NUMPAD2:        return KeyCode::NumPad2	;
        case VK_NUMPAD3:        return KeyCode::NumPad3	;
        case VK_NUMPAD4:        return KeyCode::NumPad4	;
        case VK_NUMPAD5:        return KeyCode::NumPad5	;
        case VK_NUMPAD6:        return KeyCode::NumPad6	;
        case VK_NUMPAD7:        return KeyCode::NumPad7	;
        case VK_NUMPAD8:        return KeyCode::NumPad8	;
        case VK_NUMPAD9:        return KeyCode::NumPad9	;
        case VK_MULTIPLY:       return KeyCode::Star		;
        case VK_ADD:            return KeyCode::Plus		;
        case VK_SEPARATOR:      return KeyCode::Minus		;
        case VK_SUBTRACT:       return KeyCode::Minus		;
        case VK_DECIMAL:        return KeyCode::Dot			;
        case VK_DIVIDE:         return KeyCode::Slash		;
        case VK_F1:							return KeyCode::F1			;
        case VK_F2:             return KeyCode::F2			;
        case VK_F3:             return KeyCode::F3			;
        case VK_F4:             return KeyCode::F4			;
        case VK_F5:             return KeyCode::F5			;
        case VK_F6:             return KeyCode::F6			;
        case VK_F7:             return KeyCode::F7			;
        case VK_F8:             return KeyCode::F8			;
        case VK_F9:             return KeyCode::F9			;
        case VK_F10:            return KeyCode::F10			;
        case VK_F11:            return KeyCode::F11			;
        case VK_F12:            return KeyCode::F12			;
        case VK_F13:            return KeyCode::F13			;
        case VK_F14:            return KeyCode::F14			;
        case VK_F15:            return KeyCode::F15			;
        case VK_F16:            return KeyCode::F16			;
        case VK_F17:            return KeyCode::F17			;
        case VK_F18:            return KeyCode::F18			;
        case VK_F19:            return KeyCode::F19			;
        case VK_F20:            return KeyCode::F20			;
        case VK_F21:            return KeyCode::F21			;
        case VK_F22:            return KeyCode::F22			;
        case VK_F23:            return KeyCode::F23			;
        case VK_F24:            return KeyCode::F24			;
        case VK_NUMLOCK:        return KeyCode::NumLock	;
        case VK_SCROLL:         return KeyCode::Scroll	;
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
        case VK_OEM_1:										return KeyCode::SemiColon		;
        case VK_OEM_PLUS:									return KeyCode::Plus				;
        case VK_OEM_COMMA:								return KeyCode::Comma				;
        case VK_OEM_MINUS:								return KeyCode::Minus				;
        case VK_OEM_PERIOD:								return KeyCode::Dot					;
        case VK_OEM_2:										return KeyCode::Slash				;
        case VK_OEM_3:										return KeyCode::Grave				;
        case VK_OEM_4:										return KeyCode::OpenSquare	;
        case VK_OEM_5:										return KeyCode::BackSlash		;
        case VK_OEM_6:										return KeyCode::CloseSquare	;
        case VK_OEM_7:										return KeyCode::SingleQuote	;
        //case VK_OEM_8:									
        case VK_OEM_102:									return KeyCode::GreaterThan	;
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

      switch(msg)
      {
        case WM_CLOSE: REX_WARN(LogWindows, "Verify if the user really wants to close"); break;
        case WM_DESTROY:
          PostQuitMessage(0);
          event_system().enqueue_event(rex::WindowClose());
          return 0;

        case WM_ACTIVATE:
          if(LOWORD(wparam) == WA_INACTIVE)
          {
            event_system().enqueue_event(rex::WindowDeactivated());
          }
          else
          {
            event_system().enqueue_event(rex::WindowActivated());
          }
          return 0;

        case WM_ENTERSIZEMOVE:
          event_system().enqueue_event(rex::WindowStartResize());
          return 0;

        case WM_EXITSIZEMOVE:
          event_system().enqueue_event(rex::WindowEndResize());
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

        case WM_SIZE:
        {
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
          event_system().enqueue_event(WindowResize(width, height, resize_type));
          return 0;
        }
        case WM_CHAR:
          event_system().enqueue_event(CharDown((char8)wparam));
          break;

        case WM_KEYDOWN: 
          event_system().enqueue_event(KeyDown(internal::keycode_from_vk(wparam, lparam)));
          break;
        case WM_KEYUP:
          event_system().enqueue_event(KeyUp(internal::keycode_from_vk(wparam, lparam)));
          break;
        default:
          // Nothing to implement
          break;
      }
      return DefWindowProc(static_cast<HWND>(hwnd), msg, wparam, lparam);
    }

  } // namespace win
} // namespace rex

  // NOLINTEND(cppcoreguidelines-pro-type-union-access)