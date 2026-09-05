#pragma once

class cMiniDump
{
public:
    static BOOL Begin(const TCHAR* filename);
    static BOOL End(VOID);

public:
	static TCHAR m_filename[MAX_PATH];
	static TCHAR m_folername[MAX_PATH];
};
