#pragma once

#ifdef _WINDOWS
	#undef DELETE
#endif

namespace rex
{
    enum class KeyCode
    {
		UNKNOWN				= -1,

		SPACE               = 32,
		APOSTROPHE          = 39, /* ' */
		COMMA               = 44, /* , */
		MINUS               = 45, /* - */
		PERIOD              = 46, /* . */
		SLASH               = 47, /* / */

		D0                  = 48, /* 0 */
		D1                  = 49, /* 1 */
		D2                  = 50, /* 2 */
		D3                  = 51, /* 3 */
		D4                  = 52, /* 4 */
		D5                  = 53, /* 5 */
		D6                  = 54, /* 6 */
		D7                  = 55, /* 7 */
		D8                  = 56, /* 8 */
		D9                  = 57, /* 9 */

		SEMICOLON           = 59, /* ; */
		EQUAL               = 61, /* = */

		A                   = 65,
		B                   = 66,
		C                   = 67,
		D                   = 68,
		E                   = 69,
		F                   = 70,
		G                   = 71,
		H                   = 72,
		I                   = 73,
		J                   = 74,
		K                   = 75,
		L                   = 76,
		M                   = 77,
		N                   = 78,
		O                   = 79,
		P                   = 80,
		Q                   = 81,
		R                   = 82,
		S                   = 83,
		T                   = 84,
		U                   = 85,
		V                   = 86,
		W                   = 87,
		X                   = 88,
		Y                   = 89,
		Z                   = 90,

		LEFTBRACKET         = 91,  /* [ */
		BACKSLASH           = 92,  /* \ */
		RIGHTBRACKET        = 93,  /* ] */
		GRAVEACCENT         = 96,  /* ` */

		WORLD1              = 161, /* non-US #1 */
		WORLD2              = 162, /* non-US #2 */

		/* FUNCTION KEYS */
		ESCAPE              = 256,
		ENTER               = 257,
		TAB                 = 258,
		BACKSPACE           = 259,
		INSERT              = 260,
		DELETE              = 261,
		RIGHT               = 262,
		LEFT                = 263,
		DOWN                = 264,
		UP                  = 265,
		PAGEUP              = 266,
		PAGEDOWN            = 267,
		HOME                = 268,
		END                 = 269,
		CAPSLOCK            = 280,
		SCROLLLOCK          = 281,
		NUMLOCK             = 282,
		PRINTSCREEN         = 283,
		PAUSE               = 284,
		F1                  = 290,
		F2                  = 291,
		F3                  = 292,
		F4                  = 293,
		F5                  = 294,
		F6                  = 295,
		F7                  = 296,
		F8                  = 297,
		F9                  = 298,
		F10                 = 299,
		F11                 = 300,
		F12                 = 301,
		F13                 = 302,
		F14                 = 303,
		F15                 = 304,
		F16                 = 305,
		F17                 = 306,
		F18                 = 307,
		F19                 = 308,
		F20                 = 309,
		F21                 = 310,
		F22                 = 311,
		F23                 = 312,
		F24                 = 313,
		F25                 = 314,

		/* KEYPAD */
		KP0                 = 320,
		KP1                 = 321,
		KP2                 = 322,
		KP3                 = 323,
		KP4                 = 324,
		KP5                 = 325,
		KP6                 = 326,
		KP7                 = 327,
		KP8                 = 328,
		KP9                 = 329,
		KPDECIMAL           = 330,
		KPDIVIDE            = 331,
		KPMULTIPLY          = 332,
		KPSUBTRACT          = 333,
		KPADD               = 334,
		KPENTER             = 335,
		KPEQUAL             = 336,

		LEFTSHIFT           = 340,
		LEFTCONTROL         = 341,
		LEFTALT             = 342,
		LEFTSUPER           = 343,
		RIGHTSHIFT          = 344,
		RIGHTCONTROL        = 345,
		RIGHTALT            = 346,
		RIGHTSUPER          = 347,
		MENU                = 348
	};

	//-------------------------------------------------------------------------
    inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
    {
        os << static_cast<uint16>(keyCode);
        return os;
    }

	//-------------------------------------------------------------------------
	KeyCode keyNameToKeyCode(const int8* keyName);
}

#define RX_KEY_SPACE           rex::KeyCode::SPACE
#define RX_KEY_APOSTROPHE      rex::KeyCode::APOSTROPHE    /* ' */
#define RX_KEY_COMMA           rex::KeyCode::COMMA         /* , */
#define RX_KEY_MINUS           rex::KeyCode::MINUS         /* - */
#define RX_KEY_PERIOD          rex::KeyCode::PERIOD        /* . */
#define RX_KEY_SLASH           rex::KeyCode::SLASH         /* / */
#define RX_KEY_0               rex::KeyCode::D0
#define RX_KEY_1               rex::KeyCode::D1
#define RX_KEY_2               rex::KeyCode::D2
#define RX_KEY_3               rex::KeyCode::D3
#define RX_KEY_4               rex::KeyCode::D4
#define RX_KEY_5               rex::KeyCode::D5
#define RX_KEY_6               rex::KeyCode::D6
#define RX_KEY_7               rex::KeyCode::D7
#define RX_KEY_8               rex::KeyCode::D8
#define RX_KEY_9               rex::KeyCode::D9
#define RX_KEY_SEMICOLON       rex::KeyCode::SEMICOLON     /* ; */
#define RX_KEY_EQUAL           rex::KeyCode::EQUAL         /* = */
#define RX_KEY_A               rex::KeyCode::A
#define RX_KEY_B               rex::KeyCode::B
#define RX_KEY_C               rex::KeyCode::C
#define RX_KEY_D               rex::KeyCode::D
#define RX_KEY_E               rex::KeyCode::E
#define RX_KEY_F               rex::KeyCode::F
#define RX_KEY_G               rex::KeyCode::G
#define RX_KEY_H               rex::KeyCode::H
#define RX_KEY_I               rex::KeyCode::I
#define RX_KEY_J               rex::KeyCode::J
#define RX_KEY_K               rex::KeyCode::K
#define RX_KEY_L               rex::KeyCode::L
#define RX_KEY_M               rex::KeyCode::M
#define RX_KEY_N               rex::KeyCode::N
#define RX_KEY_O               rex::KeyCode::O
#define RX_KEY_P               rex::KeyCode::P
#define RX_KEY_Q               rex::KeyCode::Q
#define RX_KEY_R               rex::KeyCode::R
#define RX_KEY_S               rex::KeyCode::S
#define RX_KEY_T               rex::KeyCode::T
#define RX_KEY_U               rex::KeyCode::U
#define RX_KEY_V               rex::KeyCode::V
#define RX_KEY_W               rex::KeyCode::W
#define RX_KEY_X               rex::KeyCode::X
#define RX_KEY_Y               rex::KeyCode::Y
#define RX_KEY_Z               rex::KeyCode::Z
#define RX_KEY_LEFT_BRACKET    rex::KeyCode::LEFTBRACKET   /* [ */
#define RX_KEY_BACKSLASH       rex::KeyCode::BACKSLASH     /* \ */
#define RX_KEY_RIGHT_BRACKET   rex::KeyCode::RIGHTBRACKET  /* ] */
#define RX_KEY_GRAVE_ACCENT    rex::KeyCode::GRAVEACCENT   /* ` */
#define RX_KEY_WORLD_1         rex::KeyCode::WORLD1        /* non-US #1 */
#define RX_KEY_WORLD_2         rex::KeyCode::WORLD2        /* non-US #2 */

/* Function keys */
#define RX_KEY_ESCAPE          rex::KeyCode::ESCAPE
#define RX_KEY_ENTER           rex::KeyCode::ENTER
#define RX_KEY_TAB             rex::KeyCode::TAB
#define RX_KEY_BACKSPACE       rex::KeyCode::BACKSPACE
#define RX_KEY_INSERT          rex::KeyCode::INSERT
#define RX_KEY_DELETE          rex::KeyCode::DELETE
#define RX_KEY_RIGHT           rex::KeyCode::RIGHT
#define RX_KEY_LEFT            rex::KeyCode::LEFT
#define RX_KEY_DOWN            rex::KeyCode::DOWN
#define RX_KEY_UP              rex::KeyCode::UP
#define RX_KEY_PAGE_UP         rex::KeyCode::PAGEUP
#define RX_KEY_PAGE_DOWN       rex::KeyCode::PAGEDOWN
#define RX_KEY_HOME            rex::KeyCode::HOME
#define RX_KEY_END             rex::KeyCode::END
#define RX_KEY_CAPS_LOCK       rex::KeyCode::CAPSLOCK
#define RX_KEY_SCROLL_LOCK     rex::KeyCode::SCROLLLOCK
#define RX_KEY_NUM_LOCK        rex::KeyCode::NUMLOCK
#define RX_KEY_PRINT_SCREEN    rex::KeyCode::PRINTSCREEN
#define RX_KEY_PAUSE           rex::KeyCode::PAUSE
#define RX_KEY_F1              rex::KeyCode::F1
#define RX_KEY_F2              rex::KeyCode::F2
#define RX_KEY_F3              rex::KeyCode::F3
#define RX_KEY_F4              rex::KeyCode::F4
#define RX_KEY_F5              rex::KeyCode::F5
#define RX_KEY_F6              rex::KeyCode::F6
#define RX_KEY_F7              rex::KeyCode::F7
#define RX_KEY_F8              rex::KeyCode::F8
#define RX_KEY_F9              rex::KeyCode::F9
#define RX_KEY_F10             rex::KeyCode::F10
#define RX_KEY_F11             rex::KeyCode::F11
#define RX_KEY_F12             rex::KeyCode::F12
#define RX_KEY_F13             rex::KeyCode::F13
#define RX_KEY_F14             rex::KeyCode::F14
#define RX_KEY_F15             rex::KeyCode::F15
#define RX_KEY_F16             rex::KeyCode::F16
#define RX_KEY_F17             rex::KeyCode::F17
#define RX_KEY_F18             rex::KeyCode::F18
#define RX_KEY_F19             rex::KeyCode::F19
#define RX_KEY_F20             rex::KeyCode::F20
#define RX_KEY_F21             rex::KeyCode::F21
#define RX_KEY_F22             rex::KeyCode::F22
#define RX_KEY_F23             rex::KeyCode::F23
#define RX_KEY_F24             rex::KeyCode::F24
#define RX_KEY_F25             rex::KeyCode::F25

/* Keypad */
#define RX_KEY_KP_0            rex::KeyCode::KP0
#define RX_KEY_KP_1            rex::KeyCode::KP1
#define RX_KEY_KP_2            rex::KeyCode::KP2
#define RX_KEY_KP_3            rex::KeyCode::KP3
#define RX_KEY_KP_4            rex::KeyCode::KP4
#define RX_KEY_KP_5            rex::KeyCode::KP5
#define RX_KEY_KP_6            rex::KeyCode::KP6
#define RX_KEY_KP_7            rex::KeyCode::KP7
#define RX_KEY_KP_8            rex::KeyCode::KP8
#define RX_KEY_KP_9            rex::KeyCode::KP9
#define RX_KEY_KP_DECIMAL      rex::KeyCode::KPDECIMAL
#define RX_KEY_KP_DIVIDE       rex::KeyCode::KPDIVIDE
#define RX_KEY_KP_MULTIPLY     rex::KeyCode::KPMULTIPLY
#define RX_KEY_KP_SUBTRACT     rex::KeyCode::KPSUBTRACT
#define RX_KEY_KP_ADD          rex::KeyCode::KPADD
#define RX_KEY_KP_ENTER        rex::KeyCode::KPENTER
#define RX_KEY_KP_EQUAL        rex::KeyCode::KPEQUAL

#define RX_KEY_LEFT_SHIFT      rex::KeyCode::LEFTSHIFT
#define RX_KEY_LEFT_CONTROL    rex::KeyCode::LEFTCONTROL
#define RX_KEY_LEFT_ALT        rex::KeyCode::LEFTALT
#define RX_KEY_LEFT_SUPER      rex::KeyCode::LEFTSUPER
#define RX_KEY_RIGHT_SHIFT     rex::KeyCode::RIGHTSHIFT
#define RX_KEY_RIGHT_CONTROL   rex::KeyCode::RIGHTCONTROL
#define RX_KEY_RIGHT_ALT       rex::KeyCode::RIGHTALT
#define RX_KEY_RIGHT_SUPER     rex::KeyCode::RIGHTSUPER
#define RX_KEY_MENU            rex::KeyCode::MENU