#if !defined(DIRECT_INPUT_HEADER)
#define DIRECT_INPUT_HEADER

#define DIRECTINPUT_VERSION 0x800

#include <dinput.h>

// Input Types. Applies to keyboard keys, mouse & joystick buttons.
#define KEY_NONE			0
#define KEY_RELEASE			1
#define KEY_PRESS			2
#define KEY_REPEAT			3

// Input Key constants
enum
{
	KEY_NULL			= 0x00000000,
    KEY_BACKSPACE		= 0x00000008,
    KEY_TAB				= 0x00000009,
    KEY_ENTER			= 0x0000000D,
    KEY_ESCAPE			= 0x0000001B,
    KEY_SPACE			= 0x00000020,

	// Printable characters
	KEY_BANG,
	KEY_DQUOTE,
	KEY_POUND,
	KEY_DOLLAR,
	KEY_PERCENT,
	KEY_AMPERSAND,
	KEY_APOSTROPHE,
	KEY_LBRACE,
	KEY_RBRACE,
	KEY_ASTERISC,
	KEY_PLUS,
	KEY_COMMA,
	KEY_DASH,
	KEY_PERIOD,
	KEY_SLASH,
    KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_COLON,
	KEY_SEMICOLON,
	KEY_LESSTHEN,
	KEY_EQUALS,
	KEY_GREATERTHEN,
	KEY_QMARK,
	KEY_AT,
	KEY_CA,
	KEY_CB,
	KEY_CC,
	KEY_CD,
	KEY_CE,
	KEY_CF,
	KEY_CG,
	KEY_CH,
	KEY_CI,
	KEY_CJ,
	KEY_CK,
	KEY_CL,
	KEY_CM,
	KEY_CN,
	KEY_CO,
	KEY_CP,
	KEY_CQ,
	KEY_CR,
	KEY_CS,
	KEY_CT,
	KEY_CU,
	KEY_CV,
	KEY_CW,
	KEY_CX,
	KEY_CY,
	KEY_CZ,
	KEY_LBRACKET,
	KEY_BACKSLASH,
	KEY_RBRACKET,
	KEY_CARETE,
	KEY_UNDERSCORE,
	KEY_GRAVE,
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_LCURLY,
	KEY_PIPE,
	KEY_RCURLY,
	KEY_TILDA,
	KEY_DELETE,

	// modifier keys
    KEY_LEFTSHIFT		= 0x00000080,
    KEY_RIGHTSHIFT,
	KEY_LEFTCTRL,
	KEY_RIGHTCTRL,
    KEY_LEFTALT,
    KEY_RIGHTALT,
	
	// arrow keys
	KEY_LEFTARROW,
    KEY_RIGHTARROW,
    KEY_UPARROW,
    KEY_DOWNARROW,
    
	// function keys
	KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,

	// cursor control keys
    KEY_INS,
    KEY_DEL,
    KEY_HOME,
    KEY_END,
    KEY_PGUP,
    KEY_PGDN,
	
	// numeric keypad
    KEY_NUMSLASH,
    KEY_NUMSTAR,
    KEY_NUMMINUS,
    KEY_NUMPLUS,
    KEY_NUMENTER,
    KEY_NUMPERIOD,
    KEY_NUM0,
    KEY_NUM1,
    KEY_NUM2,
    KEY_NUM3,
    KEY_NUM4,
    KEY_NUM5,
    KEY_NUM6,
    KEY_NUM7,
    KEY_NUM8,
    KEY_NUM9,
    
	// locks and misc keys
	KEY_NUMLOCK,
    KEY_CAPSLOCK,
    KEY_SCROLLLOCK,
    KEY_PRINTSCRN,
    KEY_PAUSE,
	
	// windows keys 
	KEY_LWIN,
	KEY_RWIN,

	// characters 256 and up used for mouse and joystick buttons etc.
	KEY_MOUSELEFT		= 0x00000100,
	KEY_MOUSERIGHT,
	KEY_MOUSEMIDDLE,
	KEY_MOUSEBUTN4,


	KEY_LSTICKLEFT,
	KEY_LSTICKRIGHT,
	KEY_LSTICKUP,
	KEY_LSTICKDOWN,

	KEY_RSTICKLEFT,
	KEY_RSTICKRIGHT,
	KEY_RSTICKUP,
	KEY_RSTICKDOWN,

	KEY_JOYBUTN1,
	KEY_JOYBUTN2,
	KEY_JOYBUTN3,
	KEY_JOYBUTN4,
	KEY_JOYBUTN5,
	KEY_JOYBUTN6,
	KEY_JOYBUTN7,
	KEY_JOYBUTN8,
	KEY_JOYBUTN9,
	KEY_JOYBUTN10,
	KEY_JOYBUTN11,
	KEY_JOYBUTN12,

	KEY_POVLEFT,
	KEY_POVRIGHT,
	KEY_POVUP,
	KEY_POVDOWN,

	// last keytype defined
	KEY_NUMKEYS
};

#define NUMDIKEYS 256

//////////////////////////////////////////////////////////////////////////////////
// CDXInput is a class wrapper for DirectInput and contains functions to receive 
// data from the mouse, keyboard and joystick.
//////////////////////////////////////////////////////////////////////////////////

#include "../io3DEngine/ioSingleton.h"

#define FC __fastcall

#define JOY_PAD_ELECOM_BLACK_PRODUCT_NAME "Game VIB Joystick"
#define JOY_PAD_ELECOM_WHITE_PRODUCT_NAME "JC-U1012F "
#define JOY_PAD_XBOX360_WIRELESS_PRODUCT_NAME "Controller (Xbox 360 Wireless Receiver for Windows)"
#define JOY_PAD_LOGITECH_WIRELESS_PRODUCT_NAME "Logicool Cordless RumblePad 2"

class CDirectInput : public Singleton< CDirectInput >
{
private:
	enum JoyPadType 
	{
		JPT_NORMAL            = 0,
		JPT_ELECOM_BLACK      = 1,
		JPT_ELECOM_WHITE      = 2,
		JPT_XBOX360           = 3,
		JPT_XBOX360_WIRELESS  = 4,
		JPT_LOGITECH_WIRELESS = 5,
	};
private:
	static LPDIRECTINPUT8        m_lpDI;
	static LPDIRECTINPUTDEVICE8  m_lpDIDJoyStick;

private:
	BYTE  m_keyStates[KEY_NUMKEYS];
	BYTE  m_keyRaw[NUMDIKEYS];
	BYTE  m_oldkeyRaw[NUMDIKEYS];
	DWORD m_keyDragStartPositions[KEY_NUMKEYS][2];
	BYTE  m_LastKey;

	BYTE  m_shiftedKeyStates[KEY_NUMKEYS];
	DWORD m_DIKToDXKEY[NUMDIKEYS];

	BOOL m_bMouse;
	BOOL m_bKeyboard;
	BOOL m_bJoyStick;

	JoyPadType m_eJoyPadType;

	float m_mouseSensitivity;
	LONG m_mouseMinX;
	LONG m_mouseMinY;
	LONG m_mouseMaxX;
	LONG m_mouseMaxY;

	LONG m_mouseX, m_mouseY;
	LONG m_mouseFreeX, m_mouseFreeY;
	LONG m_mouseDeltaX, m_mouseDeltaY;

	LPDIRECTINPUTDEVICE8  m_lpDIDKeyboard;
	LPDIRECTINPUTDEVICE8  m_lpDIDMouse;

public:
	BOOL Create(HINSTANCE hInst, HWND hWnd , BOOL ExclusiveMouseAccess=TRUE );

	void FC Update(void);
	void FC UpdateLastKey();
	void FC FlushKeyboardData();
	void FC FlushJoyStickData();
	void FC RunMouseControlPanel(void *hWnd = NULL);

public:
	void FC SetMouseLimits(LONG x1, LONG y1, LONG x2, LONG y2);
	void FC SetMouseSensitivity(float factor);
	void SetActiveDevices(BOOL bMouse, BOOL bKeyboard, BOOL bJoystick);
	void FC SetMouseAbs(void);
	void FC SetMousePos(LONG x, LONG y);
	void FC SetMouseFreePos(LONG x, LONG y);

public:
	WORD FC GetShiftedKeyState(WORD key);
	WORD FC GetKeyState(WORD key);
	WORD FC GetKeyRaw(WORD key);
	BYTE FC GetLastKey() { return m_LastKey; };
	DWORD FC GetLastKeyChar();
	void FC GetKeyDesc( DWORD Checkkey, char *str );
	void FC GetMousePos(LONG* x, LONG* y);
	void FC GetMouseFreePos(LONG* x, LONG* y);
	void FC GetMouseDeltas(LONG* x, LONG* y);
	float FC GetMouseSensitivity();

public:
	bool FC IsKeyPress(WORD key);
	bool FC IsKeyDown(WORD key);
	bool FC IsKeyUp(WORD key);
	bool FC IsKeyRepeat(WORD key);

    BOOL FC HasMouse( void )    { return m_bMouse; }
    int  FC GetNumberOfJoystickButtons( void );
    BOOL FC IsGamePad( void );

	bool FC IsLBtnPress();
	bool FC IsLBtnDown();
	bool FC IsLBtnUp();
	bool FC IsLBtnRepeat();

	bool FC IsRBtnPress();
	bool FC IsRBtnDown();
	bool FC IsRBtnUp();
	bool FC IsRBtnRepeat();

	bool FC IsEnableJoyStick();

private:
	void FC InitDIKToDXKEY();
	
	void FC AcquireMouse();
	void FC UnacquireMouse();
	
	void FC AcquireKeyboard();
	void FC UnacquireKeyboard();
	
	void FC AcquireJoyStick();
	void FC UnacquireJoyStick();

	void FC CheckKeyEvents(DWORD key, BOOL isPressed);
	void FC InitShiftedKeys();

	void FC CheckPovKey( DIJOYSTATE& JoyState );
	void FC CheckNormalLeftStickKey( DIJOYSTATE& JoyState );
	void FC Check360LeftStickKey( DIJOYSTATE& JoyState );
	void FC CheckNormalRightStickKey( DIJOYSTATE &JoyState );
	void FC CheckElecomBlackRightStickKey( DIJOYSTATE& JoyState );
	void FC CheckElecomWhiteRightStickKey( DIJOYSTATE& JoyState );
	void FC Check360WirelessRightStickKey( DIJOYSTATE& JoyState );
	void FC Check360wirelessTriggerKey( DIJOYSTATE& JoyState );
	void FC CheckLogitechWirelessRightStickKey( DIJOYSTATE& JoyState );

public:
	static BOOL CALLBACK DIEnumJoyStickCallBack( LPCDIDEVICEINSTANCE pddi, LPVOID pvRef );

public:
	static CDirectInput& GetSingleton();

public:
	CDirectInput();
	virtual ~CDirectInput();
};

#define g_Input CDirectInput::GetSingleton()

#endif
