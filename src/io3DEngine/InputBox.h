

#ifndef _InputBox_h_
#define _InputBox_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class ioIME;

class __EX ioInputBox : public Singleton< ioInputBox >
{
protected:
	ioIME	*m_pIME;

	int		m_iWheelMoveAmt;
	bool	m_bEnter;
	bool	m_bBackSpace;
	bool	m_bTab;
	bool    m_bLeft;
	bool    m_bRight;
	bool	m_bUp;
	bool	m_bDown;
	bool    m_bDel;
	bool	m_bKeyDown;


	bool    m_bUseClipboard;


public:
	BOOL InputBoxProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

public:
	void SetString( const char *szText );
	void SetStringNotDeleteComposing( const char *szText );
	void SetCaretPos( int iPos );

	void SetLimitText( int iLen );	
	void Clear();

	const char* GetString() const;
	const int GetCaretPos() const;

public:
	void SetNativeInput();
	void SetEnglishInput();
	void GetFocusToMe( HWND hWnd );
	void Process();
	bool HasFocus( HWND hFocus );

public:
	int GetWheelMove() const { return m_iWheelMoveAmt; }
	bool IsEnter();
	bool IsBackSpace();	
	bool IsTab();
	bool IsLeft();
	bool IsRight();
	bool IsUp();
	bool IsDown();
	bool IsDel();
	bool IsKeyDown();
	void ClearKeyState();

	void SetUseClipboard( bool bUse ){ m_bUseClipboard = bUse; }
	bool GetUseClipboard(){ return m_bUseClipboard; }

public:
	static ioInputBox& GetSingleton();

public:
	ioInputBox( HWND hWnd,  HINSTANCE hInstance );
	virtual ~ioInputBox();
};

#define g_InputBox ioInputBox::GetSingleton()

#endif