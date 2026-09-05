#pragma once

#import "C:\Program Files\Common Files\System\ADO\msado15.dll" \
no_namespace rename("EOF", "EndOfFile")

#include "ioTableMgr.h"

class ioAdoAutoInit
{
public:
	ioAdoAutoInit();
	~ioAdoAutoInit();
};

class ioExcelReader
{
private:
	char m_szConn[256];
	char m_szQuery[256];
	HRESULT m_hrLast;
	std::vector<std::vector<ioHashString> > m_datas;

public:
	int m_row;
	int m_col;

public:
	ioExcelReader();
	bool Open(LPCSTR path, LPCSTR sheet);
	bool LoadEnum( LPCSTR path, LPCSTR sheet, DataEumMap& mapEnum );
	bool LoadSheet( LPCSTR path, LPCSTR sheet, DataSheetMap& mapSheet );
	bool SetData(void *pData);
	bool SetEachValue(void *pData, const char* szType, const char* szValue, int& iAddPtr );
	const char* GetValue(int row, int col);
};


