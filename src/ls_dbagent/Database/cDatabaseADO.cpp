
#include "../stdafx.h"
#include "../util/cSerialize.h"
#include <iostream>
#include <oledberr.h>
#include "cDatabaseADO.h"

using namespace std;

#ifdef _DEBUG
#define PRINT_QUERY 1
#endif

void ErrorHandler(_com_error &e, const char* message)
{
	char buffer[1024];

	sprintf_s(buffer, _T("Error:\r\n"));
	sprintf_s(buffer, _T("%sCode meaning = %s\r\n"), buffer, (char*)e.ErrorMessage());
	sprintf_s(buffer, _T("%sDescription = %s"), buffer, (char*)e.Description());

	Trace("%s\n", buffer );
	LOG.PrintTimeAndLog( 0, "%s, %s", buffer, message );
}


//////////////////////////////////////////////////////////////////////
// cConnection::Construction/Destruction
//////////////////////////////////////////////////////////////////////

cConnection::cConnection()
{
	Init();
}

cConnection::~cConnection()
{
	Destroy();
}

void cConnection::Init()
{
	m_connection	= NULL;
	m_command		= NULL;

	m_connName		= new cBuffer;
	if(m_connName)
		m_connName->Create(1024);

	m_userName		= new cBuffer;
	if(m_userName)
		m_userName->Create(1024);

	m_userPassword	= new cBuffer;
	if(m_userPassword)
		m_userPassword->Create(1024);

}

void cConnection::Destroy()
{
	Close();
/*
	if(m_connName)
		delete m_connName;

	if(m_userName)
		delete m_userName;

	if(m_userPassword)
		delete m_userPassword;
*/
}

//////////////////////////////////////////////////////////////////////
// cConnection::Operation
//////////////////////////////////////////////////////////////////////

BOOL cConnection::Create()
{
	if(NULL == m_connection)
	{
		HRESULT hr = m_connection.CreateInstance(__uuidof(ADODB::Connection));
		if(SUCCEEDED(hr))
		{
			m_connection->ConnectionTimeout = 10; // default 10sec

			if(NULL == m_command)
			{
				m_command.CreateInstance(__uuidof(ADODB::Command));
			}
			return TRUE;
		}

		Information( "[%lu]DB Connection...failed(%lu)\n", GetCurrentThreadId(), GetLastError() );
		return FALSE;
	}
	return TRUE;
}

BOOL cConnection::Open()
{
	try
	{
		if(Create())
		{
			HRESULT hr = m_connection->Open(	
				_bstr_t(m_connName->GetString()), 
				m_userName->GetString(), 
				m_userPassword->GetString(), 
				ADODB::adConnectUnspecified);

			return SUCCEEDED(hr);
		}
		return FALSE;
	}
	catch(_com_error &e)
	{
		ErrorHandler(e, _T("Open"));
		Close();
		return FALSE;
	}
	return TRUE;
}

BOOL cConnection::Open(const char *userName, const char *password, const char *connName)
{
	m_connName->Copy(connName);
	m_userName->Copy(userName);
	m_userPassword->Copy(password);

	return Open();
}

BOOL cConnection::Open(const char *userName, const char *password, const char *dbName, const char *IP, const uint32 port)
{
	char szBuffer[1024];
	sprintf_s(
		szBuffer, 
		_T("Provider=SQLOLEDB.1;Persist Security Info=FALSE;User ID=%s;Initial Catalog=%s;Data Source=%s,%lu"),
		userName,
		dbName,
		IP,
		port);

	m_connName->Copy(szBuffer);
	m_userName->Copy(userName);
	m_userPassword->Copy(password);
	m_port = port;

	return Open();
}

void cConnection::Close()
{
	if(m_connection && (ADODB::adStateOpen == m_connection->GetState()))
	{
		m_connection->Close();
	}
	m_connection = NULL;
}

BOOL cConnection::Execute(CQueryData *queryData, FACTORS& factorList, const char* queryName, cTable* table)
{
	if(!queryData)
	{
		Debug(_T("쿼리  버퍼가 없음\r\n"));
		return FALSE;
	}

	ADODB::_RecordsetPtr ptrRecordSet = NULL;

#ifdef PRINT_QUERY
	char query[4096] = {0}, column[1024] = {0};
	sprintf_s( query, "%s ", queryName );
#endif

	try
	{	

		if(m_connection)
		{
			ClearParameters();
			
			cSerialize fields;
			queryData->GetFields( fields );

			for(FACTORS::const_iterator it = factorList.begin() ; it != factorList.end() ; ++it)
			{
				switch(*it)
				{
				case FT_INT8 :
					{
						uint8 value;
						if(!fields.GetUInt(value)) return FALSE;

						AddParameter(_T("uint8"), ADODB::adUnsignedTinyInt, ADODB::adParamInput, sizeof(uint8), value);

#ifdef PRINT_QUERY
						sprintf_s( column, "%lu", value );
#endif
					}
					break;
				case FT_INT16 :
					{
						int16 value;
						if(!fields.GetInt(value)) return FALSE;

						AddParameter(_T("int16"), ADODB::adSmallInt, ADODB::adParamInput, sizeof(int16), value);

#ifdef PRINT_QUERY
						sprintf_s( column, "%lu", value );
#endif
					}
					break;
				case FT_INT32 :
					{
						int32 value;
						if(!fields.GetInt(value)) return FALSE;

						AddParameter(_T("int32"), ADODB::adInteger, ADODB::adParamInput, sizeof(int32), value);

#ifdef PRINT_QUERY
						sprintf_s( column, "%d", value );
#endif
					}
					break;
				case FT_INT64 :
					{
						int64 value;
						if(!fields.GetInt(value)) return FALSE;

						AddParameter(_T("int64"), ADODB::adBigInt, ADODB::adParamInput, sizeof(int64), value);

#ifdef PRINT_QUERY
						sprintf_s( column, "%I64d", value );
#endif
					}
					break;
				case FT_FLOAT :
					{
						float value;
						if(!fields.GetFloat(value)) return FALSE;

						AddParameter(_T("float"), ADODB::adSingle, ADODB::adParamInput, sizeof(float), (double)value);

#ifdef PRINT_QUERY
						sprintf_s( column, "%lf", value );
#endif
					}	
					break;
				case FT_DATETIME :
					{
						SYSTEMTIME value;
						if(!fields.GetBuffer((uint8*)&value, sizeof(SYSTEMTIME)))	return FALSE;
						
						double time = 0;
						SystemTimeToVariantTime(&value, &time);

						AddParameter(_T("datetime"), ADODB::adDBTimeStamp, ADODB::adParamInput, sizeof(double), time);

#ifdef PRINT_QUERY
						sprintf_s( column, "'%d-%d-%d %d:%d:%d'", value.wYear, value.wMonth, value.wDay, value.wHour, value.wMinute, value.wSecond );
#endif
					}
					break;
				case FT_STRING :
					{
						char temp[4096] = {0};
						if(!fields.GetString(temp, sizeof(temp)))	return FALSE;
						
						uint32 length = _tcslen(temp);
						if(!length) length = 1;

#ifdef _UNICODE
						AddParameter(_T("string"), ADODB::adVarWChar, ADODB::adParamInput, length, temp);
#else
						AddParameter(_T("string"), ADODB::adVarChar, ADODB::adParamInput, length, temp);
#endif
#ifdef PRINT_QUERY
						sprintf_s( column, "'%s'", temp );
#endif
					}
					break;
				default :
					{
						return FALSE;
					}
				}

#ifdef PRINT_QUERY
				if(it != factorList.begin())
				{
					strcat_s( query, ", " );
				}
				strcat_s( query, column );
#endif
			}

#ifdef PRINT_QUERY
			Information( "%s\n", query );
#endif
			m_command->ActiveConnection	= m_connection;
			m_command->CommandType		= ADODB::adCmdStoredProc;
			m_command->CommandText		= _bstr_t(queryName);
			m_connection->CursorLocation= ADODB::adUseClient;
			ptrRecordSet = m_command->Execute(NULL, NULL, ADODB::adCmdStoredProc);
		}
		else
		{
			// if closed, try to connect
			cConnection::Open();
			Information( "### 디비 연결오류 1\n" );
			LOG.PrintTimeAndLog(0, "### 디비 연결오류 1\n" );
			return FALSE;
		}
	}
	catch(_com_error &e)
	{
		ErrorHandler(e, queryName);

		switch(e.Error())
		{
		case DB_E_INTEGRITYVIOLATION:
		case DB_E_ERRORSINCOMMAND:
			{
				Information( "### 디비 쿼리오류\n" );
				#ifdef PRINT_QUERY
					LOG.PrintTimeAndLog(0, "[warning][db]Failed to execute[QUERY: EXEC %s]", query );
				#else
					LOG.PrintTimeAndLog(0, "[warning][db]Failed to execute" );
				#endif
			}
			break;

		default:
			{
				Information( "### 디비 연결오류 2\n" );
				LOG.PrintTimeAndLog(0, "[critical][db]Failed to connect[2]" );

				Close();
				Open();
			}
			break;
		}
		return FALSE;
	}

	if(table && (ptrRecordSet->State == ADODB::adStateOpen))
	{
		table->SetRecordset(ptrRecordSet);
	}

	return TRUE;
}

BOOL cConnection::Generate(CQueryData *queryData, FACTORS& factorList, const char* queryName, char *buffer, const int length)
{
	if(!queryData)
	{
		Debug(_T("쿼리  버퍼가 없음\r\n"));
		return FALSE;
	}

	char column[1024] = {0};
	sprintf_s( buffer, length, "%s ", queryName );

	cSerialize fields;
	queryData->GetFields( fields );

	for(FACTORS::const_iterator it = factorList.begin() ; it != factorList.end() ; ++it)
	{
		switch(*it)
		{
		case FT_INT8 :
			{
				uint8 value;
				if(!fields.GetUInt(value)) return FALSE;

				sprintf_s( column, "%lu", value );
			}
			break;
		case FT_INT16 :
			{
				int16 value;
				if(!fields.GetInt(value)) return FALSE;

				sprintf_s( column, "%lu", value );
			}
			break;
		case FT_INT32 :
			{
				int32 value;
				if(!fields.GetInt(value)) return FALSE;

				sprintf_s( column, "%d", value );
			}
			break;
		case FT_INT64 :
			{
				int64 value;
				if(!fields.GetInt(value)) return FALSE;

				sprintf_s( column, "%I64d", value );
			}
			break;
		case FT_FLOAT :
			{
				float value;
				if(!fields.GetFloat(value)) return FALSE;

				sprintf_s( column, "%lf", value );
			}	
			break;
		case FT_DATETIME :
			{
				SYSTEMTIME value;
				if(!fields.GetBuffer((uint8*)&value, sizeof(SYSTEMTIME)))	return FALSE;
						
				sprintf_s( column, "'%d-%d-%d %d:%d:%d'", value.wYear, value.wMonth, value.wDay, value.wHour, value.wMinute, value.wSecond );
			}
			break;
		case FT_STRING :
			{
				char temp[4096] = {0};
				if(!fields.GetString(temp, sizeof(temp)))	return FALSE;
						
				uint32 length = _tcslen(temp);
				if(!length) length = 1;

				sprintf_s( column, "'%s'", temp );
			}
			break;
		default :
			{
				return FALSE;
			}
		}
		if(it != factorList.begin())
		{
			strcat_s( buffer, length, ", " );
		}
		strcat_s( buffer, length, column );
	}
	return TRUE;
}

void cConnection::ClearParameters()
{
	while(m_command->Parameters->Count > 0) 
	{
		m_command->Parameters->Delete(0L);
	}
}

BOOL cConnection::AddParameter(const char* name, ADODB::DataTypeEnum dataType, ADODB::ParameterDirectionEnum direction, long size, int8 value)
{
	_variant_t vtValue;

	vtValue.vt		= VT_I1;
	vtValue.iVal	= value;

	return AddParameter(name, dataType, direction, size, vtValue);
}

BOOL cConnection::AddParameter(const char* name, ADODB::DataTypeEnum dataType, ADODB::ParameterDirectionEnum direction, long size, uint8 value)
{
	_variant_t vtValue;

	vtValue.vt		= VT_UI1;
	vtValue.iVal	= value;

	return AddParameter(name, dataType, direction, size, vtValue);
}

BOOL cConnection::AddParameter(const char* name, ADODB::DataTypeEnum dataType, ADODB::ParameterDirectionEnum direction, long size, int16 value)
{
	_variant_t vtValue;

	vtValue.vt		= VT_I2;
	vtValue.iVal	= value;

	return AddParameter(name, dataType, direction, size, vtValue);
}

BOOL cConnection::AddParameter(const char* name, ADODB::DataTypeEnum dataType, ADODB::ParameterDirectionEnum direction, long size, int32 value)
{
	_variant_t vtValue;

	vtValue.vt		= VT_I4;
	vtValue.lVal	= value;

	return AddParameter(name, dataType, direction, size, vtValue);
}

BOOL cConnection::AddParameter(const char* name, ADODB::DataTypeEnum dataType, ADODB::ParameterDirectionEnum direction, long size, int64 value)
{
	_variant_t vtValue;

	vtValue.vt		= VT_I8;
	vtValue.llVal	= value;

	return AddParameter(name, dataType, direction, size, vtValue);
}

BOOL cConnection::AddParameter(const char* name, ADODB::DataTypeEnum dataType, ADODB::ParameterDirectionEnum direction, long size, double value)
{
	_variant_t vtValue;

	vtValue.vt		= VT_R8;
	vtValue.dblVal	= value;

	return AddParameter(name, dataType, direction, size, vtValue);
}

BOOL cConnection::AddParameter(const char* name, ADODB::DataTypeEnum dataType, ADODB::ParameterDirectionEnum direction, long size, const char* value)
{
	_variant_t vtValue;

	vtValue.vt		= VT_BSTR;
	vtValue.bstrVal = _bstr_t(value);

	return AddParameter(name, dataType, direction, size, vtValue);
}

BOOL cConnection::AddParameter(const char* name, ADODB::DataTypeEnum dataType, ADODB::ParameterDirectionEnum direction, long size, _variant_t value, int precision, int scale)
{
	try
	{
		ADODB::_ParameterPtr ptrParam = m_command->CreateParameter(name, dataType, direction, size, value);
		if(NULL != ptrParam)
		{
			ptrParam->PutPrecision(precision);
			ptrParam->PutNumericScale(scale);
			m_command->Parameters->Append(ptrParam);
			return TRUE;
		}
		return FALSE;
	}
	catch(_com_error& e)
	{
		ErrorHandler(e, _T("AddParameter"));
		return FALSE;
	}
}

//////////////////////////////////////////////////////////////////////
// cDatabaseADO::Construction/Destruction
//////////////////////////////////////////////////////////////////////

cDatabaseADO::cDatabaseADO()
{
	Init();
}

cDatabaseADO::~cDatabaseADO()
{
	Destroy();
}

void cDatabaseADO::Init()
{
}

void cDatabaseADO::Destroy()
{
}


//////////////////////////////////////////////////////////////////////
// startup/cleanup
//////////////////////////////////////////////////////////////////////

BOOL cDatabaseADO::Startup()
{
	return (FAILED(::CoInitialize(NULL)) ? FALSE : TRUE);
}

void cDatabaseADO::Cleanup()
{
	::CoUninitialize();
}

//////////////////////////////////////////////////////////////////////
// create/open/close
//////////////////////////////////////////////////////////////////////

BOOL cDatabaseADO::Create(const uint32 nID)
{
	CONNECTION_TABLE::const_iterator it = m_cConnectionTable.find(nID);
	if(it == m_cConnectionTable.end())
	{
		cConnection* pConnection = new cConnection;
		pConnection->Create();

		m_cConnectionTable[nID] = pConnection;
		return TRUE;
	}
	return FALSE;
}

void cDatabaseADO::Close(const uint32 nID)
{
	CONNECTION_TABLE::const_iterator it = m_cConnectionTable.find(nID);
	if(it != m_cConnectionTable.end())
	{
		cConnection* pConnection = it->second;
		pConnection->Close();
	}
}

BOOL cDatabaseADO::Open(const uint32 nID)
{
	CONNECTION_TABLE::const_iterator it = m_cConnectionTable.find(nID);
	if(it != m_cConnectionTable.end())
	{
		cConnection* pConnection = it->second;
		return pConnection->Open();
	}
	return FALSE;
}

BOOL cDatabaseADO::Open(const uint32 nID, const char *userName, const char *password, const char *connName)
{
	CONNECTION_TABLE::const_iterator it = m_cConnectionTable.find(nID);
	if(it != m_cConnectionTable.end())
	{
		cConnection* pConnection = it->second;
		return pConnection->Open(userName, password, connName);
	}
	return FALSE;
}

BOOL cDatabaseADO::Open(const uint32 nID, const char *userName, const char *password, const char *dbName, const char *IP, const uint32 port)
{
	CONNECTION_TABLE::const_iterator it = m_cConnectionTable.find(nID);
	if(it != m_cConnectionTable.end())
	{
		cConnection* pConnection = it->second;
		return pConnection->Open(userName, password, dbName, IP, port);
	}
	return FALSE;
}


//////////////////////////////////////////////////////////////////////
// execute
//////////////////////////////////////////////////////////////////////

BOOL cDatabaseADO::Execute(const uint32 nID, CQueryData *queryData, FACTORS& factorList,const char *queryName, cTable* table)
{
	if(0 != nID) // 0이 아닌 지정일 경우 지정된 데이타베이스에 보낸다, 2011-04-05 youngdie
	{
		CONNECTION_TABLE::const_iterator it = m_cConnectionTable.find(nID);
		if(it != m_cConnectionTable.end())
		{
			cConnection* pConnection = it->second;
			return pConnection->Execute(queryData, factorList, queryName, table);
		}
		return FALSE;
	}
	else // 0일 경우 모든 데이타베이스에 보낸다, 2011-04-05 youngdie
	{
		for(CONNECTION_TABLE::const_iterator it = m_cConnectionTable.begin() ; it != m_cConnectionTable.end() ; ++it)
		{
			cConnection* pConnection = it->second;
			if(pConnection)
				pConnection->Execute(queryData, factorList, queryName, table);
		}
	}
	return TRUE;
}

BOOL cDatabaseADO::Generate(const uint32 nID, CQueryData *queryData, FACTORS& factorList, const char *queryName, char *buffer, const int length)
{
	CONNECTION_TABLE::const_iterator it = m_cConnectionTable.find(nID);
	if(it != m_cConnectionTable.end())
	{
		cConnection* pConnection = it->second;
		return pConnection->Generate(queryData, factorList, queryName, buffer, length);
	}
	return FALSE;
}



