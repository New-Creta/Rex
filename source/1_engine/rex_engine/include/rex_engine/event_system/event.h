#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/event_system/event_type.h"

namespace rex
{
  namespace event_system
  {
    struct WindowResizeEvent
    {
      s32 window_width;
      s32 window_height;
    };

    struct Event
    {
      EventType type;

      union EventData
      {
        WindowResizeEvent window_resize;
      } data;
    };
  } // namespace event_system

  class EventBase
  {

  };

	enum class KeyCode
	{
		Unknown,

		Zero = 48, // '0'
		One,
		Two,
		Three,
		Four,
		Five,
		Six,
		Seven,
		Eight,
		Nine,

		A = 65, // 'A'
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,

		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		F16,
		F17,
		F18,
		F19,
		F20,
		F21,
		F22,
		F23,
		F24,

		UpArrow,
		DownArrow,
		LeftArrow,
		RightArrow,

		NumPad0,
		NumPad1,
		NumPad2,
		NumPad3,
		NumPad4,
		NumPad5,
		NumPad6,
		NumPad7,
		NumPad8,
		NumPad9,

		Escape,
		Equal,
		Grave,
		Tilde,
		Exclamation,
		Question,
		AtSign,
		Pound,
		DollarSign,
		Percent,
		Caret,
		AmperSand,
		Star,
		OpenBracket,
		CloseBracket,
		Minus,
		UnderScore,
		Plus,
		OpenCurly,
		CloseCurly,
		OpenSquare,
		CloseSquare,
		SemiColon,
		Colon,
		SingleQuote,
		DoubleQuote,
		LessThan,
		GreaterThan,
		Comma,
		Dot,
		Slash,
		BackSlash,
		VerticalBar,

		Ctrl,
		Shift,
		Alt,
		Caps,
		BackSpace,
		Tab,
		Space,
		NumLock,

		Clear,
		Return,
		NewLine,
		Menu,
		Pause,
		Capital,
		PageUp,
		PageDown,
		End,
		Home,
		Select,
		Print,
		Snapshot,
		Insert,
		Help,
		Sleep,
		Scroll,
		LeftShift,
		RightShift,
		LeftCtrl,
		RightCtrl,
		LeftPlatformBtn,
		RightPlatformBtn,
		Apps,
		Delete,

		KeyCount
	};

  struct Key
  {
  public:
    constexpr Key(KeyCode code, rsl::string_view name)
      : code(code)
      , name(name)
    {}

    KeyCode code;
    rsl::string_view name;
  };

  struct CharKey
  {
    KeyCode code;
    rsl::string_view name;
    char8 ascii;
  };

  class CharDown : public EventBase
  {
  public:
		CharDown(CharKey key)
			: m_key(key)
		{}
		
		CharKey  key() const
		{
			return m_key;
		}

  private:
    CharKey  m_key;
  };
} // namespace rex