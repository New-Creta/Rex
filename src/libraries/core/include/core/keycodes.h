#pragma once

#ifdef _WINDOWS
	#undef DELETE
#endif

namespace rex
{
    enum class KeyCode
    {
		UNKNOWN				= -1,

        SHIFT				= 16,
        CONTROL				= 17,

		SPACE               = 32, /*   */
		EXCLAMATION			= 33, /* ! */
		DOUBLE_QUOTE		= 34, /* " */
		NUMBER_SIGN			= 35, /* # */
		DOLLAR_SIGN			= 36, /* $ */
		PERCENT				= 37, /* % */
		AMPERSAND			= 38, /* & */
		APOSTROPHE          = 39, /* ' */
		OPEN_BRACKET		= 40, /* ( */
		CLOSE_BRACKET		= 41, /* ) */
		STAR				= 42, /* * */
		PLUS				= 43, /* + */
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

		COLON				= 58, /* : */ 
		SEMICOLON           = 59, /* ; */
		LESS_THAN			= 60, /* < */
		EQUAL               = 61, /* = */
		GREATER_THAN		= 62, /* > */
		QUESTION			= 63, /* ? */
		AT_SIGN				= 64, /* @ */

		A_HI                = 65,
		B_HI                = 66,
		C_HI                = 67,
		D_HI                = 68,
		E_HI                = 69,
		F_HI                = 70,
		G_HI                = 71,
		H_HI                = 72,
		I_HI                = 73,
		J_HI                = 74,
		K_HI                = 75,
		L_HI                = 76,
		M_HI                = 77,
		N_HI                = 78,
		O_HI                = 79,
		P_HI                = 80,
		Q_HI                = 81,
		R_HI                = 82,
		S_HI                = 83,
		T_HI                = 84,
		U_HI                = 85,
		V_HI                = 86,
		W_HI                = 87,
		X_HI                = 88,
		Y_HI                = 89,
		Z_HI                = 90,

		OPEN_SQUARE			= 91,  /* [ */
		BACKSLASH           = 92,  /* \ */
		CLOSE_SQUARE		= 93,  /* ] */
		CIRCONFLEXE			= 94,  /* ^ */
		UNDERSCORE			= 95,  /* _ */
		GRAVEACCENT         = 96,  /* ` */

        A_LO				= 97,
        B_LO				= 98,
        C_LO				= 99,
        D_LO				= 100,
        E_LO				= 101,
        F_LO				= 102,
        G_LO				= 103,
        H_LO				= 104,
        I_LO				= 105,
        J_LO				= 106,
        K_LO				= 107,
        L_LO				= 108,
        M_LO				= 109,
        N_LO				= 110,
        O_LO				= 111,
        P_LO				= 112,
        Q_LO				= 113,
        R_LO				= 114,
        S_LO				= 115,
        T_LO				= 116,
        U_LO				= 117,
        V_LO				= 118,
        W_LO				= 119,
        X_LO				= 120,
        Y_LO				= 121,
        Z_LO				= 122,

		OPEN_CURLY			= 123, /* { */
		VERTICAL_BAR		= 124, /* | */
		CLOSE_CURLY			= 125, /* | */
		TILDE				= 126, /* ~ */

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
		MENU                = 348,
	};

	//-------------------------------------------------------------------------
    inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
    {
        os << static_cast<uint16>(keyCode);
        return os;
    }
}

#define RX_KEY_SPACE					(int)rex::KeyCode::SPACE               
#define RX_KEY_EXCLAMATION				(int)rex::KeyCode::EXCLAMATION			
#define RX_KEY_DOUBLE_QUOTE				(int)rex::KeyCode::DOUBLE_QUOTE		
#define RX_KEY_NUMBER_SIGN				(int)rex::KeyCode::NUMBER_SIGN			
#define RX_KEY_DOLLAR_SIGN				(int)rex::KeyCode::DOLLAR_SIGN			
#define RX_KEY_PERCENT					(int)rex::KeyCode::PERCENT				
#define RX_KEY_AMPERSAND				(int)rex::KeyCode::AMPERSAND			
#define RX_KEY_APOSTROPHE				(int)rex::KeyCode::APOSTROPHE          
#define RX_KEY_OPEN_BRACKET				(int)rex::KeyCode::OPEN_BRACKET		
#define RX_KEY_CLOSE_BRACKET			(int)rex::KeyCode::CLOSE_BRACKET		
#define RX_KEY_STAR						(int)rex::KeyCode::STAR				
#define RX_KEY_PLUS						(int)rex::KeyCode::PLUS				
#define RX_KEY_COMMA					(int)rex::KeyCode::COMMA               
#define RX_KEY_MINUS					(int)rex::KeyCode::MINUS               
#define RX_KEY_PERIOD					(int)rex::KeyCode::PERIOD              
#define RX_KEY_SLASH					(int)rex::KeyCode::SLASH               
										
#define RX_KEY_D0						(int)rex::KeyCode::D0                  
#define RX_KEY_D1						(int)rex::KeyCode::D1                  
#define RX_KEY_D2						(int)rex::KeyCode::D2                  
#define RX_KEY_D3						(int)rex::KeyCode::D3                  
#define RX_KEY_D4						(int)rex::KeyCode::D4                  
#define RX_KEY_D5						(int)rex::KeyCode::D5                  
#define RX_KEY_D6						(int)rex::KeyCode::D6                  
#define RX_KEY_D7						(int)rex::KeyCode::D7                  
#define RX_KEY_D8						(int)rex::KeyCode::D8                  
#define RX_KEY_D9						(int)rex::KeyCode::D9                  
										
#define RX_KEY_COLON					(int)rex::KeyCode::COLON				
#define RX_KEY_SEMICOLON				(int)rex::KeyCode::SEMICOLON           
#define RX_KEY_LESS_THAN				(int)rex::KeyCode::LESS_THAN			
#define RX_KEY_EQUAL					(int)rex::KeyCode::EQUAL               
#define RX_KEY_GREATER_THAN				(int)rex::KeyCode::GREATER_THAN		
#define RX_KEY_QUESTION					(int)rex::KeyCode::QUESTION			
#define RX_KEY_AT_SIGN					(int)rex::KeyCode::AT_SIGN				
										
#define RX_KEY_A_HI						(int)rex::KeyCode::A_HI                
#define RX_KEY_B_HI						(int)rex::KeyCode::B_HI                
#define RX_KEY_C_HI						(int)rex::KeyCode::C_HI                
#define RX_KEY_D_HI						(int)rex::KeyCode::D_HI                
#define RX_KEY_E_HI						(int)rex::KeyCode::E_HI                
#define RX_KEY_F_HI						(int)rex::KeyCode::F_HI                
#define RX_KEY_G_HI						(int)rex::KeyCode::G_HI                
#define RX_KEY_H_HI						(int)rex::KeyCode::H_HI                
#define RX_KEY_I_HI						(int)rex::KeyCode::I_HI                
#define RX_KEY_J_HI						(int)rex::KeyCode::J_HI                
#define RX_KEY_K_HI						(int)rex::KeyCode::K_HI                
#define RX_KEY_L_HI						(int)rex::KeyCode::L_HI                
#define RX_KEY_M_HI						(int)rex::KeyCode::M_HI                
#define RX_KEY_N_HI						(int)rex::KeyCode::N_HI                
#define RX_KEY_O_HI						(int)rex::KeyCode::O_HI                
#define RX_KEY_P_HI						(int)rex::KeyCode::P_HI                
#define RX_KEY_Q_HI						(int)rex::KeyCode::Q_HI                
#define RX_KEY_R_HI						(int)rex::KeyCode::R_HI                
#define RX_KEY_S_HI						(int)rex::KeyCode::S_HI                
#define RX_KEY_T_HI						(int)rex::KeyCode::T_HI                
#define RX_KEY_U_HI						(int)rex::KeyCode::U_HI                
#define RX_KEY_V_HI						(int)rex::KeyCode::V_HI                
#define RX_KEY_W_HI						(int)rex::KeyCode::W_HI                
#define RX_KEY_X_HI						(int)rex::KeyCode::X_HI                
#define RX_KEY_Y_HI						(int)rex::KeyCode::Y_HI                
#define RX_KEY_Z_HI						(int)rex::KeyCode::Z_HI                
										
#define RX_KEY_OPEN_SQUARE				(int)rex::KeyCode::OPEN_SQUARE			
#define RX_KEY_BACKSLASH				(int)rex::KeyCode::BACKSLASH           
#define RX_KEY_CLOSE_SQUARE				(int)rex::KeyCode::CLOSE_SQUARE		
#define RX_KEY_CIRCONFLEXE				(int)rex::KeyCode::CIRCONFLEXE			
#define RX_KEY_UNDERSCORE				(int)rex::KeyCode::UNDERSCORE			
#define RX_KEY_GRAVEACCENT				(int)rex::KeyCode::GRAVEACCENT         
										
#define RX_KEY_A_LO						(int)rex::KeyCode::A_LO				
#define RX_KEY_B_LO						(int)rex::KeyCode::B_LO				
#define RX_KEY_C_LO						(int)rex::KeyCode::C_LO				
#define RX_KEY_D_LO						(int)rex::KeyCode::D_LO				
#define RX_KEY_E_LO						(int)rex::KeyCode::E_LO				
#define RX_KEY_F_LO						(int)rex::KeyCode::F_LO				
#define RX_KEY_G_LO						(int)rex::KeyCode::G_LO				
#define RX_KEY_H_LO						(int)rex::KeyCode::H_LO				
#define RX_KEY_I_LO						(int)rex::KeyCode::I_LO				
#define RX_KEY_J_LO						(int)rex::KeyCode::J_LO				
#define RX_KEY_K_LO						(int)rex::KeyCode::K_LO				
#define RX_KEY_L_LO						(int)rex::KeyCode::L_LO				
#define RX_KEY_M_LO						(int)rex::KeyCode::M_LO				
#define RX_KEY_N_LO						(int)rex::KeyCode::N_LO				
#define RX_KEY_O_LO						(int)rex::KeyCode::O_LO				
#define RX_KEY_P_LO						(int)rex::KeyCode::P_LO				
#define RX_KEY_Q_LO						(int)rex::KeyCode::Q_LO				
#define RX_KEY_R_LO						(int)rex::KeyCode::R_LO				
#define RX_KEY_S_LO						(int)rex::KeyCode::S_LO				
#define RX_KEY_T_LO						(int)rex::KeyCode::T_LO				
#define RX_KEY_U_LO						(int)rex::KeyCode::U_LO				
#define RX_KEY_V_LO						(int)rex::KeyCode::V_LO				
#define RX_KEY_W_LO						(int)rex::KeyCode::W_LO				
#define RX_KEY_X_LO						(int)rex::KeyCode::X_LO				
#define RX_KEY_Y_LO						(int)rex::KeyCode::Y_LO				
#define RX_KEY_Z_LO						(int)rex::KeyCode::Z_LO				
										
#define RX_KEY_OPEN_CURLY				(int)rex::KeyCode::OPEN_CURLY			
#define RX_KEY_VERTICAL_BAR				(int)rex::KeyCode::VERTICAL_BAR		
#define RX_KEY_CLOSE_CURLY				(int)rex::KeyCode::CLOSE_CURLY			
#define RX_KEY_TILDE					(int)rex::KeyCode::TILDE				
										
#define RX_KEY_WORLD1					(int)rex::KeyCode::WORLD1              
#define RX_KEY_WORLD2					(int)rex::KeyCode::WORLD2              

/* FUNCTION KEYS */
#define RX_KEY_ESCAPE					(int)rex::KeyCode::ESCAPE              
#define RX_KEY_ENTER					(int)rex::KeyCode::ENTER               
#define RX_KEY_TAB						(int)rex::KeyCode::TAB                 
#define RX_KEY_BACKSPACE				(int)rex::KeyCode::BACKSPACE           
#define RX_KEY_INSERT					(int)rex::KeyCode::INSERT              
#define RX_KEY_DELETE					(int)rex::KeyCode::DELETE              
#define RX_KEY_RIGHT					(int)rex::KeyCode::RIGHT               
#define RX_KEY_LEFT						(int)rex::KeyCode::LEFT                
#define RX_KEY_DOWN						(int)rex::KeyCode::DOWN                
#define RX_KEY_UP						(int)rex::KeyCode::UP                  
#define RX_KEY_PAGEUP					(int)rex::KeyCode::PAGEUP              
#define RX_KEY_PAGEDOWN					(int)rex::KeyCode::PAGEDOWN            
#define RX_KEY_HOME						(int)rex::KeyCode::HOME                
#define RX_KEY_END						(int)rex::KeyCode::END                 
#define RX_KEY_CAPSLOCK					(int)rex::KeyCode::CAPSLOCK            
#define RX_KEY_SCROLLLOCK				(int)rex::KeyCode::SCROLLLOCK          
#define RX_KEY_NUMLOCK					(int)rex::KeyCode::NUMLOCK             
#define RX_KEY_PRINTSCREEN				(int)rex::KeyCode::PRINTSCREEN         
#define RX_KEY_PAUSE					(int)rex::KeyCode::PAUSE               
#define RX_KEY_F1						(int)rex::KeyCode::F1                  
#define RX_KEY_F2						(int)rex::KeyCode::F2                  
#define RX_KEY_F3						(int)rex::KeyCode::F3                  
#define RX_KEY_F4						(int)rex::KeyCode::F4                  
#define RX_KEY_F5						(int)rex::KeyCode::F5                  
#define RX_KEY_F6						(int)rex::KeyCode::F6                  
#define RX_KEY_F7						(int)rex::KeyCode::F7                  
#define RX_KEY_F8						(int)rex::KeyCode::F8                  
#define RX_KEY_F9						(int)rex::KeyCode::F9                  
#define RX_KEY_F10						(int)rex::KeyCode::F10                 
#define RX_KEY_F11						(int)rex::KeyCode::F11                 
#define RX_KEY_F12						(int)rex::KeyCode::F12                 
#define RX_KEY_F13						(int)rex::KeyCode::F13                 
#define RX_KEY_F14						(int)rex::KeyCode::F14                 
#define RX_KEY_F15						(int)rex::KeyCode::F15                 
#define RX_KEY_F16						(int)rex::KeyCode::F16                 
#define RX_KEY_F17						(int)rex::KeyCode::F17                 
#define RX_KEY_F18						(int)rex::KeyCode::F18                 
#define RX_KEY_F19						(int)rex::KeyCode::F19                 
#define RX_KEY_F20						(int)rex::KeyCode::F20                 
#define RX_KEY_F21						(int)rex::KeyCode::F21                 
#define RX_KEY_F22						(int)rex::KeyCode::F22                 
#define RX_KEY_F23						(int)rex::KeyCode::F23                 
#define RX_KEY_F24						(int)rex::KeyCode::F24                 
#define RX_KEY_F25						(int)rex::KeyCode::F25                 

#define RX_KEY_LEFTSHIFT				(int)rex::KeyCode::LEFTSHIFT           
#define RX_KEY_LEFTCONTROL				(int)rex::KeyCode::LEFTCONTROL         
#define RX_KEY_LEFTALT					(int)rex::KeyCode::LEFTALT             
#define RX_KEY_LEFTSUPER				(int)rex::KeyCode::LEFTSUPER           
#define RX_KEY_RIGHTSHIFT				(int)rex::KeyCode::RIGHTSHIFT          
#define RX_KEY_RIGHTCONTROL				(int)rex::KeyCode::RIGHTCONTROL        
#define RX_KEY_RIGHTALT					(int)rex::KeyCode::RIGHTALT            
#define RX_KEY_RIGHTSUPER				(int)rex::KeyCode::RIGHTSUPER          
#define RX_KEY_MENU						(int)rex::KeyCode::MENU                

/* KEYPAD */
#define RX_KEY_KP0						(int)rex::KeyCode::KP0                 
#define RX_KEY_KP1						(int)rex::KeyCode::KP1                 
#define RX_KEY_KP2						(int)rex::KeyCode::KP2                 
#define RX_KEY_KP3						(int)rex::KeyCode::KP3                 
#define RX_KEY_KP4						(int)rex::KeyCode::KP4                 
#define RX_KEY_KP5						(int)rex::KeyCode::KP5                 
#define RX_KEY_KP6						(int)rex::KeyCode::KP6                 
#define RX_KEY_KP7						(int)rex::KeyCode::KP7                 
#define RX_KEY_KP8						(int)rex::KeyCode::KP8                 
#define RX_KEY_KP9						(int)rex::KeyCode::KP9                 
#define RX_KEY_KPDECIMAL				(int)rex::KeyCode::KPDECIMAL           
#define RX_KEY_KPDIVIDE					(int)rex::KeyCode::KPDIVIDE            
#define RX_KEY_KPMULTIPLY				(int)rex::KeyCode::KPMULTIPLY          
#define RX_KEY_KPSUBTRACT				(int)rex::KeyCode::KPSUBTRACT          
#define RX_KEY_KPADD					(int)rex::KeyCode::KPADD               
#define RX_KEY_KPENTER					(int)rex::KeyCode::KPENTER             
#define RX_KEY_KPEQUAL					(int)rex::KeyCode::KPEQUAL             