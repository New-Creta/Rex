#include "core/keycodes.h"
#include "core/keynames.h"

#include "string/hash.h"

namespace
{
	constexpr size_t	KEY_0_HASH = rex::hash("0");
	constexpr size_t	KEY_1_HASH = rex::hash("1");
	constexpr size_t	KEY_2_HASH = rex::hash("2");
	constexpr size_t	KEY_3_HASH = rex::hash("3");
	constexpr size_t	KEY_4_HASH = rex::hash("4");
	constexpr size_t	KEY_5_HASH = rex::hash("5");
	constexpr size_t	KEY_6_HASH = rex::hash("6");
	constexpr size_t	KEY_7_HASH = rex::hash("7");
	constexpr size_t	KEY_8_HASH = rex::hash("8");
	constexpr size_t	KEY_9_HASH = rex::hash("9");

	constexpr size_t	KEY_A_HI_HASH = rex::hash("A");
	constexpr size_t	KEY_B_HI_HASH = rex::hash("B");
	constexpr size_t	KEY_C_HI_HASH = rex::hash("C");
	constexpr size_t	KEY_D_HI_HASH = rex::hash("D");
	constexpr size_t	KEY_E_HI_HASH = rex::hash("E");
	constexpr size_t	KEY_F_HI_HASH = rex::hash("F");
	constexpr size_t	KEY_G_HI_HASH = rex::hash("G");
	constexpr size_t	KEY_H_HI_HASH = rex::hash("H");
	constexpr size_t	KEY_I_HI_HASH = rex::hash("I");
	constexpr size_t	KEY_J_HI_HASH = rex::hash("J");
	constexpr size_t	KEY_K_HI_HASH = rex::hash("K");
	constexpr size_t	KEY_L_HI_HASH = rex::hash("L");
	constexpr size_t	KEY_M_HI_HASH = rex::hash("M");
	constexpr size_t	KEY_N_HI_HASH = rex::hash("N");
	constexpr size_t	KEY_O_HI_HASH = rex::hash("O");
	constexpr size_t	KEY_P_HI_HASH = rex::hash("P");
	constexpr size_t	KEY_Q_HI_HASH = rex::hash("Q");
	constexpr size_t	KEY_R_HI_HASH = rex::hash("R");
	constexpr size_t	KEY_S_HI_HASH = rex::hash("S");
	constexpr size_t	KEY_T_HI_HASH = rex::hash("T");
	constexpr size_t	KEY_U_HI_HASH = rex::hash("U");
	constexpr size_t	KEY_V_HI_HASH = rex::hash("V");
	constexpr size_t	KEY_W_HI_HASH = rex::hash("W");
	constexpr size_t	KEY_X_HI_HASH = rex::hash("X");
	constexpr size_t	KEY_Y_HI_HASH = rex::hash("Y");
	constexpr size_t	KEY_Z_HI_HASH = rex::hash("Z");

	constexpr size_t	KEY_A_LO_HASH = rex::hash("a");
	constexpr size_t	KEY_B_LO_HASH = rex::hash("b");
	constexpr size_t	KEY_C_LO_HASH = rex::hash("c");
	constexpr size_t	KEY_D_LO_HASH = rex::hash("d");
	constexpr size_t	KEY_E_LO_HASH = rex::hash("e");
	constexpr size_t	KEY_F_LO_HASH = rex::hash("f");
	constexpr size_t	KEY_G_LO_HASH = rex::hash("g");
	constexpr size_t	KEY_H_LO_HASH = rex::hash("h");
	constexpr size_t	KEY_I_LO_HASH = rex::hash("i");
	constexpr size_t	KEY_J_LO_HASH = rex::hash("j");
	constexpr size_t	KEY_K_LO_HASH = rex::hash("k");
	constexpr size_t	KEY_L_LO_HASH = rex::hash("l");
	constexpr size_t	KEY_M_LO_HASH = rex::hash("m");
	constexpr size_t	KEY_N_LO_HASH = rex::hash("n");
	constexpr size_t	KEY_O_LO_HASH = rex::hash("o");
	constexpr size_t	KEY_P_LO_HASH = rex::hash("p");
	constexpr size_t	KEY_Q_LO_HASH = rex::hash("q");
	constexpr size_t	KEY_R_LO_HASH = rex::hash("r");
	constexpr size_t	KEY_S_LO_HASH = rex::hash("s");
	constexpr size_t	KEY_T_LO_HASH = rex::hash("t");
	constexpr size_t	KEY_U_LO_HASH = rex::hash("u");
	constexpr size_t	KEY_V_LO_HASH = rex::hash("v");
	constexpr size_t	KEY_W_LO_HASH = rex::hash("w");
	constexpr size_t	KEY_X_LO_HASH = rex::hash("x");
	constexpr size_t	KEY_Y_LO_HASH = rex::hash("y");
	constexpr size_t	KEY_Z_LO_HASH = rex::hash("z");

	constexpr size_t	KEY_F1_HASH = rex::hash(rex::keynames::F1);
	constexpr size_t	KEY_F2_HASH = rex::hash(rex::keynames::F2);
	constexpr size_t	KEY_F3_HASH = rex::hash(rex::keynames::F3);
	constexpr size_t	KEY_F4_HASH = rex::hash(rex::keynames::F4);
	constexpr size_t	KEY_F5_HASH = rex::hash(rex::keynames::F5);
	constexpr size_t	KEY_F6_HASH = rex::hash(rex::keynames::F6);
	constexpr size_t	KEY_F7_HASH = rex::hash(rex::keynames::F7);
	constexpr size_t	KEY_F8_HASH = rex::hash(rex::keynames::F8);
	constexpr size_t	KEY_F9_HASH = rex::hash(rex::keynames::F9);
	constexpr size_t	KEY_F10_HASH = rex::hash(rex::keynames::F10);
	constexpr size_t	KEY_F11_HASH = rex::hash(rex::keynames::F11);
	constexpr size_t	KEY_F12_HASH = rex::hash(rex::keynames::F12);

	constexpr size_t	KEY_EQUAL_HASH = rex::hash("=");
	constexpr size_t	KEY_GRAVE_HASH = rex::hash("`");
	constexpr size_t	KEY_TILDE_HASH = rex::hash("~");
	constexpr size_t	KEY_EXCLAMATION_HASH = rex::hash("!");
	constexpr size_t	KEY_QUESTION_HASH = rex::hash("?");
	constexpr size_t	KEY_AT_SIGN_HASH = rex::hash("@");
	constexpr size_t	KEY_NUMBER_SIGN_HASH = rex::hash("#");
	constexpr size_t	KEY_DOLLAR_SIGN_HASH = rex::hash("$");
	constexpr size_t	KEY_PERCENT = rex::hash("%");
	constexpr size_t	KEY_CARET_HASH = rex::hash("^");
	constexpr size_t	KEY_AMPERSAND_HASH = rex::hash("&");
	constexpr size_t	KEY_STAR_HASH = rex::hash("*");
	constexpr size_t	KEY_OPEN_BRACKET_HASH = rex::hash("(");
	constexpr size_t	KEY_CLOSE_BRACKET_HASH = rex::hash(")");
	constexpr size_t	KEY_MINUS_HASH = rex::hash("-");
	constexpr size_t	KEY_UNDERSCORE_HASH = rex::hash("_");
	constexpr size_t	KEY_PLUS_HASH = rex::hash("+");
	constexpr size_t	KEY_OPEN_CURLY_HASH = rex::hash("{");
	constexpr size_t	KEY_CLOSE_CURLY_HASH = rex::hash("}");
	constexpr size_t	KEY_OPEN_SQUARE_HASH = rex::hash("[");
	constexpr size_t	KEY_CLOSE_SQUARE_HASH = rex::hash("]");
	constexpr size_t	KEY_SEMI_COLON_HASH = rex::hash(";");
	constexpr size_t	KEY_COLON_HASH = rex::hash(":");
	constexpr size_t	KEY_SINGLE_QUOTE_HASH = rex::hash("'");
	constexpr size_t	KEY_DOUBLE_QUOTE_HASH = rex::hash("\"");
	constexpr size_t	KEY_LESS_THAN_HASH = rex::hash("<");
	constexpr size_t	KEY_GREATER_THAN_HASH = rex::hash(">");
	constexpr size_t	KEY_COMMA_HASH = rex::hash(",");
	constexpr size_t	KEY_DOT_HASH = rex::hash(".");
	constexpr size_t	KEY_SLASH_HASH = rex::hash("/");
	constexpr size_t	KEY_BACK_SLASH_HASH = rex::hash("\\");
	constexpr size_t	KEY_VERTICAL_BAR_HASH = rex::hash("|");
	constexpr size_t	KEY_ESC_KEY_HASH = rex::hash("\x1b");

	constexpr size_t	KEY_TAB_HASH = rex::hash("\t");
	constexpr size_t	KEY_SPACE_HASH = rex::hash(" ");

	constexpr size_t	KEY_ESC_HASH = rex::hash(rex::keynames::ESCAPE);
	constexpr size_t	KEY_UP_HASH = rex::hash(rex::keynames::UP_ARROW);
	constexpr size_t	KEY_DOWN_HASH = rex::hash(rex::keynames::DOWN_ARROW);
	constexpr size_t	KEY_LEFT_HASH = rex::hash(rex::keynames::LEFT_ARROW);
	constexpr size_t	KEY_RIGHT_HASH = rex::hash(rex::keynames::RIGHT_ARROW);
	constexpr size_t	KEY_NUMLOCK_HASH = rex::hash(rex::keynames::NUMLOCK);
	constexpr size_t	KEY_RIGHT_SHIFT_HASH = rex::hash(rex::keynames::RIGHT_SHIFT);
	constexpr size_t	KEY_LEFT_SHIFT_HASH = rex::hash(rex::keynames::LEFT_SHIFT);
	constexpr size_t	KEY_RIGHT_CTRL_HASH = rex::hash(rex::keynames::RIGHT_CTRL);
	constexpr size_t	KEY_LEFT_CTRL_HASH = rex::hash(rex::keynames::LEFT_CTRL);
	constexpr size_t	KEY_RIGHT_ALT_HASH = rex::hash(rex::keynames::RIGHT_ALT);
	constexpr size_t	KEY_LEFT_ALT_HASH = rex::hash(rex::keynames::LEFT_ALT);
	constexpr size_t	KEY_CAPS_HASH = rex::hash(rex::keynames::CAPITAL);
	constexpr size_t	KEY_HOME_HASH = rex::hash(rex::keynames::HOME);
	constexpr size_t	KEY_END_HASH = rex::hash(rex::keynames::END);
	constexpr size_t	KEY_DELETE_HASH = rex::hash(rex::keynames::DELETE);
	constexpr size_t	KEY_LEFT_PLATFORM_BTN_HASH = rex::hash(rex::keynames::LEFT_PLATFORM_BTN);
}

//-------------------------------------------------------------------------
rex::KeyCode rex::keyNameToKeyCode(const int8* keyName)
{
	size_t hash = rex::hash(keyName);

	switch (hash)
	{
	case KEY_0_HASH:					return KeyCode::D0;
	case KEY_1_HASH:					return KeyCode::D1;
	case KEY_2_HASH:					return KeyCode::D2;
	case KEY_3_HASH:					return KeyCode::D3;
	case KEY_4_HASH:					return KeyCode::D4;
	case KEY_5_HASH:					return KeyCode::D5;
	case KEY_6_HASH:					return KeyCode::D6;
	case KEY_7_HASH:					return KeyCode::D7;
	case KEY_8_HASH:					return KeyCode::D8;
	case KEY_9_HASH:					return KeyCode::D9;

	case KEY_A_HI_HASH:					return KeyCode::A;
	case KEY_B_HI_HASH:					return KeyCode::B;
	case KEY_C_HI_HASH:					return KeyCode::C;
	case KEY_D_HI_HASH:					return KeyCode::D;
	case KEY_E_HI_HASH:					return KeyCode::E;
	case KEY_F_HI_HASH:					return KeyCode::F;
	case KEY_G_HI_HASH:					return KeyCode::G;
	case KEY_H_HI_HASH:					return KeyCode::H;
	case KEY_I_HI_HASH:					return KeyCode::I;
	case KEY_J_HI_HASH:					return KeyCode::J;
	case KEY_K_HI_HASH:					return KeyCode::K;
	case KEY_L_HI_HASH:					return KeyCode::L;
	case KEY_M_HI_HASH:					return KeyCode::M;
	case KEY_N_HI_HASH:					return KeyCode::N;
	case KEY_O_HI_HASH:					return KeyCode::O;
	case KEY_P_HI_HASH:					return KeyCode::P;
	case KEY_Q_HI_HASH:					return KeyCode::Q;
	case KEY_R_HI_HASH:					return KeyCode::R;
	case KEY_S_HI_HASH:					return KeyCode::S;
	case KEY_T_HI_HASH:					return KeyCode::T;
	case KEY_U_HI_HASH:					return KeyCode::U;
	case KEY_V_HI_HASH:					return KeyCode::V;
	case KEY_W_HI_HASH:					return KeyCode::W;
	case KEY_X_HI_HASH:					return KeyCode::X;
	case KEY_Y_HI_HASH:					return KeyCode::Y;
	case KEY_Z_HI_HASH:					return KeyCode::Z;

	case KEY_A_LO_HASH:					return KeyCode::A;
	case KEY_B_LO_HASH:					return KeyCode::B;
	case KEY_C_LO_HASH:					return KeyCode::C;
	case KEY_D_LO_HASH:					return KeyCode::D;
	case KEY_E_LO_HASH:					return KeyCode::E;
	case KEY_F_LO_HASH:					return KeyCode::F;
	case KEY_G_LO_HASH:					return KeyCode::G;
	case KEY_H_LO_HASH:					return KeyCode::H;
	case KEY_I_LO_HASH:					return KeyCode::I;
	case KEY_J_LO_HASH:					return KeyCode::J;
	case KEY_K_LO_HASH:					return KeyCode::K;
	case KEY_L_LO_HASH:					return KeyCode::L;
	case KEY_M_LO_HASH:					return KeyCode::M;
	case KEY_N_LO_HASH:					return KeyCode::N;
	case KEY_O_LO_HASH:					return KeyCode::O;
	case KEY_P_LO_HASH:					return KeyCode::P;
	case KEY_Q_LO_HASH:					return KeyCode::Q;
	case KEY_R_LO_HASH:					return KeyCode::R;
	case KEY_S_LO_HASH:					return KeyCode::S;
	case KEY_T_LO_HASH:					return KeyCode::T;
	case KEY_U_LO_HASH:					return KeyCode::U;
	case KEY_V_LO_HASH:					return KeyCode::V;
	case KEY_W_LO_HASH:					return KeyCode::W;
	case KEY_X_LO_HASH:					return KeyCode::X;
	case KEY_Y_LO_HASH:					return KeyCode::Y;
	case KEY_Z_LO_HASH:					return KeyCode::Z;


	case KEY_F1_HASH:					return KeyCode::F1;
	case KEY_F2_HASH:					return KeyCode::F2;
	case KEY_F3_HASH:					return KeyCode::F3;
	case KEY_F4_HASH:					return KeyCode::F4;
	case KEY_F5_HASH:					return KeyCode::F5;
	case KEY_F6_HASH:					return KeyCode::F6;
	case KEY_F7_HASH:					return KeyCode::F7;
	case KEY_F8_HASH:					return KeyCode::F8;
	case KEY_F9_HASH:					return KeyCode::F9;
	case KEY_F10_HASH:					return KeyCode::F10;
	case KEY_F11_HASH:					return KeyCode::F11;
	case KEY_F12_HASH:					return KeyCode::F12;

	case KEY_EQUAL_HASH:				return KeyCode::EQUAL;
	case KEY_GRAVE_HASH:				return KeyCode::GRAVEACCENT;
	//case KEY_TILDE_HASH:				return KeyCode::TILDE;
	//case KEY_EXCLAMATION_HASH:		return KeyCode::EXCLAMATION;
	//case KEY_QUESTION_HASH:			return KeyCode::QUESTION;
	//case KEY_AT_SIGN_HASH:			return KeyCode::AT_SIGN;
	//case KEY_NUMBER_SIGN_HASH:		return KeyCode::NUMBER_SIGN;
	//case KEY_DOLLAR_SIGN_HASH:		return KeyCode::DOLLAR_SIGN;
	//case KEY_PERCENT:					return KeyCode::PERCENT;
	//case KEY_CARET_HASH:				return KeyCode::CARET;
	//case KEY_AMPERSAND_HASH:			return KeyCode::AMPERSAND;
	//case KEY_STAR_HASH:				return KeyCode::STAR;
	//case KEY_OPEN_BRACKET_HASH:		return KeyCode::OPEN_BRACKET;
	//case KEY_CLOSE_BRACKET_HASH:		return KeyCode::CLOSE_BRACKET;
	case KEY_MINUS_HASH:				return KeyCode::MINUS;
	//case KEY_UNDERSCORE_HASH:			return KeyCode::UNDERSCORE;
	//case KEY_PLUS_HASH:				return KeyCode::PLUS;
	//case KEY_OPEN_CURLY_HASH:			return KeyCode::OPEN_CURLY;
	//case KEY_CLOSE_CURLY_HASH:		return KeyCode::CLOSE_CURLY;
	//case KEY_OPEN_SQUARE_HASH:		return KeyCode::OPEN_SQUARE;
	//case KEY_CLOSE_SQUARE_HASH:		return KeyCode::CLOSE_SQUARE;
	//case KEY_SEMI_COLON_HASH:			return KeyCode::SEMI_COLON;
	//case KEY_COLON_HASH:				return KeyCode::COLON;
	//case KEY_SINGLE_QUOTE_HASH:		return KeyCode::SINGLE_QUOTE;
	//case KEY_DOUBLE_QUOTE_HASH:		return KeyCode::DOUBLE_QUOTE;
	//case KEY_LESS_THAN_HASH:			return KeyCode::LESS_THAN;
	//case KEY_GREATER_THAN_HASH:		return KeyCode::GREATER_THAN;
	case KEY_COMMA_HASH:				return KeyCode::COMMA;
	//case KEY_DOT_HASH:				return KeyCode::DOT;
	case KEY_SLASH_HASH:				return KeyCode::SLASH;
	case KEY_BACK_SLASH_HASH:			return KeyCode::BACKSLASH;
	//case KEY_VERTICAL_BAR_HASH:		return KeyCode::VERTICAL_BAR;
	case KEY_ESC_KEY_HASH:				return KeyCode::ESCAPE;

	case KEY_TAB_HASH:					return KeyCode::TAB;
	case KEY_SPACE_HASH:				return KeyCode::SPACE;

	case KEY_ESC_HASH:					return KeyCode::ESCAPE;
	case KEY_UP_HASH:					return KeyCode::UP;
	case KEY_DOWN_HASH:					return KeyCode::DOWN;
	case KEY_LEFT_HASH:					return KeyCode::LEFT;
	case KEY_RIGHT_HASH:				return KeyCode::RIGHT;
	case KEY_NUMLOCK_HASH:				return KeyCode::NUMLOCK;
	case KEY_RIGHT_CTRL_HASH:			return KeyCode::RIGHTCONTROL;
	case KEY_LEFT_CTRL_HASH:			return KeyCode::LEFTCONTROL;
	case KEY_RIGHT_SHIFT_HASH:			return KeyCode::RIGHTSHIFT;
	case KEY_LEFT_SHIFT_HASH:			return KeyCode::LEFTSHIFT;
	case KEY_RIGHT_ALT_HASH:			return KeyCode::RIGHTALT;
	case KEY_LEFT_ALT_HASH:				return KeyCode::LEFTALT;
	case KEY_CAPS_HASH:					return KeyCode::CAPSLOCK;

	case KEY_HOME_HASH:					return KeyCode::HOME;
	case KEY_END_HASH:					return KeyCode::END;
	case KEY_DELETE_HASH:				return KeyCode::DELETE;
	//case KEY_LEFT_PLATFORM_BTN_HASH:	return KeyCode::LEFT_PLATOFORM_KEY;
	}

	RX_ERROR("Invalid key name! <{}>", keyName);
	return KeyCode::UNKNOWN;
}
