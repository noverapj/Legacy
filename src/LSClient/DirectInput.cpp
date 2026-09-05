
#include "stdafx.h"
#include <XInput.h>
#include "DirectInput.h"

template<> CDirectInput* Singleton< CDirectInput >::ms_Singleton = 0;

LPDIRECTINPUT8 CDirectInput::m_lpDI = NULL;
LPDIRECTINPUTDEVICE8 CDirectInput::m_lpDIDJoyStick = NULL;


//////////////////////////////////////////////////////////////////////////////////
// Default constructor. Simply initializes the member elements.
//////////////////////////////////////////////////////////////////////////////////
CDirectInput::CDirectInput()
{
	m_lpDIDKeyboard			= NULL;
	m_lpDIDMouse			= NULL;

	m_bMouse				= FALSE;
	m_bKeyboard				= FALSE;
	m_bJoyStick				= FALSE;

	m_mouseSensitivity		= 1.0;
	m_mouseMinX				= 0x80000000;
	m_mouseMinY				= 0x80000000; 
	m_mouseMaxX				= 0x7fffffff;
	m_mouseMaxY				= 0x7fffffff; 

	m_mouseX				= 0;
	m_mouseY				= 0;
	m_mouseFreeX			= 0;
	m_mouseFreeY			= 0;
	m_mouseDeltaX			= 0;
	m_mouseDeltaY			= 0;

	m_eJoyPadType           = JPT_NORMAL;

	// wipe our internal key data
	FlushKeyboardData();
}

//////////////////////////////////////////////////////////////////////////////////
// Default destructor. Releases all devices that accquired
//////////////////////////////////////////////////////////////////////////////////
CDirectInput::~CDirectInput()
{
	// make sure we're initialized
	if (!m_lpDI)	return;
	
	// shutdown keyboard
	if (m_lpDIDKeyboard)
	{
		UnacquireKeyboard();
		m_lpDIDKeyboard->Release();
		m_lpDIDKeyboard = NULL;
	}
	
	// shutdown mouse
	if (m_lpDIDMouse)
	{
		UnacquireMouse();
		m_lpDIDMouse->Release();
		m_lpDIDMouse = NULL;
	}

	// shutdown joystick
	if( m_lpDIDJoyStick )
	{
		UnacquireJoyStick();
		m_lpDIDJoyStick->Release();
		m_lpDIDJoyStick = NULL;
	}
	
	// kill directinput
	m_lpDI->Release();
	m_lpDI = NULL;
}

//////////////////////////////////////////////////////////////////////////////////
// Try to accquire all devices. Use SetActiveDevices() if you do not want 
// some devices.
//////////////////////////////////////////////////////////////////////////////////
BOOL CDirectInput::Create(HINSTANCE hInst, HWND hWnd , BOOL ExclusiveMouseAccess)
{
	HRESULT rval;

	rval = DirectInput8Create((HINSTANCE)hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&m_lpDI, NULL);
	if(rval != DI_OK)
		return FALSE;

	// Create the mouse device
	rval = m_lpDI->CreateDevice(GUID_SysMouse, &m_lpDIDMouse, NULL);
	if(rval == DI_OK)
	{
		m_lpDIDMouse->SetDataFormat(&c_dfDIMouse);
		if( ExclusiveMouseAccess == FALSE )
			m_lpDIDMouse->SetCooperativeLevel((HWND)hWnd, DISCL_FOREGROUND);
		else
			m_lpDIDMouse->SetCooperativeLevel((HWND)hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);

		AcquireMouse();

		if(!m_bMouse)
			return FALSE;
	}

	// Create the keyboard device
	rval = m_lpDI->CreateDevice(GUID_SysKeyboard, &m_lpDIDKeyboard, NULL);
	if(rval == DI_OK)
	{
		m_lpDIDKeyboard->SetDataFormat(&c_dfDIKeyboard);
		m_lpDIDKeyboard->SetCooperativeLevel((HWND)hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND  | DISCL_NOWINKEY);

		AcquireKeyboard();
		if(!m_bKeyboard) 
			return FALSE;

		// set up DIK_ remap
		InitDIKToDXKEY();
		InitShiftedKeys();
	}


	// Create the joystick device
	//
	rval = m_lpDI->EnumDevices( DI8DEVCLASS_GAMECTRL,
								DIEnumJoyStickCallBack,
								NULL,
								DIEDFL_ATTACHEDONLY );

	if(rval == DI_OK)
	{
		if( m_lpDIDJoyStick )
		{
			m_lpDIDJoyStick->SetDataFormat(&c_dfDIJoystick);
			m_lpDIDJoyStick->SetCooperativeLevel((HWND)hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);


            DIPROPRANGE range;
            range.diph.dwSize = sizeof range;
            range.diph.dwHeaderSize = sizeof range.diph;
            range.diph.dwObj = DIJOFS_X;
            range.diph.dwHow = DIPH_BYOFFSET;
            range.lMin = -100;
            range.lMax = 100;


			DIPROPDWORD saturation, dead;
			saturation.diph.dwSize = sizeof saturation;
            saturation.diph.dwHeaderSize = sizeof saturation.diph;
            saturation.diph.dwObj = DIJOFS_X;
            saturation.diph.dwHow = DIPH_BYOFFSET;
            saturation.dwData = 9800;

			dead = saturation;
			dead.dwData = 400;

			// x축 보정
			m_lpDIDJoyStick->SetProperty( DIPROP_RANGE, &range.diph);
			m_lpDIDJoyStick->SetProperty( DIPROP_DEADZONE, &dead.diph );
			m_lpDIDJoyStick->SetProperty( DIPROP_SATURATION, &saturation.diph );

			// y축 보정
			range.diph.dwObj = DIJOFS_Y;
			dead.diph.dwObj = DIJOFS_Y;
			saturation.diph.dwObj = DIJOFS_Y;
			
			m_lpDIDJoyStick->SetProperty( DIPROP_RANGE, &range.diph);
			m_lpDIDJoyStick->SetProperty( DIPROP_DEADZONE, &dead.diph );
			m_lpDIDJoyStick->SetProperty( DIPROP_SATURATION, &saturation.diph );

			AcquireJoyStick();

			DIDEVICEINSTANCE _DIDEVInst;
			ZeroMemory( &_DIDEVInst, sizeof(_DIDEVInst) );
			_DIDEVInst.dwSize = sizeof(_DIDEVInst);

			HRESULT hrr = m_lpDIDJoyStick->GetDeviceInfo( &_DIDEVInst );
			if( FAILED( hrr ) )
			{
				#ifndef SHIPPING 
				LOG.PrintTimeAndLog( 0, "Test - Info Fail" );
				#endif
			}
			else
			{
				XINPUT_STATE xInputstate;
				ZeroMemory( &xInputstate, sizeof(XINPUT_STATE) );

				// Simply get the state of the controller from XInput.
				DWORD dwResult = XInputGetState( 0, &xInputstate );

				if( strcmp( _DIDEVInst.tszProductName, JOY_PAD_ELECOM_BLACK_PRODUCT_NAME ) == 0 )
					m_eJoyPadType = JPT_ELECOM_BLACK;
				else if( strcmp( _DIDEVInst.tszProductName, JOY_PAD_ELECOM_WHITE_PRODUCT_NAME ) == 0 )
					m_eJoyPadType = JPT_ELECOM_WHITE;
				else if( strcmp( _DIDEVInst.tszProductName, JOY_PAD_XBOX360_WIRELESS_PRODUCT_NAME ) == 0 )
					m_eJoyPadType = JPT_XBOX360_WIRELESS;
				else if( strcmp( _DIDEVInst.tszProductName, JOY_PAD_LOGITECH_WIRELESS_PRODUCT_NAME ) == 0 )
					m_eJoyPadType = JPT_LOGITECH_WIRELESS;
				else if( dwResult == ERROR_SUCCESS )
					m_eJoyPadType = JPT_XBOX360;

				#ifndef SHIPPING 
				LOG.PrintTimeAndLog( 0, "Test - Info : %d, %s, JoyPadType: %d", _DIDEVInst.guidProduct.Data1, _DIDEVInst.tszProductName, m_eJoyPadType );
				#endif
			}
		}
	}
	else
	{
		LOG.PrintLog(0,"Couldn't create DirectInput CreateDevice");
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Controls which devices you have accquired.
/////////////////////////////////////////////////////////////////////////////////////////////
void CDirectInput::SetActiveDevices(BOOL bMouse, BOOL bKeyboard, BOOL bJoystick)
{
	m_bMouse = bMouse;

	if ( bMouse )
		AcquireMouse();
	else
		UnacquireMouse();

	m_bKeyboard = bKeyboard;

	if (m_bKeyboard = bKeyboard)
		AcquireKeyboard();
	else 
		UnacquireKeyboard();

	// 특별함
	if( m_lpDIDJoyStick )
	{
		m_bJoyStick = bJoystick;
		
		if( bJoystick )
			AcquireJoyStick();
		else
			UnacquireJoyStick();
	}
	else
	{
		m_bJoyStick = FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Sets the axis mode of the mouse.
/////////////////////////////////////////////////////////////////////////////////////////////
void FC CDirectInput::SetMouseAbs(void)
{
	if( !m_lpDIDMouse )	return;

	DIPROPDWORD dipdw;

	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = DIPROPAXISMODE_ABS;
	m_lpDIDMouse->SetProperty(DIPROP_AXISMODE, &dipdw.diph);
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Runs the DirectInput control panel associated with the mouse. If the device does not 
// have a control panel associated with it, the default device control panel is launched.
/////////////////////////////////////////////////////////////////////////////////////////////
void FC CDirectInput::RunMouseControlPanel(void *hWnd)
{
	if( m_lpDIDMouse )
	{
		m_lpDIDMouse->RunControlPanel((HWND)hWnd, 0);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Wipes out the internal key data.
/////////////////////////////////////////////////////////////////////////////////////////////
void FC CDirectInput::FlushKeyboardData()
{
	memset(m_keyStates, 0, sizeof(BYTE) * KEY_NUMKEYS);
	memset(m_keyRaw, 0, sizeof(BYTE) * KEY_NUMKEYS);
	memset(m_keyDragStartPositions, 0, sizeof(DWORD) * KEY_NUMKEYS * 2);
}

void FC CDirectInput::FlushJoyStickData()
{
	// 방향키
	CheckKeyEvents( KEY_LSTICKLEFT, FALSE );
	CheckKeyEvents( KEY_LSTICKRIGHT, FALSE );
	CheckKeyEvents( KEY_LSTICKUP, FALSE );
	CheckKeyEvents( KEY_LSTICKDOWN, FALSE );

	// 오른쪽 스틱
	CheckKeyEvents( KEY_RSTICKLEFT, FALSE );
	CheckKeyEvents( KEY_RSTICKRIGHT, FALSE );
	CheckKeyEvents( KEY_RSTICKUP, FALSE );
	CheckKeyEvents( KEY_RSTICKDOWN, FALSE );

	// Pov
	CheckKeyEvents( KEY_POVLEFT, FALSE );
	CheckKeyEvents( KEY_POVRIGHT, FALSE );
	CheckKeyEvents( KEY_POVUP, FALSE );
	CheckKeyEvents( KEY_POVDOWN, FALSE );

	// 버튼
	for( int i=0; i < 12; ++i )
	{
		CheckKeyEvents( KEY_JOYBUTN1+i, FALSE );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// This restricts the mouse to a defined area.
/////////////////////////////////////////////////////////////////////////////////////////////
void FC CDirectInput::SetMouseLimits(LONG x1, LONG y1, LONG x2, LONG y2)
{
	m_mouseMinX = x1;
	m_mouseMinY = y1;
	m_mouseMaxX = x2;
	m_mouseMaxY = y2;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Sets the mouse position. This restricts the position to the physical display.
/////////////////////////////////////////////////////////////////////////////////////////////
void FC CDirectInput::SetMousePos(LONG x, LONG y)
{
	// clamp non-free mouse values to limits
	if ((m_mouseX = x) >= m_mouseMaxX)
		m_mouseX = m_mouseMaxX-1;

	if ((m_mouseY = y) >= m_mouseMaxY)
		m_mouseY = m_mouseMaxY-1;

	if ((m_mouseX = x) <= m_mouseMinX)
		m_mouseX = m_mouseMinX+1;

	if ((m_mouseY = y) <= m_mouseMinY)
		m_mouseY = m_mouseMinY+1;

	// Zero out the Deltas
	m_mouseDeltaX = 0;
	m_mouseDeltaY = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Sets the mouse position. This allows you to set the mouse to anywhere (no limits).
/////////////////////////////////////////////////////////////////////////////////////////////
void FC CDirectInput::SetMouseFreePos(LONG x, LONG y)
{
	m_mouseFreeX  = x;
	m_mouseFreeY  = y;

	// Zero out the Deltas
	m_mouseDeltaX = 0;
	m_mouseDeltaY = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Sets the mouse sensitivity factor.
/////////////////////////////////////////////////////////////////////////////////////////////
void FC CDirectInput::SetMouseSensitivity(float factor)
{
	m_mouseSensitivity = factor;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Retrieves the current mouse sensitivity factor.
/////////////////////////////////////////////////////////////////////////////////////////////
float FC CDirectInput::GetMouseSensitivity()
{
	return	m_mouseSensitivity;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// This tells you whether or not the given key is in a shifted state.
/////////////////////////////////////////////////////////////////////////////////////////////
WORD FC CDirectInput::GetShiftedKeyState(WORD key)
{
	if( COMPARE( key, 0, NUMDIKEYS ) )
	{
		if ( m_shiftedKeyStates[key] )
			return m_shiftedKeyStates[key];
	}

	return key;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Retrieves the keystate of a given key.
/////////////////////////////////////////////////////////////////////////////////////////////
WORD FC CDirectInput::GetKeyState(WORD key)
{
	if( COMPARE( key, 0, KEY_NUMKEYS ) )
		return	m_keyStates[key];

	return KEY_NONE;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Gets raw information about a key.
/////////////////////////////////////////////////////////////////////////////////////////////
WORD CDirectInput::GetKeyRaw(WORD key)
{
	if( COMPARE( key, 0, KEY_NUMKEYS ) )
		return	m_keyRaw[key];

	return KEY_NONE;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Retrieves the mouse position and stores the x and y values in the passed parameters.
/////////////////////////////////////////////////////////////////////////////////////////////
void FC CDirectInput::GetMousePos(LONG* x, LONG* y)
{
	*x = m_mouseX;
	*y = m_mouseY;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Retrieves the free mouse position and stores the x and y values in the passed parameters.
/////////////////////////////////////////////////////////////////////////////////////////////
void FC CDirectInput::GetMouseFreePos(LONG* x, LONG* y)
{
	*x = m_mouseFreeX;
	*y = m_mouseFreeY;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Retrieves the mouse deltas and stores the x and y values in the passed parameters.
/////////////////////////////////////////////////////////////////////////////////////////////
void FC CDirectInput::GetMouseDeltas(LONG* x, LONG* y)
{
	*x = m_mouseDeltaX;
	*y = m_mouseDeltaY;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Obtains access to the mouse.
/////////////////////////////////////////////////////////////////////////////////////////////
void FC CDirectInput::AcquireMouse()
{
	if( !m_lpDIDMouse )	return;

	m_bMouse = TRUE;

	HRESULT hr = m_lpDIDMouse->Acquire();
	if( FAILED(hr) )	// didn't get it back, try a few more times
	{
		for( int i=0 ; i<50 ; i++ )
		{
			hr = m_lpDIDMouse->Acquire();
			if( hr == DI_OK )
				return;
		}

		// still don't have it, something's up
		m_bMouse = FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Releases access to the mouse.
/////////////////////////////////////////////////////////////////////////////////////////////
void FC CDirectInput::UnacquireMouse()
{
	m_bMouse = FALSE;
	if( m_lpDIDMouse )
	{
		m_lpDIDMouse->Unacquire();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Obtains access to the keyboard.
/////////////////////////////////////////////////////////////////////////////////////////////
void FC CDirectInput::AcquireKeyboard()
{
	if( !m_lpDIDKeyboard )	return;

	m_bKeyboard = TRUE;

	HRESULT hr = m_lpDIDKeyboard->Acquire();
	if( hr != DI_OK )	// didn't get it back, try a few more times
	{
		for( int i=0 ; i<50 ; i++ )
		{
			hr = m_lpDIDKeyboard->Acquire();
			if( SUCCEEDED(hr) )
				break;
		}
	}

	FlushKeyboardData();
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Releases access to the keyboard.
/////////////////////////////////////////////////////////////////////////////////////////////
void FC CDirectInput::UnacquireKeyboard()
{
	FlushKeyboardData();

	m_bKeyboard = FALSE;

	if( m_lpDIDKeyboard )
	{
		m_lpDIDKeyboard->Unacquire();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Obtains access to the joystick
/////////////////////////////////////////////////////////////////////////////////////////////
void FC CDirectInput::AcquireJoyStick()
{
	if( !m_lpDIDJoyStick )	return;

	m_bJoyStick = TRUE;

	HRESULT hr = m_lpDIDJoyStick->Acquire();
	if( FAILED(hr) )	// didn't get it back, try a few more times
	{
		for( int i=0 ; i<50 ; i++ )
		{
			hr = m_lpDIDJoyStick->Acquire();
			if( hr == DI_OK )
				return;
		}
	}

	FlushJoyStickData();
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Releases access to the joystick
/////////////////////////////////////////////////////////////////////////////////////////////
void FC CDirectInput::UnacquireJoyStick()
{
	FlushKeyboardData();

	m_bJoyStick = FALSE;
	if( m_lpDIDJoyStick )
	{
		m_lpDIDJoyStick->Unacquire();
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Updates all devices. Call this before you check for input.
/////////////////////////////////////////////////////////////////////////////////////////////
void FC CDirectInput::Update(void)
{
	///////////////////////
	//  KEYBOARD
	///////////////////////

	// for keyboard hook
#ifdef SHIPPING
	if( GetAsyncKeyState(VK_LMENU) < 0 || GetAsyncKeyState (VK_CONTROL) < 0 )
	{
		UnacquireKeyboard();
		return;
	}
#endif

	//printscreen 키 입력 보안 (렉권 방지)
	if ( !g_App.IsMeDeveloper() && GetAsyncKeyState(VK_SNAPSHOT) == 0xffff8000 )
	{
		UnacquireKeyboard();

		if ( g_RequestLagMgr.GetSingletonPtr() )
			g_RequestLagMgr.IncreaseValueInput();
		return;
	}

	if( m_lpDIDKeyboard )
	{
		HRESULT hr = m_lpDIDKeyboard->GetDeviceState( NUMDIKEYS, &m_keyRaw );
		if( FAILED(hr) )
		{
			AcquireKeyboard();
		}
		else
		{
			DWORD dwKeyValue;
			for( int i=0; i<NUMDIKEYS; i++)
			{
				dwKeyValue = m_DIKToDXKEY[i];

				if (dwKeyValue )
				{
					// check current key state (bit 8 flipped on means key is pressed)
					if ( m_keyRaw[i] & (BYTE)0x80 )
					{
						CheckKeyEvents( dwKeyValue, TRUE );
					}
					else
					{
						CheckKeyEvents( dwKeyValue, FALSE );
					}
				}
			}
		}
	}

	///////////////////////
	//  JoyStick
	///////////////////////
	if( m_lpDIDJoyStick && m_bJoyStick )
	{
		DIJOYSTATE JoyState;

		m_lpDIDJoyStick->Poll();
		HRESULT hr = m_lpDIDJoyStick->GetDeviceState(sizeof(JoyState), &JoyState);
		if( FAILED(hr) )
		{
			AcquireJoyStick();
		}
		else
		{
/*
#ifndef SHIPPING 
			static DIJOYSTATE g_JoyState;
			bool bChange = false;

			if( g_JoyState.lX != JoyState.lX ||
				g_JoyState.lY != JoyState.lY ||
				g_JoyState.lZ != JoyState.lZ ||
				g_JoyState.lRx != JoyState.lRx ||
				g_JoyState.lRy != JoyState.lRy ||
				g_JoyState.lRz != JoyState.lRz )
				bChange = true;

			for (int i = 0; i < 2 ; i++)
			{
				if( g_JoyState.rglSlider[i] != JoyState.rglSlider[i] )
					bChange = true;
			}

			for (int i = 0; i < 4 ; i++)
			{
				if( g_JoyState.rgdwPOV[i] != JoyState.rgdwPOV[i] )
					bChange = true;
			}

			for (int i = 0; i < 32 ; i++)
			{
				if( g_JoyState.rgbButtons[i] != JoyState.rgbButtons[i] )
					bChange = true;
			}


			if( bChange )
			{
				g_JoyState = JoyState;
				static int iCnt = 0;
				iCnt++;
				LOG.PrintTimeAndLog( 0, "Start[%d]---------------------------------------------------------", iCnt );
				LOG.PrintTimeAndLog( 0, "Test - lX: %d", JoyState.lX );
				LOG.PrintTimeAndLog( 0, "Test - lY: %d", JoyState.lY );
				LOG.PrintTimeAndLog( 0, "Test - lZ: %d", JoyState.lZ );

				LOG.PrintTimeAndLog( 0, "Test - lRx: %d", JoyState.lRx );
				LOG.PrintTimeAndLog( 0, "Test - lRy: %d", JoyState.lRy );
				LOG.PrintTimeAndLog( 0, "Test - lRz: %d", JoyState.lRz );

				for (int i = 0; i < 2 ; i++)
				{
					LOG.PrintTimeAndLog( 0, "Test - rglSlider%d: %d", i, JoyState.rglSlider[i] );	
				}

				for (int i = 0; i < 4 ; i++)
				{
					LOG.PrintTimeAndLog( 0, "Test - Pov%d: %d", i, JoyState.rgdwPOV[i] );
				}

				for (int i = 0; i < 32 ; i++)
				{
					LOG.PrintTimeAndLog( 0, "Test - rgbButtons%d: %d", i, JoyState.rgbButtons[i] );
				}
				LOG.PrintTimeAndLog( 0, "End[%d]---------------------------------------------------------", iCnt );
			}
#endif
*/

			// 방향키
			CheckPovKey( JoyState );

			// Left Stick
			if( m_eJoyPadType == JPT_ELECOM_BLACK     || 
				m_eJoyPadType == JPT_XBOX360          || 
				m_eJoyPadType == JPT_XBOX360_WIRELESS || 
				m_eJoyPadType == JPT_LOGITECH_WIRELESS )
				Check360LeftStickKey( JoyState );
			else
				CheckNormalLeftStickKey( JoyState );

			// Right Stick
			if( m_eJoyPadType == JPT_ELECOM_BLACK )
				CheckElecomBlackRightStickKey( JoyState ); 
			else if( m_eJoyPadType == JPT_ELECOM_WHITE )
				CheckElecomWhiteRightStickKey( JoyState );
			else if( m_eJoyPadType == JPT_XBOX360_WIRELESS )
				Check360WirelessRightStickKey( JoyState );
			else if( m_eJoyPadType == JPT_LOGITECH_WIRELESS )
				CheckLogitechWirelessRightStickKey( JoyState );
			else
				CheckNormalRightStickKey( JoyState );


			// 버튼
			int iMaxBtn = 12;
			if( m_eJoyPadType == JPT_XBOX360_WIRELESS )
			{
				Check360wirelessTriggerKey( JoyState );
				iMaxBtn = 10; // 내부에서 KEY_JOYBUTN11~12번을 TRUE/FALSE 한다.
			}
						
			for( int i=0; i < iMaxBtn; ++i )
			{
				if( JoyState.rgbButtons[i] & (BYTE)0x80 )
					CheckKeyEvents( KEY_JOYBUTN1+i, TRUE );
				else
					CheckKeyEvents( KEY_JOYBUTN1+i, FALSE );
			}

			
		}
	}

	///////////////////////
	//  MOUSE
	///////////////////////
	if(m_bMouse)
	{
		DIMOUSESTATE MouseState;

		HRESULT hr = m_lpDIDMouse->GetDeviceState(sizeof(MouseState), &MouseState);
		if( FAILED(hr) )
		{
			AcquireMouse();
		}
		else
		{
			// get new mouse position status
			LONG oldx = m_mouseFreeX;
			LONG oldy = m_mouseFreeY;

			m_mouseFreeX += (LONG)((float)MouseState.lX * m_mouseSensitivity);
			m_mouseFreeY += (LONG)((float)MouseState.lY * m_mouseSensitivity);
			m_mouseX     += (LONG)((float)MouseState.lX * m_mouseSensitivity);
			m_mouseY     += (LONG)((float)MouseState.lY * m_mouseSensitivity);

			// clamp non-free mouse values to limits
			if (m_mouseX >= m_mouseMaxX)
				m_mouseX = m_mouseMaxX-1;

			if (m_mouseY >= m_mouseMaxY)
				m_mouseY = m_mouseMaxY-1;

			if (m_mouseX <= m_mouseMinX)
				m_mouseX = m_mouseMinX;

			if (m_mouseY <= m_mouseMinY)
				m_mouseY = m_mouseMinY;

			m_mouseDeltaX = m_mouseFreeX - oldx;
			m_mouseDeltaY = m_mouseFreeY - oldy;
	
			for( int i=0; i<3; i++)
			{
				if (MouseState.rgbButtons[i] & (BYTE)0x80)
					CheckKeyEvents( KEY_MOUSELEFT + i, TRUE );
				else
					CheckKeyEvents( KEY_MOUSELEFT + i, FALSE );
			}
		}
	}
}


void FC CDirectInput::CheckPovKey( DIJOYSTATE& JoyState )
{
	// Pov
	DWORD dwCurPov = JoyState.rgdwPOV[0];

	// Left, Right
	if( dwCurPov == 27000 || dwCurPov == 31500 || dwCurPov == 22500 )
	{
		CheckKeyEvents( KEY_POVLEFT, TRUE );
		CheckKeyEvents( KEY_POVRIGHT, FALSE );
	}
	else if( dwCurPov == 9000 || dwCurPov == 4500 || dwCurPov == 13500 )
	{
		CheckKeyEvents( KEY_POVLEFT, FALSE );
		CheckKeyEvents( KEY_POVRIGHT, TRUE );
	}
	else
	{
		CheckKeyEvents( KEY_POVLEFT, FALSE );
		CheckKeyEvents( KEY_POVRIGHT, FALSE );
	}

	// Up, Down
	if( dwCurPov == 0 || dwCurPov == 31500 || dwCurPov == 4500 )
	{
		CheckKeyEvents( KEY_POVUP, TRUE );
		CheckKeyEvents( KEY_POVDOWN, FALSE );
	}
	else if( dwCurPov == 18000 || dwCurPov == 13500 || dwCurPov == 22500 )
	{
		CheckKeyEvents( KEY_POVUP, FALSE );
		CheckKeyEvents( KEY_POVDOWN, TRUE );
	}
	else
	{
		CheckKeyEvents( KEY_POVUP, FALSE );
		CheckKeyEvents( KEY_POVDOWN, FALSE );
	}
}

void FC CDirectInput::CheckNormalLeftStickKey( DIJOYSTATE& JoyState )
{
	// 방향키
	if( JoyState.lX == -100 )
	{
		CheckKeyEvents( KEY_LSTICKLEFT, TRUE );
		CheckKeyEvents( KEY_LSTICKRIGHT, FALSE );
	}
	else if( JoyState.lX == 100 )
	{
		CheckKeyEvents( KEY_LSTICKLEFT, FALSE );
		CheckKeyEvents( KEY_LSTICKRIGHT, TRUE );
	}
	else
	{
		CheckKeyEvents( KEY_LSTICKLEFT, FALSE );
		CheckKeyEvents( KEY_LSTICKRIGHT, FALSE );
	}

	if( JoyState.lY == -100 )
	{
		CheckKeyEvents( KEY_LSTICKUP, TRUE );
		CheckKeyEvents( KEY_LSTICKDOWN, FALSE );
	}
	else if( JoyState.lY == 100 )
	{
		CheckKeyEvents( KEY_LSTICKUP, FALSE );
		CheckKeyEvents( KEY_LSTICKDOWN, TRUE );
	}
	else
	{
		CheckKeyEvents( KEY_LSTICKUP, FALSE );
		CheckKeyEvents( KEY_LSTICKDOWN, FALSE );
	}
}

void FC CDirectInput::Check360LeftStickKey( DIJOYSTATE& JoyState )
{
	// 방향키
	if( JoyState.lX >= -100 && JoyState.lX < -20 )
	{
		CheckKeyEvents( KEY_LSTICKLEFT, TRUE );
		CheckKeyEvents( KEY_LSTICKRIGHT, FALSE );
	}
	else if( JoyState.lX <= 100 && JoyState.lX > 20 )
	{
		CheckKeyEvents( KEY_LSTICKLEFT, FALSE );
		CheckKeyEvents( KEY_LSTICKRIGHT, TRUE );
	}
	else
	{
		CheckKeyEvents( KEY_LSTICKLEFT, FALSE );
		CheckKeyEvents( KEY_LSTICKRIGHT, FALSE );
	}

	if( JoyState.lY >= -100 && JoyState.lY < -20  )
	{
		CheckKeyEvents( KEY_LSTICKUP, TRUE );
		CheckKeyEvents( KEY_LSTICKDOWN, FALSE );
	}
	else if( JoyState.lY <= 100 && JoyState.lY > 20 )
	{
		CheckKeyEvents( KEY_LSTICKUP, FALSE );
		CheckKeyEvents( KEY_LSTICKDOWN, TRUE );
	}
	else
	{
		CheckKeyEvents( KEY_LSTICKUP, FALSE );
		CheckKeyEvents( KEY_LSTICKDOWN, FALSE );
	}
}


void FC CDirectInput::CheckNormalRightStickKey( DIJOYSTATE &JoyState )
{
	if( COMPARE( JoyState.lZ , 0, 19660 ) )  // LEFT
	{
		CheckKeyEvents( KEY_RSTICKLEFT, TRUE );
		CheckKeyEvents( KEY_RSTICKRIGHT, FALSE );
	}
	else if( COMPARE( JoyState.lZ, 45875, 65535+1 ) ) // RIGHT
	{
		CheckKeyEvents( KEY_RSTICKLEFT, FALSE );
		CheckKeyEvents( KEY_RSTICKRIGHT, TRUE );
	}
	else
	{
		CheckKeyEvents( KEY_RSTICKLEFT, FALSE );
		CheckKeyEvents( KEY_RSTICKRIGHT, FALSE );
	}

	if( COMPARE( JoyState.lRz, 0, 19660 )  ) // UP
	{
		CheckKeyEvents( KEY_RSTICKUP, TRUE );
		CheckKeyEvents( KEY_RSTICKDOWN, FALSE );
	}
	else if( COMPARE( JoyState.lRz, 45875, 65535+1 )  ) // DOWN
	{
		CheckKeyEvents( KEY_RSTICKUP, FALSE );
		CheckKeyEvents( KEY_RSTICKDOWN, TRUE );
	}
	else
	{
		CheckKeyEvents( KEY_RSTICKUP, FALSE );
		CheckKeyEvents( KEY_RSTICKDOWN, FALSE );
	}
}

void FC CDirectInput::CheckElecomBlackRightStickKey( DIJOYSTATE& JoyState )
{
	if( COMPARE( JoyState.lRz , 0, 19660 ) )  // LEFT
	{
		CheckKeyEvents( KEY_RSTICKLEFT, TRUE );
		CheckKeyEvents( KEY_RSTICKRIGHT, FALSE );
	}
	else if( COMPARE( JoyState.lRz, 45875, 65535+1 ) ) // RIGHT
	{
		CheckKeyEvents( KEY_RSTICKLEFT, FALSE );
		CheckKeyEvents( KEY_RSTICKRIGHT, TRUE );
	}
	else
	{
		CheckKeyEvents( KEY_RSTICKLEFT, FALSE );
		CheckKeyEvents( KEY_RSTICKRIGHT, FALSE );
	}
	
	if( COMPARE( JoyState.lZ, 0, 19660 )  ) // UP
	{
		CheckKeyEvents( KEY_RSTICKUP, TRUE );
		CheckKeyEvents( KEY_RSTICKDOWN, FALSE );
	}
	else if( COMPARE( JoyState.lZ, 45875, 65535+1 )  ) // DOWN
	{
		CheckKeyEvents( KEY_RSTICKUP, FALSE );
		CheckKeyEvents( KEY_RSTICKDOWN, TRUE );
	}
	else
	{
		CheckKeyEvents( KEY_RSTICKUP, FALSE );
		CheckKeyEvents( KEY_RSTICKDOWN, FALSE );
	}
}

void FC CDirectInput::CheckElecomWhiteRightStickKey( DIJOYSTATE& JoyState )
{
	if( COMPARE( JoyState.lZ, 255,  19660 ) )  // LEFT
	{
		CheckKeyEvents( KEY_RSTICKLEFT, TRUE );
		CheckKeyEvents( KEY_RSTICKRIGHT, FALSE );
	}
	else if( COMPARE( JoyState.lZ, 45875, 65535+1 ) ) // RIGHT
	{
		CheckKeyEvents( KEY_RSTICKLEFT, FALSE );
		CheckKeyEvents( KEY_RSTICKRIGHT, TRUE );
	}
	else
	{
		CheckKeyEvents( KEY_RSTICKLEFT, FALSE );
		CheckKeyEvents( KEY_RSTICKRIGHT, FALSE );
	}

	if( COMPARE( JoyState.lRz, 255, 19660 )  ) // UP
	{
		CheckKeyEvents( KEY_RSTICKUP, TRUE );
		CheckKeyEvents( KEY_RSTICKDOWN, FALSE );
	}
	else if( COMPARE( JoyState.lRz, 45875, 65535+1 )  ) // DOWN
	{
		CheckKeyEvents( KEY_RSTICKUP, FALSE );
		CheckKeyEvents( KEY_RSTICKDOWN, TRUE );
	}
	else
	{
		CheckKeyEvents( KEY_RSTICKUP, FALSE );
		CheckKeyEvents( KEY_RSTICKDOWN, FALSE );
	}
}

void FC CDirectInput::Check360WirelessRightStickKey( DIJOYSTATE& JoyState )
{
	if( COMPARE( JoyState.lRx, 0 , 19660 ) )  // LEFT
	{
		CheckKeyEvents( KEY_RSTICKLEFT, TRUE );
		CheckKeyEvents( KEY_RSTICKRIGHT, FALSE );
	}
	else if( COMPARE( JoyState.lRx, 45875, 65535+1 ) ) // RIGHT
	{
		CheckKeyEvents( KEY_RSTICKLEFT, FALSE );
		CheckKeyEvents( KEY_RSTICKRIGHT, TRUE );
	}
	else
	{
		CheckKeyEvents( KEY_RSTICKLEFT, FALSE );
		CheckKeyEvents( KEY_RSTICKRIGHT, FALSE );
	}

	if( COMPARE( JoyState.lRy, 0 , 19660 )  ) // UP
	{
		CheckKeyEvents( KEY_RSTICKUP, TRUE );
		CheckKeyEvents( KEY_RSTICKDOWN, FALSE );
	}
	else if( COMPARE( JoyState.lRy, 45875, 65535+1 ) ) // DOWN
	{
		CheckKeyEvents( KEY_RSTICKUP, FALSE );
		CheckKeyEvents( KEY_RSTICKDOWN, TRUE );
	}
	else
	{
		CheckKeyEvents( KEY_RSTICKUP, FALSE );
		CheckKeyEvents( KEY_RSTICKDOWN, FALSE );
	}
}

void FC CDirectInput::Check360wirelessTriggerKey( DIJOYSTATE& JoyState )
{
	if( COMPARE( JoyState.lZ, 45875, 65535+1 ) ) // L2( LT )
	{
		CheckKeyEvents( KEY_JOYBUTN11, TRUE );
		CheckKeyEvents( KEY_JOYBUTN12, FALSE );
	}
	else if( COMPARE( JoyState.lZ, 0, 19660 ) ) // R2( RT )
	{
		CheckKeyEvents( KEY_JOYBUTN11, FALSE );
		CheckKeyEvents( KEY_JOYBUTN12, TRUE );
	}
	else
	{
		CheckKeyEvents( KEY_JOYBUTN11, FALSE );
		CheckKeyEvents( KEY_JOYBUTN12, FALSE );
	}
}

void FC CDirectInput::CheckLogitechWirelessRightStickKey( DIJOYSTATE& JoyState )
{
	if( COMPARE( JoyState.lZ, 0, 19660 ) )  // LEFT
	{
		CheckKeyEvents( KEY_RSTICKLEFT, TRUE );
		CheckKeyEvents( KEY_RSTICKRIGHT, FALSE );
	}
	else if( COMPARE( JoyState.lZ, 45875, 65535+1 ) ) // RIGHT
	{
		CheckKeyEvents( KEY_RSTICKLEFT, FALSE );
		CheckKeyEvents( KEY_RSTICKRIGHT, TRUE );
	}
	else
	{
		CheckKeyEvents( KEY_RSTICKLEFT, FALSE );
		CheckKeyEvents( KEY_RSTICKRIGHT, FALSE );
	}

	if( COMPARE( JoyState.lRz, 0 , 19660 ) ) // UP
	{
		CheckKeyEvents( KEY_RSTICKUP, TRUE );
		CheckKeyEvents( KEY_RSTICKDOWN, FALSE );
	}
	else if( COMPARE( JoyState.lRz, 45875, 65535+1 ) ) // DOWN
	{
		CheckKeyEvents( KEY_RSTICKUP, FALSE );
		CheckKeyEvents( KEY_RSTICKDOWN, TRUE );
	}
	else
	{
		CheckKeyEvents( KEY_RSTICKUP, FALSE );
		CheckKeyEvents( KEY_RSTICKDOWN, FALSE );
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////
// Checks for keyboard events.
/////////////////////////////////////////////////////////////////////////////////////////////
void FC CDirectInput::CheckKeyEvents(DWORD key, BOOL isPressed)
{ 
	if (key >= KEY_NUMKEYS)
		return;

	if (isPressed)
	{
		if (m_keyStates[key] == KEY_NONE || m_keyStates[key] == KEY_RELEASE)
		{
			// press event (key was up before but down now)
			m_keyStates[key]				= KEY_PRESS;
			m_keyDragStartPositions[key][0] = m_mouseFreeX;
			m_keyDragStartPositions[key][1] = m_mouseFreeY;
		}
		else if ( m_keyStates[key] == KEY_PRESS )
		{
			// drag event (key is still down)
			m_keyStates[key] = KEY_REPEAT;
		}
	}
	else // Key is not currently pressed
	{
		if (m_keyStates[key] == KEY_REPEAT || m_keyStates[key] == KEY_PRESS)
		{
			// release event (key was down before but up now)
			m_keyStates[key] = KEY_RELEASE;
		}
		else if (m_keyStates[key] == KEY_RELEASE)
		{
			m_keyStates[key] = KEY_NONE;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Initializes the shifted key states.
/////////////////////////////////////////////////////////////////////////////////////////////
void FC CDirectInput::InitShiftedKeys()
{
	memset(m_shiftedKeyStates, 0, KEY_NUMKEYS * sizeof(BYTE));

	for ( int i=0; i<26; i++ )
	{
		m_shiftedKeyStates[i+'a'] = (unsigned char)(i+'A');
	}

	m_shiftedKeyStates['0']  = ')';
	m_shiftedKeyStates['1']  = '!';
	m_shiftedKeyStates['2']  = '@';
	m_shiftedKeyStates['3']  = '#';
	m_shiftedKeyStates['4']  = '$';
	m_shiftedKeyStates['5']  = '%';
	m_shiftedKeyStates['6']  = '^';
	m_shiftedKeyStates['7']  = '&';
	m_shiftedKeyStates['8']  = '*';
	m_shiftedKeyStates['9']  = '(';
	m_shiftedKeyStates['-']  = '_';
	m_shiftedKeyStates['=']  = '+';
	m_shiftedKeyStates['[']  = '{';
	m_shiftedKeyStates[']']  = '}';
	m_shiftedKeyStates['\\'] = '|';
	m_shiftedKeyStates[';']  = ':';
	m_shiftedKeyStates['\''] = '"';
	m_shiftedKeyStates[',']  = '<';
	m_shiftedKeyStates['.']  = '>';
	m_shiftedKeyStates['/']  = '?';
	m_shiftedKeyStates['`']  = '~';
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Since we can't guarantee that the constants for these DIKs won't
// change, initialize the remap table here instead of statically doing it.
/////////////////////////////////////////////////////////////////////////////////////////////
void FC CDirectInput::InitDIKToDXKEY()
{
	// any key mapped to zero means unmapped and should be ignored
	memset(m_DIKToDXKEY, 0, sizeof(m_DIKToDXKEY));

	m_DIKToDXKEY[DIK_ESCAPE]	= KEY_ESCAPE;
	m_DIKToDXKEY[DIK_1]			= '1';
	m_DIKToDXKEY[DIK_2]			= '2';
	m_DIKToDXKEY[DIK_3]			= '3';
	m_DIKToDXKEY[DIK_4]			= '4';
	m_DIKToDXKEY[DIK_5]			= '5';
	m_DIKToDXKEY[DIK_6]			= '6';
	m_DIKToDXKEY[DIK_7]			= '7';
	m_DIKToDXKEY[DIK_8]			= '8';
	m_DIKToDXKEY[DIK_9]			= '9';
	m_DIKToDXKEY[DIK_0]			= '0';
	m_DIKToDXKEY[DIK_MINUS]		= '-';
	m_DIKToDXKEY[DIK_EQUALS]	= '=';
	m_DIKToDXKEY[DIK_BACK]		= KEY_BACKSPACE;
	m_DIKToDXKEY[DIK_TAB]		= KEY_TAB;
	m_DIKToDXKEY[DIK_Q]			= 'q';
	m_DIKToDXKEY[DIK_W]			= 'w';
	m_DIKToDXKEY[DIK_E]			= 'e';
	m_DIKToDXKEY[DIK_R]			= 'r';
	m_DIKToDXKEY[DIK_T]			= 't';
	m_DIKToDXKEY[DIK_Y]			= 'y';
	m_DIKToDXKEY[DIK_U]			= 'u';
	m_DIKToDXKEY[DIK_I]			= 'i';
	m_DIKToDXKEY[DIK_O]			= 'o';
	m_DIKToDXKEY[DIK_P]			= 'p';
	m_DIKToDXKEY[DIK_LBRACKET]	= '[';
	m_DIKToDXKEY[DIK_RBRACKET]	= ']';
	m_DIKToDXKEY[DIK_RETURN]	= KEY_ENTER;
	m_DIKToDXKEY[DIK_LCONTROL]	= KEY_LEFTCTRL;
	m_DIKToDXKEY[DIK_A]			= 'a';
	m_DIKToDXKEY[DIK_S]			= 's';
	m_DIKToDXKEY[DIK_D]			= 'd';
	m_DIKToDXKEY[DIK_F]			= 'f';
	m_DIKToDXKEY[DIK_G]			= 'g';
	m_DIKToDXKEY[DIK_H]			= 'h';
	m_DIKToDXKEY[DIK_J]			= 'j';
	m_DIKToDXKEY[DIK_K]			= 'k';
	m_DIKToDXKEY[DIK_L]			= 'l';
	m_DIKToDXKEY[DIK_SEMICOLON]	= ';';
	m_DIKToDXKEY[DIK_APOSTROPHE]= '\'';
	m_DIKToDXKEY[DIK_GRAVE]		= '`';
	m_DIKToDXKEY[DIK_LSHIFT]	= KEY_LEFTSHIFT;
	m_DIKToDXKEY[DIK_BACKSLASH]	= '\\';
	m_DIKToDXKEY[DIK_Z]			= 'z';
	m_DIKToDXKEY[DIK_X]			= 'x';
	m_DIKToDXKEY[DIK_C]			= 'c';
	m_DIKToDXKEY[DIK_V]			= 'v';
	m_DIKToDXKEY[DIK_B]			= 'b';
	m_DIKToDXKEY[DIK_N]			= 'n';
	m_DIKToDXKEY[DIK_M]			= 'm';
	m_DIKToDXKEY[DIK_COMMA]		= ',';
	m_DIKToDXKEY[DIK_PERIOD]	= '.';
	m_DIKToDXKEY[DIK_SLASH]		= '/';
	m_DIKToDXKEY[DIK_RSHIFT]	= KEY_RIGHTSHIFT;
	m_DIKToDXKEY[DIK_MULTIPLY]	= KEY_NUMSTAR;
	m_DIKToDXKEY[DIK_LMENU]		= KEY_LEFTALT;
	m_DIKToDXKEY[DIK_SPACE]		= KEY_SPACE;
	m_DIKToDXKEY[DIK_CAPITAL]	= KEY_CAPSLOCK;
	m_DIKToDXKEY[DIK_F1]		= KEY_F1;
	m_DIKToDXKEY[DIK_F2]		= KEY_F2;
	m_DIKToDXKEY[DIK_F3]		= KEY_F3;
	m_DIKToDXKEY[DIK_F4]		= KEY_F4;
	m_DIKToDXKEY[DIK_F5]		= KEY_F5;
	m_DIKToDXKEY[DIK_F6]		= KEY_F6;
	m_DIKToDXKEY[DIK_F7]		= KEY_F7;
	m_DIKToDXKEY[DIK_F8]		= KEY_F8;
	m_DIKToDXKEY[DIK_F9]		= KEY_F9;
	m_DIKToDXKEY[DIK_F10]		= KEY_F10;
	m_DIKToDXKEY[DIK_NUMLOCK]	= KEY_NUMLOCK;
	m_DIKToDXKEY[DIK_SCROLL]	= KEY_SCROLLLOCK;
	m_DIKToDXKEY[DIK_NUMPAD7]	= KEY_NUM7;
	m_DIKToDXKEY[DIK_NUMPAD8]	= KEY_NUM8;
	m_DIKToDXKEY[DIK_NUMPAD9]	= KEY_NUM9;
	m_DIKToDXKEY[DIK_SUBTRACT]	= KEY_NUMMINUS;
	m_DIKToDXKEY[DIK_NUMPAD4]	= KEY_NUM4;
	m_DIKToDXKEY[DIK_NUMPAD5]	= KEY_NUM5;
	m_DIKToDXKEY[DIK_NUMPAD6]	= KEY_NUM6;
	m_DIKToDXKEY[DIK_ADD]		= KEY_NUMPLUS;
	m_DIKToDXKEY[DIK_NUMPAD1]	= KEY_NUM1;
	m_DIKToDXKEY[DIK_NUMPAD2]	= KEY_NUM2;
	m_DIKToDXKEY[DIK_NUMPAD3]	= KEY_NUM3;
	m_DIKToDXKEY[DIK_NUMPAD0]	= KEY_NUM0;
	m_DIKToDXKEY[DIK_DECIMAL]	= KEY_NUMPERIOD;
	m_DIKToDXKEY[DIK_F11]		= KEY_F11;
	m_DIKToDXKEY[DIK_F12]		= KEY_F12;
	m_DIKToDXKEY[DIK_NUMPADENTER]  = KEY_NUMENTER;
	m_DIKToDXKEY[DIK_RCONTROL]	= KEY_RIGHTCTRL;
	m_DIKToDXKEY[DIK_DIVIDE]	= KEY_NUMSLASH;
	m_DIKToDXKEY[DIK_SYSRQ]		= KEY_PRINTSCRN;
	m_DIKToDXKEY[DIK_RMENU]		= KEY_RIGHTALT;
	m_DIKToDXKEY[DIK_HOME]		= KEY_HOME;
	m_DIKToDXKEY[DIK_UP]		= KEY_UPARROW;
	m_DIKToDXKEY[DIK_PRIOR]		= KEY_PGUP;
	m_DIKToDXKEY[DIK_LEFT]		= KEY_LEFTARROW;
	m_DIKToDXKEY[DIK_RIGHT]		= KEY_RIGHTARROW;
	m_DIKToDXKEY[DIK_END]		= KEY_END;
	m_DIKToDXKEY[DIK_DOWN]		= KEY_DOWNARROW;
	m_DIKToDXKEY[DIK_NEXT]		= KEY_PGDN;
	m_DIKToDXKEY[DIK_INSERT]	= KEY_INS;
	m_DIKToDXKEY[DIK_DELETE]	= KEY_DEL;
	m_DIKToDXKEY[DIK_LWIN]		= KEY_LWIN;
	m_DIKToDXKEY[DIK_RWIN]		= KEY_RWIN;
	m_DIKToDXKEY[DIK_APPS]		= 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Updates the m_LastKey variable, and fills it with the LastKey pressed
/////////////////////////////////////////////////////////////////////////////////////////////
void FC CDirectInput::UpdateLastKey()
{
	for( int i=0; i<255; i++ )
	{
		if( m_keyRaw[i] && !m_oldkeyRaw[i] )
		{
			m_LastKey = (unsigned char)i;
			break;
		}
	}

    memcpy( m_oldkeyRaw, m_keyRaw, 255 );
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Fills the str with the Description of the CeckKey.
/////////////////////////////////////////////////////////////////////////////////////////////
void FC CDirectInput::GetKeyDesc( DWORD CheckKey, char *str )
{       
	DWORD key = (CheckKey<<16); //+(1<<30); Extended Keys
    GetKeyNameText( key, str, 50 );
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Will convert LastKey to Scancode like in dos 
/////////////////////////////////////////////////////////////////////////////////////////////
DWORD FC CDirectInput::GetLastKeyChar()
{   
    return MapVirtualKey( m_LastKey, 1 );
}

bool FC CDirectInput::IsKeyPress( WORD key )
{
	if( GetKeyState( key ) == KEY_PRESS )
		return true;

	return false;
}

bool FC CDirectInput::IsKeyDown(WORD key)
{
	WORD wKeyState = GetKeyState( key );

	if( wKeyState == KEY_REPEAT || wKeyState == KEY_PRESS )
		return true;

	return false;
}

bool FC CDirectInput::IsKeyUp(WORD key)
{
	if( GetKeyState(key) == KEY_RELEASE )
		return true;
	
	return false;
}

bool FC CDirectInput::IsKeyRepeat(WORD key)
{
	if( GetKeyState(key) == KEY_REPEAT )
		return true;

	return false;
}

bool FC CDirectInput::IsLBtnPress()
{
	return IsKeyPress( KEY_MOUSELEFT );
}

bool FC CDirectInput::IsLBtnDown()
{
	return IsKeyDown( KEY_MOUSELEFT );
}

bool FC CDirectInput::IsLBtnUp()
{
	return IsKeyUp( KEY_MOUSELEFT );
}

bool FC CDirectInput::IsLBtnRepeat()
{
	return IsKeyRepeat( KEY_MOUSELEFT );
}

bool FC CDirectInput::IsRBtnPress()
{
	return IsKeyPress( KEY_MOUSERIGHT );
}

bool FC CDirectInput::IsRBtnDown()
{
	return IsKeyDown( KEY_MOUSERIGHT );
}

bool FC CDirectInput::IsRBtnUp()
{
	return IsKeyUp( KEY_MOUSERIGHT );
}

bool FC CDirectInput::IsRBtnRepeat()
{
	return IsKeyRepeat( KEY_MOUSERIGHT );
}

bool FC CDirectInput::IsEnableJoyStick()
{
	if( m_lpDIDJoyStick && m_bJoyStick )
		return true;

	return false;
}

BOOL CALLBACK CDirectInput::DIEnumJoyStickCallBack( LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef )
{
	UNREFERENCED_PARAMETER( pvRef );

	HRESULT hr;
	hr = m_lpDI->CreateDevice( lpddi->guidInstance, &m_lpDIDJoyStick, NULL );

	if( FAILED( hr ) )
		return DIENUM_CONTINUE;

	return DIENUM_STOP;
}

CDirectInput& CDirectInput::GetSingleton()
{
	return Singleton< CDirectInput >::GetSingleton();
}

