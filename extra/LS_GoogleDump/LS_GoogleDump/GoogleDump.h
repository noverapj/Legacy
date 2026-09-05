#pragma once

#define MAXPATH 512

class GoogleDump
{	
public:
	static TCHAR m_foldername[MAXPATH];
	static TCHAR m_filename[MAXPATH];

	GoogleDump();
	~GoogleDump();

	static void Begin(const TCHAR* foldername);
	static void End();
};