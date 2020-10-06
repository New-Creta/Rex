#pragma once

#ifdef _WINDOWS
#undef DELETE
#endif

namespace rex
{
#pragma  region OLD_KEYCODES

       //enum class KeyCode
       //{
       //	UNKNOWN				= -1,
       //
       //   SHIFT				= 16,
       //   CONTROL				= 17,
       //
       //	SPACE               = 32, /*   */
       //	EXCLAMATION			= 33, /* ! */
       //	DOUBLE_QUOTE		= 34, /* " */
       //	NUMBER_SIGN			= 35, /* # */
       //	DOLLAR_SIGN			= 36, /* $ */
       //	PERCENT				= 37, /* % */
       //	AMPERSAND			= 38, /* & */
       //	APOSTROPHE          = 39, /* ' */
       //	OPEN_BRACKET		= 40, /* ( */
       //	CLOSE_BRACKET		= 41, /* ) */
       //	STAR				= 42, /* * */
       //	PLUS				= 43, /* + */
       //	COMMA               = 44, /* , */
       //	MINUS               = 45, /* - */
       //	PERIOD              = 46, /* . */
       //	SLASH               = 47, /* / */
       //
       //	D0                  = 48, /* 0 */
       //	D1                  = 49, /* 1 */
       //	D2                  = 50, /* 2 */
       //	D3                  = 51, /* 3 */
       //	D4                  = 52, /* 4 */
       //	D5                  = 53, /* 5 */
       //	D6                  = 54, /* 6 */
       //	D7                  = 55, /* 7 */
       //	D8                  = 56, /* 8 */
       //	D9                  = 57, /* 9 */
       //
       //	COLON				= 58, /* : */ 
       //	SEMICOLON           = 59, /* ; */
       //	LESS_THAN			= 60, /* < */
       //	EQUAL               = 61, /* = */
       //	GREATER_THAN		= 62, /* > */
       //	QUESTION			= 63, /* ? */
       //	AT_SIGN				= 64, /* @ */
       //
       //	A_HI                = 65,
       //	B_HI                = 66,
       //	C_HI                = 67,
       //	D_HI                = 68,
       //	E_HI                = 69,
       //	F_HI                = 70,
       //	G_HI                = 71,
       //	H_HI                = 72,
       //	I_HI                = 73,
       //	J_HI                = 74,
       //	K_HI                = 75,
       //	L_HI                = 76,
       //	M_HI                = 77,
       //	N_HI                = 78,
       //	O_HI                = 79,
       //	P_HI                = 80,
       //	Q_HI                = 81,
       //	R_HI                = 82,
       //	S_HI                = 83,
       //	T_HI                = 84,
       //	U_HI                = 85,
       //	V_HI                = 86,
       //	W_HI                = 87,
       //	X_HI                = 88,
       //	Y_HI                = 89,
       //	Z_HI                = 90,
       //
       //	OPEN_SQUARE			= 91,  /* [ */
       //	BACKSLASH           = 92,  /* \ */
       //	CLOSE_SQUARE		= 93,  /* ] */
       //	CIRCONFLEXE			= 94,  /* ^ */
       //	UNDERSCORE			= 95,  /* _ */
       //	GRAVEACCENT         = 96,  /* ` */
       //
       //   A_LO				= 97,
       //   B_LO				= 98,
       //   C_LO				= 99,
       //   D_LO				= 100,
       //   E_LO				= 101,
       //   F_LO				= 102,
       //   G_LO				= 103,
       //   H_LO				= 104,
       //   I_LO				= 105,
       //   J_LO				= 106,
       //   K_LO				= 107,
       //   L_LO				= 108,
       //   M_LO				= 109,
       //   N_LO				= 110,
       //   O_LO				= 111,
       //   P_LO				= 112,
       //   Q_LO				= 113,
       //   R_LO				= 114,
       //   S_LO				= 115,
       //   T_LO				= 116,
       //   U_LO				= 117,
       //   V_LO				= 118,
       //   W_LO				= 119,
       //   X_LO				= 120,
       //   Y_LO				= 121,
       //   Z_LO				= 122,
       //
       //	OPEN_CURLY			= 123, /* { */
       //	VERTICAL_BAR		= 124, /* | */
       //	CLOSE_CURLY			= 125, /* | */
       //	TILDE				= 126, /* ~ */
       //
       //	WORLD1              = 161, /* non-US #1 */
       //	WORLD2              = 162, /* non-US #2 */
       //
       //	/* FUNCTION KEYS */
       //	ESCAPE              = 256,
       //	ENTER               = 257,
       //	TAB                 = 258,
       //	BACKSPACE           = 259,
       //	INSERT              = 260,
       //	DELETE              = 261,
       //	RIGHT               = 262,
       //	LEFT                = 263,
       //	DOWN                = 264,
       //	UP                  = 265,
       //	PAGEUP              = 266,
       //	PAGEDOWN            = 267,
       //	HOME                = 268,
       //	END                 = 269,
       //	CAPSLOCK            = 280,
       //	SCROLLLOCK          = 281,
       //	NUMLOCK             = 282,
       //	PRINTSCREEN         = 283,
       //	PAUSE               = 284,
       //	F1                  = 290,
       //	F2                  = 291,
       //	F3                  = 292,
       //	F4                  = 293,
       //	F5                  = 294,
       //	F6                  = 295,
       //	F7                  = 296,
       //	F8                  = 297,
       //	F9                  = 298,
       //	F10                 = 299,
       //	F11                 = 300,
       //	F12                 = 301,
       //	F13                 = 302,
       //	F14                 = 303,
       //	F15                 = 304,
       //	F16                 = 305,
       //	F17                 = 306,
       //	F18                 = 307,
       //	F19                 = 308,
       //	F20                 = 309,
       //	F21                 = 310,
       //	F22                 = 311,
       //	F23                 = 312,
       //	F24                 = 313,
       //	F25                 = 314,
       //
       //	/* KEYPAD */
       //	KP0                 = 320,
       //	KP1                 = 321,
       //	KP2                 = 322,
       //	KP3                 = 323,
       //	KP4                 = 324,
       //	KP5                 = 325,
       //	KP6                 = 326,
       //	KP7                 = 327,
       //	KP8                 = 328,
       //	KP9                 = 329,
       //	KPDECIMAL           = 330,
       //	KPDIVIDE            = 331,
       //	KPMULTIPLY          = 332,
       //	KPSUBTRACT          = 333,
       //	KPADD               = 334,
       //	KPENTER             = 335,
       //	KPEQUAL             = 336,
       //
       //	LEFTSHIFT           = 340,
       //	LEFTCONTROL         = 341,
       //	LEFTALT             = 342,
       //	LEFTSUPER           = 343,
       //	RIGHTSHIFT          = 344,
       //	RIGHTCONTROL        = 345,
       //	RIGHTALT            = 346,
       //	RIGHTSUPER          = 347,
       //	MENU                = 348,
       //};

#pragma endregion

    enum class KeyCode
    {
        UNKNOWN        =   -1,

        BACK           = 0x08,
        TAB            = 0x09,
                             
        CLEAR          = 0x0C,
        RETURN         = 0x0D,
                             
        SHIFT          = 0x10,
        CONTROL        = 0x11,
        MENU           = 0x12,
        PAUSE          = 0x13,
        CAPITAL        = 0x14,
        KANA           = 0x15,
        HANGEUL        = 0x15,  /* old name - should be here for compatibility */
        HANGUL         = 0x15,
        JUNJA          = 0x17,
        FINAL          = 0x18,
        HANJA          = 0x19,
        KANJI          = 0x19,
        ESCAPE         = 0x1B,
                             
        CONVERT        = 0x1C,
        NONCONVERT     = 0x1D,
        ACCEPT         = 0x1E,
        MODECHANGE     = 0x1F,
                             
        SPACE          = 0x20,
        PRIOR          = 0x21,
        NEXT           = 0x22,
        END            = 0x23,
        HOME           = 0x24,
        LEFT           = 0x25,
        UP             = 0x26,
        RIGHT          = 0x27,
        DOWN           = 0x28,
        SELECT         = 0x29,
        PRINT          = 0x2A,
        EXECUTE        = 0x2B,
        SNAPSHOT       = 0x2C,
        INSERT         = 0x2D,
        DELETE         = 0x2E,
        HELP           = 0x2F,
        LWIN           = 0x5B,
        RWIN           = 0x5C,
        APPS           = 0x5D,
                             
        SLEEP          = 0x5F,
                             
        NUMPAD0        = 0x60,
        NUMPAD1        = 0x61,
        NUMPAD2        = 0x62,
        NUMPAD3        = 0x63,
        NUMPAD4        = 0x64,
        NUMPAD5        = 0x65,
        NUMPAD6        = 0x66,
        NUMPAD7        = 0x67,
        NUMPAD8        = 0x68,
        NUMPAD9        = 0x69,
        MULTIPLY       = 0x6A,
        ADD            = 0x6B,
        SEPARATOR      = 0x6C,
        SUBTRACT       = 0x6D,
        DECIMAL        = 0x6E,
        DIVIDE         = 0x6F,
        F1             = 0x70,
        F2             = 0x71,
        F3             = 0x72,
        F4             = 0x73,
        F5             = 0x74,
        F6             = 0x75,
        F7             = 0x76,
        F8             = 0x77,
        F9             = 0x78,
        F10            = 0x79,
        F11            = 0x7A,
        F12            = 0x7B,
        F13            = 0x7C,
        F14            = 0x7D,
        F15            = 0x7E,
        F16            = 0x7F,
        F17            = 0x80,
        F18            = 0x81,
        F19            = 0x82,
        F20            = 0x83,
        F21            = 0x84,
        F22            = 0x85,
        F23            = 0x86,
        F24            = 0x87,
                             
        NUMLOCK        = 0x90,
        SCROLL         = 0x91,
                             
        LSHIFT         = 0xA0,
        RSHIFT         = 0xA1,
        LCONTROL       = 0xA2,
        RCONTROL       = 0xA3,
        LMENU          = 0xA4,
        RMENU          = 0xA5,

        KEY_A          = 0x41,
        KEY_B          = 0x42,
        KEY_C          = 0x43,
        KEY_D          = 0x44,
        KEY_E          = 0x45,
        KEY_F          = 0x46,
        KEY_G          = 0x47,
        KEY_H          = 0x48,
        KEY_I          = 0x49,
        KEY_J          = 0x4A,
        KEY_K          = 0x4B,
        KEY_L          = 0x4C,
        KEY_M          = 0x4D,
        KEY_N          = 0x4E,
        KEY_O          = 0x4F,
        KEY_P          = 0x50,
        KEY_Q          = 0x51,
        KEY_R          = 0x52,
        KEY_S          = 0x53,
        KEY_T          = 0x54,
        KEY_U          = 0x55,
        KEY_V          = 0x56,
        KEY_W          = 0x57,
        KEY_X          = 0x58,
        KEY_Y          = 0x59,
        KEY_Z          = 0x5A,

        KEY_0          = 0x30,
        KEY_1          = 0x31,
        KEY_2          = 0x32,
        KEY_3          = 0x33,
        KEY_4          = 0x34,
        KEY_5          = 0x35,
        KEY_6          = 0x36,
        KEY_7          = 0x37,
        KEY_8          = 0x38,
        KEY_9          = 0x39,
    };

    //-------------------------------------------------------------------------
    inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
    {
        os << static_cast<uint16>(keyCode);
        return os;
    }
}

#define RX_KEY_UNKNOWN        	(int)rex::KeyCode::UNKNOWN        

#define RX_KEY_BACK           	(int)rex::KeyCode::BACK           
#define RX_KEY_TAB            	(int)rex::KeyCode::TAB            

#define RX_KEY_CLEAR          	(int)rex::KeyCode::CLEAR          
#define RX_KEY_RETURN         	(int)rex::KeyCode::RETURN         

#define RX_KEY_SHIFT          	(int)rex::KeyCode::SHIFT          
#define RX_KEY_CONTROL        	(int)rex::KeyCode::CONTROL        
#define RX_KEY_MENU           	(int)rex::KeyCode::MENU           
#define RX_KEY_PAUSE          	(int)rex::KeyCode::PAUSE          
#define RX_KEY_CAPITAL        	(int)rex::KeyCode::CAPITAL        
#define RX_KEY_KANA           	(int)rex::KeyCode::KANA           
#define RX_KEY_HANGEUL        	(int)rex::KeyCode::HANGEUL        
#define RX_KEY_HANGUL         	(int)rex::KeyCode::HANGUL         
#define RX_KEY_JUNJA          	(int)rex::KeyCode::JUNJA          
#define RX_KEY_FINAL          	(int)rex::KeyCode::FINAL          
#define RX_KEY_HANJA          	(int)rex::KeyCode::HANJA          
#define RX_KEY_KANJI          	(int)rex::KeyCode::KANJI          
#define RX_KEY_ESCAPE         	(int)rex::KeyCode::ESCAPE         

#define RX_KEY_CONVERT        	(int)rex::KeyCode::CONVERT        
#define RX_KEY_NONCONVERT     	(int)rex::KeyCode::NONCONVERT     
#define RX_KEY_ACCEPT         	(int)rex::KeyCode::ACCEPT         
#define RX_KEY_MODECHANGE     	(int)rex::KeyCode::MODECHANGE     

#define RX_KEY_SPACE          	(int)rex::KeyCode::SPACE          
#define RX_KEY_PRIOR          	(int)rex::KeyCode::PRIOR          
#define RX_KEY_NEXT           	(int)rex::KeyCode::NEXT           
#define RX_KEY_END            	(int)rex::KeyCode::END            
#define RX_KEY_HOME           	(int)rex::KeyCode::HOME           
#define RX_KEY_LEFT           	(int)rex::KeyCode::LEFT           
#define RX_KEY_UP             	(int)rex::KeyCode::UP             
#define RX_KEY_RIGHT          	(int)rex::KeyCode::RIGHT          
#define RX_KEY_DOWN           	(int)rex::KeyCode::DOWN           
#define RX_KEY_SELECT         	(int)rex::KeyCode::SELECT         
#define RX_KEY_PRINT          	(int)rex::KeyCode::PRINT          
#define RX_KEY_EXECUTE        	(int)rex::KeyCode::EXECUTE        
#define RX_KEY_SNAPSHOT       	(int)rex::KeyCode::SNAPSHOT       
#define RX_KEY_INSERT         	(int)rex::KeyCode::INSERT         
#define RX_KEY_DELETE         	(int)rex::KeyCode::DELETE         
#define RX_KEY_HELP           	(int)rex::KeyCode::HELP           
#define RX_KEY_LWIN           	(int)rex::KeyCode::LWIN           
#define RX_KEY_RWIN           	(int)rex::KeyCode::RWIN           
#define RX_KEY_APPS           	(int)rex::KeyCode::APPS           

#define RX_KEY_SLEEP          	(int)rex::KeyCode::SLEEP          

#define RX_KEY_NUMPAD0        	(int)rex::KeyCode::NUMPAD0        
#define RX_KEY_NUMPAD1        	(int)rex::KeyCode::NUMPAD1        
#define RX_KEY_NUMPAD2        	(int)rex::KeyCode::NUMPAD2        
#define RX_KEY_NUMPAD3        	(int)rex::KeyCode::NUMPAD3        
#define RX_KEY_NUMPAD4        	(int)rex::KeyCode::NUMPAD4        
#define RX_KEY_NUMPAD5        	(int)rex::KeyCode::NUMPAD5        
#define RX_KEY_NUMPAD6        	(int)rex::KeyCode::NUMPAD6        
#define RX_KEY_NUMPAD7        	(int)rex::KeyCode::NUMPAD7        
#define RX_KEY_NUMPAD8        	(int)rex::KeyCode::NUMPAD8        
#define RX_KEY_NUMPAD9        	(int)rex::KeyCode::NUMPAD9        
#define RX_KEY_MULTIPLY       	(int)rex::KeyCode::MULTIPLY       
#define RX_KEY_ADD            	(int)rex::KeyCode::ADD            
#define RX_KEY_SEPARATOR      	(int)rex::KeyCode::SEPARATOR      
#define RX_KEY_SUBTRACT       	(int)rex::KeyCode::SUBTRACT       
#define RX_KEY_DECIMAL        	(int)rex::KeyCode::DECIMAL        
#define RX_KEY_DIVIDE         	(int)rex::KeyCode::DIVIDE         
#define RX_KEY_F1             	(int)rex::KeyCode::F1             
#define RX_KEY_F2             	(int)rex::KeyCode::F2             
#define RX_KEY_F3             	(int)rex::KeyCode::F3             
#define RX_KEY_F4             	(int)rex::KeyCode::F4             
#define RX_KEY_F5             	(int)rex::KeyCode::F5             
#define RX_KEY_F6             	(int)rex::KeyCode::F6             
#define RX_KEY_F7             	(int)rex::KeyCode::F7             
#define RX_KEY_F8             	(int)rex::KeyCode::F8             
#define RX_KEY_F9             	(int)rex::KeyCode::F9             
#define RX_KEY_F10            	(int)rex::KeyCode::F10            
#define RX_KEY_F11            	(int)rex::KeyCode::F11            
#define RX_KEY_F12            	(int)rex::KeyCode::F12            
#define RX_KEY_F13            	(int)rex::KeyCode::F13            
#define RX_KEY_F14            	(int)rex::KeyCode::F14            
#define RX_KEY_F15            	(int)rex::KeyCode::F15            
#define RX_KEY_F16            	(int)rex::KeyCode::F16            
#define RX_KEY_F17            	(int)rex::KeyCode::F17            
#define RX_KEY_F18            	(int)rex::KeyCode::F18            
#define RX_KEY_F19            	(int)rex::KeyCode::F19            
#define RX_KEY_F20            	(int)rex::KeyCode::F20            
#define RX_KEY_F21            	(int)rex::KeyCode::F21            
#define RX_KEY_F22            	(int)rex::KeyCode::F22            
#define RX_KEY_F23            	(int)rex::KeyCode::F23            
#define RX_KEY_F24            	(int)rex::KeyCode::F24            

#define RX_KEY_NUMLOCK        	(int)rex::KeyCode::NUMLOCK        
#define RX_KEY_SCROLL         	(int)rex::KeyCode::SCROLL         

#define RX_KEY_LSHIFT         	(int)rex::KeyCode::LSHIFT         
#define RX_KEY_RSHIFT         	(int)rex::KeyCode::RSHIFT         
#define RX_KEY_LCONTROL       	(int)rex::KeyCode::LCONTROL       
#define RX_KEY_RCONTROL       	(int)rex::KeyCode::RCONTROL       
#define RX_KEY_LMENU            (int)rex::KeyCode::LMENU
#define RX_KEY_RMENU          	(int)rex::KeyCode::RMENU       

#define RX_KEY_A     	        (int)rex::KeyCode::KEY_A      
#define RX_KEY_B     	        (int)rex::KeyCode::KEY_B      
#define RX_KEY_C     	        (int)rex::KeyCode::KEY_C      
#define RX_KEY_D     	        (int)rex::KeyCode::KEY_D      
#define RX_KEY_E     	        (int)rex::KeyCode::KEY_E      
#define RX_KEY_F     	        (int)rex::KeyCode::KEY_F      
#define RX_KEY_G     	        (int)rex::KeyCode::KEY_G      
#define RX_KEY_H     	        (int)rex::KeyCode::KEY_H      
#define RX_KEY_I     	        (int)rex::KeyCode::KEY_I      
#define RX_KEY_J     	        (int)rex::KeyCode::KEY_J      
#define RX_KEY_K     	        (int)rex::KeyCode::KEY_K      
#define RX_KEY_L     	        (int)rex::KeyCode::KEY_L      
#define RX_KEY_M     	        (int)rex::KeyCode::KEY_M      
#define RX_KEY_N     	        (int)rex::KeyCode::KEY_N      
#define RX_KEY_O     	        (int)rex::KeyCode::KEY_O      
#define RX_KEY_P     	        (int)rex::KeyCode::KEY_P      
#define RX_KEY_Q     	        (int)rex::KeyCode::KEY_Q      
#define RX_KEY_R     	        (int)rex::KeyCode::KEY_R      
#define RX_KEY_S     	        (int)rex::KeyCode::KEY_S      
#define RX_KEY_T     	        (int)rex::KeyCode::KEY_T      
#define RX_KEY_U     	        (int)rex::KeyCode::KEY_U      
#define RX_KEY_V     	        (int)rex::KeyCode::KEY_V      
#define RX_KEY_W     	        (int)rex::KeyCode::KEY_W      
#define RX_KEY_X     	        (int)rex::KeyCode::KEY_X      
#define RX_KEY_Y     	        (int)rex::KeyCode::KEY_Y      
#define RX_KEY_Z     	        (int)rex::KeyCode::KEY_Z      

#define RX_KEY_0            	(int)rex::KeyCode::KEY_0      
#define RX_KEY_1            	(int)rex::KeyCode::KEY_1      
#define RX_KEY_2            	(int)rex::KeyCode::KEY_2      
#define RX_KEY_3            	(int)rex::KeyCode::KEY_3      
#define RX_KEY_4            	(int)rex::KeyCode::KEY_4      
#define RX_KEY_5            	(int)rex::KeyCode::KEY_5      
#define RX_KEY_6            	(int)rex::KeyCode::KEY_6      
#define RX_KEY_7            	(int)rex::KeyCode::KEY_7      
#define RX_KEY_8            	(int)rex::KeyCode::KEY_8      
#define RX_KEY_9            	(int)rex::KeyCode::KEY_9      
