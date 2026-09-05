#pragma once

class CBlockKeyBoard 
{
private:
	bool	m_bWin98Series;
	bool	m_bBlock;

public:
	void EnableSysKey();
	void DisableSysKey( HINSTANCE hInstance );

public:
	bool IsDisableSysKey() { return m_bBlock; }
	bool CheckAltTabKey();

private:
	static HHOOK	m_hHook;
	static bool		m_bAltTab;

public:
	static LRESULT CALLBACK LowLevelKeyboardProc (int nCode, WPARAM wParam, LPARAM lParam);

public:
	CBlockKeyBoard( bool bWin98Series );
	~CBlockKeyBoard();
};

