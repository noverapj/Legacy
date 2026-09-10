#include <stdafx.h>
#include "ioExcelReader.h"

template<> ioExcelReader* Singleton< ioExcelReader >::ms_Singleton = 0;

ioAdoAutoInit::ioAdoAutoInit()
{
	 CoInitialize(NULL);
}

ioAdoAutoInit::~ioAdoAutoInit()
{
	 CoUninitialize();
}

ioExcelReader::ioExcelReader()
{
	m_row = 0;
	m_col = 0;
	memset(m_szConn, 0, sizeof(m_szConn));
	memset(m_szQuery, 0, sizeof(m_szQuery));
	m_hrLast = S_OK;
}

bool ioExcelReader::Open(LPCSTR path, LPCSTR sheet)
{
	_snprintf_s(m_szConn, _countof(m_szConn), _TRUNCATE, "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=%s;Extended Properties=\"Excel 12.0;IMEX=1;HDR=No;\";", path);
	_bstr_t strCnn = m_szConn;
	_RecordsetPtr pRstAuthors;

	m_hrLast = pRstAuthors.CreateInstance(__uuidof(Recordset));
	if(FAILED(m_hrLast))
	{
		if(pRstAuthors) 
			pRstAuthors.Release();
		return false;
	}

	_snprintf_s(m_szQuery, _countof(m_szQuery), _TRUNCATE, "SELECT * FROM [%s$]", sheet);	
	m_hrLast = pRstAuthors->Open(m_szQuery, strCnn, adOpenStatic, adLockReadOnly,adCmdText);
	if(FAILED(m_hrLast))
	{
		if(pRstAuthors) 
			pRstAuthors.Release();
		return false;
	}

	//pRstAuthors->MoveFirst();

	m_row = 0;
	m_col = 0;
	m_datas.clear();

	long r = 0;
	long c = 0;

	IntVec vecNoneReadCol;

	bool bBreak = false;

	for(r=0; !pRstAuthors->EndOfFile; pRstAuthors->MoveNext(), r++)
	{
		// 첫번째 열은 버전, 세번째 열은 열 이름 이므로 패스
		if( r == 0 || r == 3 )
		{
			continue;
		}
		// 두번째 열은 읽을 곳(클라, 서버, 공통)
		else if( r == 1 )
		{
			for(c=0; c<pRstAuthors->Fields->Count; c++)
			{
				FieldPtr field = pRstAuthors->Fields->GetItem(c);
				_variant_t val = field->Value;

				if( c == 0 )
					continue;

				if(VT_NULL != val.vt)
				{
					_bstr_t str = val;
					const char* sz = str;

					if( stricmp(sz, "END") == 0 )
						break;

					if( (stricmp(sz, "EXPORT") != 0) && (stricmp(sz, "CLIENT") != 0) && (stricmp(sz, "COMMON") != 0) )
						vecNoneReadCol.push_back(c);
				}
			}

			m_col = c;
		}
		// 데이터 타입과 실제 데이터를 읽어 온다
		else
		{
			std::vector<ioHashString> row;
			for(c=0; c<m_col; c++)
			{
				if( std::find( vecNoneReadCol.begin(), vecNoneReadCol.end(), c) != vecNoneReadCol.end() )
					continue;

				FieldPtr field = pRstAuthors->Fields->GetItem(c);
				_variant_t val = field->Value;
				if(VT_NULL == val.vt)
				{
					if( c != 0 )
						row.push_back("");

					continue;
				}
				else
				{
					_bstr_t str = val;
					const char* sz = str;

					if( c == 0 && (*sz) == ';')
						break;

					if( stricmp(sz, "REMARK") == 0 )
						break;

					if( c == 0 && ( _stricmp(sz, "END") == 0 ) )
					{
						bBreak = true;
						break;
					}

					if( stricmp(sz, "END") == 0 )
						break;

					if( stricmp(sz, "TYPE") == 0 )
						continue;

					row.push_back(sz);
				}
			}

			if( row.size() > 0 )
				m_datas.push_back(row);
		}

		if(bBreak)
			break;
	}

	m_col = m_col - 1 - vecNoneReadCol.size();
	m_row = m_datas.size();

	if(pRstAuthors) 
		pRstAuthors.Release();

	return true;
}

bool ioExcelReader::LoadEnum( LPCSTR path, LPCSTR sheet, DataEumMap& mapEnum )
{
	_snprintf_s(m_szConn, _countof(m_szConn), _TRUNCATE, "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=%s;Extended Properties=\"Excel 12.0;IMEX=1;HDR=No;\";", path);
	_bstr_t strCnn = m_szConn;
	_RecordsetPtr pRstAuthors;

	m_hrLast = pRstAuthors.CreateInstance(__uuidof(Recordset));
	if(FAILED(m_hrLast))
	{
		if(pRstAuthors) 
			pRstAuthors.Release();
		return false;
	}

	_snprintf_s(m_szQuery, _countof(m_szQuery), _TRUNCATE, "SELECT * FROM [%s$]", sheet);	
	m_hrLast = pRstAuthors->Open(m_szQuery, strCnn, adOpenStatic, adLockReadOnly,adCmdText);
	if(FAILED(m_hrLast))
	{
		if(pRstAuthors) 
			pRstAuthors.Release();
		return false;
	}

	long r = 0;
	long c = 0;

	int iEnumValue = 0;

	for(r=0; !pRstAuthors->EndOfFile; pRstAuthors->MoveNext(), r++)
	{
		if( r < 1 )
		{
			continue;
		}
		else
		{
			for( c = 0; c < 3; c++)
			{
				FieldPtr field = pRstAuthors->Fields->GetItem(c);
				_variant_t val = field->Value;
				if(VT_NULL != val.vt)
				{
					_bstr_t str = val;
					const char* sz = str;
					ioHashString strEnum = sz;

					if( c == 0 && stricmp( sz, ";") == 0 )
					{
						iEnumValue = 0;
						break;
					}

					if( stricmp(sz, "END") == 0 )
					{
						iEnumValue = 0;
						break;
					}

					if( stricmp(sz, "NAME") == 0 )
					{
						iEnumValue = 0;
						break;
					}

					if( c == 1 )
						continue;

					mapEnum.insert( std::make_pair(strEnum, iEnumValue) );
					iEnumValue++;
				}
			}
		}
	}

	if(pRstAuthors) 
		pRstAuthors.Release();

	return true;
}

bool ioExcelReader::LoadSheet( LPCSTR path, LPCSTR sheet, DataSheetMap& mapSheet )
{
	_snprintf_s(m_szConn, _countof(m_szConn), _TRUNCATE, "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=%s;Extended Properties=\"Excel 12.0;IMEX=1;HDR=No;\";", path);
	_bstr_t strCnn = m_szConn;
	_RecordsetPtr pRstAuthors;

	m_hrLast = pRstAuthors.CreateInstance(__uuidof(Recordset));
	if(FAILED(m_hrLast))
	{
		if(pRstAuthors) 
			pRstAuthors.Release();
		return false;
	}

	_snprintf_s(m_szQuery, _countof(m_szQuery), _TRUNCATE, "SELECT * FROM [%s$]", sheet);	
	m_hrLast = pRstAuthors->Open(m_szQuery, strCnn, adOpenStatic, adLockReadOnly,adCmdText);
	if(FAILED(m_hrLast))
	{
		if(pRstAuthors) 
			pRstAuthors.Release();
		return false;
	}

	long r = 0;
	long c = 0;

	for(r=0; !pRstAuthors->EndOfFile; pRstAuthors->MoveNext(), r++)
	{
		if( r < 1 )
		{
			continue;
		}
		else
		{
			ioHashString szSheet;
			ioHashString szFile;

			for( c = 0; c < 3; c++)
			{
				FieldPtr field = pRstAuthors->Fields->GetItem(c);
				_variant_t val = field->Value;
				if(VT_NULL != val.vt)
				{
					_bstr_t str = val;
					char* pPath = str;

					if( c == 0 && stricmp( str, ";") == 0 )
						break;

					if( c == 1 )
					{
						szFile = "..\\..\\Tool\\ioDataMaker\\";
						pPath++;
						szFile += pPath;
					}

					if( c == 2 )
						szSheet = str;
				}
			}

			if( szSheet.Length() > 0 && szFile.Length() > 0 )
				mapSheet.insert( std::make_pair(szSheet, szFile) );
		}
	}

	if(pRstAuthors)
		pRstAuthors.Release();

	return true;
}

const char* ioExcelReader::GetValue(int row, int col)
{
	if(row >= 0 && row < m_row)
	{
		if(col >= 0 && col < m_col)
		{
			return m_datas[row][col].c_str();
		}
	}

	return "";
}

bool ioExcelReader::SetData(void *pData)
{
	std::vector<ioHashString> vecType;

	int iAddPtr = 0;

	for( int c = 0; c < m_col; c++ )
	{
		const char* szType = GetValue(0, c);
		vecType.push_back(szType);
	}

	for( int r = 1; r < m_row; r++ )
	{
		for( int c = 0; c < m_col; c++ )
		{
			const char* szValue = GetValue(r, c);
			SetEachValue(pData, vecType[c].c_str(), szValue, iAddPtr);
		}
	}

	return true;
}

bool ioExcelReader::SetEachValue(void *pData, const char* szType, const char* szValue, int& iAddPtr )
{
	if( pData == NULL )
		return false;

	char* pStruct = (char*)pData;

	if( strnicmp(szType, "STRING", 5) == 0 )
	{
		char strType[512];
		memset(strType, 0, sizeof(strType));
		memcpy(strType, szType, strlen(szType));
		char *ptr = NULL;
		ptr = strtok(strType, "[");
		if( ptr != NULL )
		{
			ptr = strtok(NULL, "[");
			if( ptr != NULL )
			{
				ptr = strtok(ptr, "]");
			}
		}

		if( ptr != NULL )
		{
			INT iLength = atoi(ptr);
			//char szStr[1024];
			//memset( szStr, 0, 1024 );
			//memcpy( szStr, szValue, sizeof(szValue) );
			memset( (pStruct + iAddPtr), 0, iLength);
			memcpy( (pStruct + iAddPtr), szValue, strlen(szValue));
			iAddPtr += iLength;
		}
	}
	else if( strnicmp(szType, "ENUM", 4) == 0 )
	{
		int iEnum = g_TableDataMgr.GetEnumValue( szValue );
		memcpy( (pStruct + iAddPtr), &iEnum, sizeof(INT));
		iAddPtr += sizeof(INT);
	}
	else if( stricmp(szType, "BYTE") == 0 )
	{
		BYTE byData = (BYTE)atoi(szValue);
		memcpy( (pStruct + iAddPtr), &byData, sizeof(BYTE));
		iAddPtr += sizeof(BYTE);
	}
	else if( stricmp(szType, "WORD") == 0 )
	{
		WORD wData = (WORD)atoi(szValue);
		memcpy( (pStruct + iAddPtr), &wData, sizeof(WORD));
		iAddPtr += sizeof(WORD);
	}
	else if( stricmp(szType, "DWORD") == 0 )
	{
		DWORD dwData = (DWORD)atoi(szValue);
		memcpy( (pStruct + iAddPtr), &dwData, sizeof(DWORD));
		iAddPtr += sizeof(DWORD);
	}
	else if( stricmp(szType, "SHORT") == 0 )
	{
		SHORT sData = (SHORT)atoi(szValue);
		memcpy( (pStruct + iAddPtr), &sData, sizeof(SHORT));
		iAddPtr += sizeof(SHORT);
	}
	else if( stricmp(szType, "USHORT") == 0 )
	{
		USHORT usData = (USHORT)atoi(szValue);
		memcpy( (pStruct + iAddPtr), &usData, sizeof(USHORT));
		iAddPtr += sizeof(USHORT);
	}
	else if( stricmp(szType, "INT") == 0 )
	{
		INT iData = atoi(szValue);
		memcpy( (pStruct + iAddPtr), &iData, sizeof(INT));
		iAddPtr += sizeof(INT);
	}
	else if( stricmp(szType, "UINT") == 0 )
	{
		UINT uiData = (UINT)atoi(szValue);
		memcpy( (pStruct + iAddPtr), &uiData, sizeof(UINT));
		iAddPtr += sizeof(UINT);
	}
	else if( stricmp(szType, "LONG") == 0 )
	{
		LONG lData = (LONG)atol(szValue);
		memcpy( (pStruct + iAddPtr), &lData, sizeof(LONG));
		iAddPtr += sizeof(LONG);
	}
	else if( stricmp(szType, "ULONG") == 0 )
	{
		ULONG ulData = (ULONG)atol(szValue);
		memcpy( (pStruct + iAddPtr), &ulData, sizeof(ULONG));
		iAddPtr += sizeof(ULONG);
	}
	else if( stricmp(szType, "FLOAT") == 0 )
	{
		float fData = (float)atof(szValue);
		memcpy( (pStruct + iAddPtr), &fData, sizeof(float));
		iAddPtr += sizeof(float);
	}

	return true;
}