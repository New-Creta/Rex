#pragma once

#ifdef DELETE
    #undef DELETE
#endif

namespace rex
{
	namespace keynames
	{
		constexpr const char* CLEAR                 = "CLEAR";
		constexpr const char* MENU                  = "MENU";
		constexpr const char* PAUSE                 = "PAUSE";
		constexpr const char* CAPITAL               = "CAPITAL";
		constexpr const char* SHIFT                 = "SHIFT";
		constexpr const char* CONTROL               = "CONTROL";
		constexpr const char* ESCAPE                = "ESCAPE";
		constexpr const char* PAGE_UP               = "PAGE UP";
		constexpr const char* PAGE_DOWN             = "PAGE DOWN";
		constexpr const char* END                   = "END";
		constexpr const char* HOME                  = "HOME";
		constexpr const char* LEFT_ARROW            = "LEFT ARROW";
		constexpr const char* UP_ARROW              = "UP ARROW";
		constexpr const char* RIGHT_ARROW           = "RIGHT ARROW";
		constexpr const char* DOWN_ARROW            = "DOWN ARROW";
		constexpr const char* SELECT                = "SELECT";
		constexpr const char* PRINT                 = "PRINT";
		constexpr const char* SNAPSHOT              = "SNAPSHOT";
		constexpr const char* INSERT                = "INSERT";
		constexpr const char* DELETE                = "DELETE";
		constexpr const char* HELP                  = "HELP";
		constexpr const char* SLEEP                 = "SLEEP";

        constexpr const char* NUMLOCK               = "NUMLOCK";
		constexpr const char* SCROLL                = "SCROLL";
		constexpr const char* LEFT_SHIFT            = "LEFT SHIFT";
		constexpr const char* RIGHT_SHIFT           = "RIGHT SHIFT";
		constexpr const char* LEFT_CTRL             = "LEFT CTRL";
		constexpr const char* RIGHT_CTRL            = "RIGHT CTRL";
		constexpr const char* LEFT_ALT              = "LEFT ALT";
		constexpr const char* RIGHT_ALT             = "RIGHT ALT";
		constexpr const char* LEFT_PLATFORM_BTN     = "LEFT PLATFORM BUTTON";
		constexpr const char* RIGHT_PLATFORM_BTN    = "RIGHT PLATFORM BUTTON";
		constexpr const char* APPS                  = "APPS";

        constexpr const char* EQUAL                 = "=";
        constexpr const char* GRAVE                 = "`";
        constexpr const char* TILDE                 = "~";
        constexpr const char* EXCLAMATION           = "!";
        constexpr const char* QUESTION              = "?";
        constexpr const char* AT_SIGN               = "@";
        constexpr const char* NUMBER_SIGN           = "#";
        constexpr const char* DOLLAR_SIGN           = "$";
        constexpr const char* PERCENT               = "%";
        constexpr const char* CARET                 = "^";
        constexpr const char* AMPERSAND             = "&";
        constexpr const char* STAR                  = "*";
        constexpr const char* OPEN_BRACKET          = "(";
        constexpr const char* CLOSE_BRACKET         = ")";
        constexpr const char* MINUS                 = "-";
        constexpr const char* UNDERSCORE            = "_";
        constexpr const char* PLUS                  = "+";
        constexpr const char* OPEN_CURLY            = "{";
        constexpr const char* CLOSE_CURLY           = "}";
        constexpr const char* OPEN_SQUARE           = "[";
        constexpr const char* CLOSE_SQUARE          = "]";
        constexpr const char* SEMI_COLON            = ";";
        constexpr const char* COLON                 = ":";
        constexpr const char* SINGLE_QUOTE          = "'";
        constexpr const char* DOUBLE_QUOTE          = "\"";
        constexpr const char* LESS_THAN             = "<";
        constexpr const char* GREATER_THAN          = ">";
        constexpr const char* COMMA                 = ",";
        constexpr const char* DOT                   = ".";
        constexpr const char* SLASH                 = "/";
        constexpr const char* VERTICAL_BAR          = "|";
        constexpr const char* BACK_SLASH            = "\\";
        constexpr const char* ESC_KEY               = "ESCAPE";
        constexpr const char* TAB                   = "TAB";
        constexpr const char* SPACE                 = "SPACE";
        constexpr const char* BACKSPACE             = "BACKSPACE";

		constexpr const char* F1                    = "F1";
		constexpr const char* F2                    = "F2";
		constexpr const char* F3                    = "F3";
		constexpr const char* F4                    = "F4";
		constexpr const char* F5                    = "F5";
		constexpr const char* F6                    = "F6";
		constexpr const char* F7                    = "F7";
		constexpr const char* F8                    = "F8";
		constexpr const char* F9                    = "F9";
		constexpr const char* F10                   = "F10";
		constexpr const char* F11                   = "F11";
		constexpr const char* F12                   = "F12";
		constexpr const char* F13                   = "F13";
		constexpr const char* F14                   = "F14";
		constexpr const char* F15                   = "F15";
		constexpr const char* F16                   = "F16";
		constexpr const char* F17                   = "F17";
		constexpr const char* F18                   = "F18";
		constexpr const char* F19                   = "F19";
		constexpr const char* F20                   = "F20";
		constexpr const char* F21                   = "F21";
		constexpr const char* F22                   = "F22";
		constexpr const char* F23                   = "F23";
		constexpr const char* F24                   = "F24";

        constexpr const char* D0                    = "0";
        constexpr const char* D1                    = "1";
        constexpr const char* D2                    = "2";
        constexpr const char* D3                    = "3";
        constexpr const char* D4                    = "4";
        constexpr const char* D5                    = "5";
        constexpr const char* D6                    = "6";
        constexpr const char* D7                    = "7";
        constexpr const char* D8                    = "8";
        constexpr const char* D9                    = "9";

        constexpr const char* KP0                   = "Keypad 0";
        constexpr const char* KP1                   = "Keypad 1";
        constexpr const char* KP2                   = "Keypad 2";
        constexpr const char* KP3                   = "Keypad 3";
        constexpr const char* KP4                   = "Keypad 4";
        constexpr const char* KP5                   = "Keypad 5";
        constexpr const char* KP6                   = "Keypad 6";
        constexpr const char* KP7                   = "Keypad 7";
        constexpr const char* KP8                   = "Keypad 8";
        constexpr const char* KP9                   = "Keypad 9";

        constexpr const char* KPDECIMAL             = "Keypad .";
        constexpr const char* KPDIVIDE              = "Keypad /";
        constexpr const char* KPMULTIPLY            = "Keypad *";
        constexpr const char* KPSUBTRACT            = "Keypad -";
        constexpr const char* KPADD                 = "Keypad +";
        constexpr const char* KPENTER               = "Keypad ENTER";
        constexpr const char* KPEQUAL               = "Keypad =";

        constexpr const char* A_HI                  = "A";
        constexpr const char* B_HI                  = "B";
        constexpr const char* C_HI                  = "C";
        constexpr const char* D_HI                  = "D";
        constexpr const char* E_HI                  = "E";
        constexpr const char* F_HI                  = "F";
        constexpr const char* G_HI                  = "G";
        constexpr const char* H_HI                  = "H";
        constexpr const char* I_HI                  = "I";
        constexpr const char* J_HI                  = "J";
        constexpr const char* K_HI                  = "K";
        constexpr const char* L_HI                  = "L";
        constexpr const char* M_HI                  = "M";
        constexpr const char* N_HI                  = "N";
        constexpr const char* O_HI                  = "O";
        constexpr const char* P_HI                  = "P";
        constexpr const char* Q_HI                  = "Q";
        constexpr const char* R_HI                  = "R";
        constexpr const char* S_HI                  = "S";
        constexpr const char* T_HI                  = "T";
        constexpr const char* U_HI                  = "U";
        constexpr const char* V_HI                  = "V";
        constexpr const char* W_HI                  = "W";
        constexpr const char* X_HI                  = "X";
        constexpr const char* Y_HI                  = "Y";
        constexpr const char* Z_HI                  = "Z";

        constexpr const char* A_LO                  = "a";
        constexpr const char* B_LO                  = "b";
        constexpr const char* C_LO                  = "c";
        constexpr const char* D_LO                  = "d";
        constexpr const char* E_LO                  = "e";
        constexpr const char* F_LO                  = "f";
        constexpr const char* G_LO                  = "g";
        constexpr const char* H_LO                  = "h";
        constexpr const char* I_LO                  = "i";
        constexpr const char* J_LO                  = "j";
        constexpr const char* K_LO                  = "k";
        constexpr const char* L_LO                  = "l";
        constexpr const char* M_LO                  = "m";
        constexpr const char* N_LO                  = "n";
        constexpr const char* O_LO                  = "o";
        constexpr const char* P_LO                  = "p";
        constexpr const char* Q_LO                  = "q";
        constexpr const char* R_LO                  = "r";
        constexpr const char* S_LO                  = "s";
        constexpr const char* T_LO                  = "t";
        constexpr const char* U_LO                  = "u";
        constexpr const char* V_LO                  = "v";
        constexpr const char* W_LO                  = "w";
        constexpr const char* X_LO                  = "x";
        constexpr const char* Y_LO                  = "y";
        constexpr const char* Z_LO                  = "z";
	}
}