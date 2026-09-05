#ifndef __EditBox_h__
#define __EditBox_h__

#include "ioIME.h"

class __EX EditBox : public ioIME
{
protected:
	static WNDPROC m_EditProc;
	HWND   m_hEdit;
	HWND   m_hParentWnd;
	HIMC   m_hSavedIMC;

	DWORD m_dwSavedwConv;
	DWORD m_dwSavedwSent;

protected:
	void SetCompositionWindow( int iX, int iY );
	void SetCandidateWindow( int iX, int iY );

public:
	static LRESULT CALLBACK EditProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	virtual BOOL IMEWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual void Create( HWND hWnd,  HINSTANCE hInstance );

	virtual void SetString( const char *szText );
	virtual void SetStringNotDeleteComposing( const char *szText );

	virtual void SetMaxInputLen( int iLen );	
	virtual void ClearString();

	virtual const char* GetString();

	virtual void ChangeConversionStatus(DWORD dwConvMode, DWORD dwSentMode);

	virtual void Process();

	virtual bool IsNowComposing();

	virtual bool HasFocus( HWND hFocus );

public:
	EditBox(void);
	virtual ~EditBox(void);
};

#endif // __EditBox_h__