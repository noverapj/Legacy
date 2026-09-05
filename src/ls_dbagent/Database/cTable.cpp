// cTable.cpp: implementation of the cTable class.
//
//////////////////////////////////////////////////////////////////////
#include "../stdafx.h"
#include <iostream>
#include "cTable.h"


extern void ErrorHandler(_com_error &e, const char* message);


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cTable::cTable()
{
	m_pRecordSet = NULL;
}

cTable::~cTable()
{
	if(m_pRecordSet)
	{
		if(!(m_pRecordSet->EndOfFile && m_pRecordSet->BOF))
		{
			m_pRecordSet->Close();
		}
		m_pRecordSet = NULL;
	}
}


//////////////////////////////////////////////////////////////////////
// assert
//////////////////////////////////////////////////////////////////////

bool	cTable::Assert()
{
	if( m_pRecordSet == NULL )	
		return false;

	return (m_pRecordSet->EndOfFile == VARIANT_FALSE) ? true : false;
}

//////////////////////////////////////////////////////////////////////
// record count
//////////////////////////////////////////////////////////////////////

int		cTable::GetCount()
{
	int count = 0;

	try
	{
		if(m_pRecordSet)
		{
			count = m_pRecordSet->GetRecordCount();
		}
	}
	catch(_com_error &e)
	{
		LOG.PrintTimeAndLog(0, "Table FAILED : %lu", m_queryId );

		ErrorHandler(e, _T("GetCount"));
	}

	return count;
}

int		cTable::GetFieldCount()
{
	if(m_pRecordSet)
	{
		int count = m_pRecordSet->Fields->Count;
		return count;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////
// cursor
//////////////////////////////////////////////////////////////////////
HRESULT cTable::MoveNext()
{
	if( !Assert() )	
		return S_FALSE;

	return m_pRecordSet->MoveNext();
}

HRESULT cTable::MovePrevious()
{
	if( !Assert() )	
		return S_FALSE;

	return m_pRecordSet->MovePrevious();
}

HRESULT cTable::MoveFirst()
{
	return m_pRecordSet->MoveFirst();
}

HRESULT cTable::MoveLast()
{
	return m_pRecordSet->MoveLast();
}

//////////////////////////////////////////////////////////////////////
// get value
//////////////////////////////////////////////////////////////////////

/*
	CHAR cVal - VT_I1
	SHORT iVal - VT_I2
	LONG lVal - VT_I4
	LONGLONG llVal - VT_I8

	BYTE bVal - VT_UI1
	USHORT uiVal - VT_UI2
	ULONG ulVal - VT_UI4
	ULONGLONG ullVal - VT_UI8

	FLOAT fltVal - VT_R4
	DOUBLE dblVal - VT_R8

	INT intVal - VT_INT
	UINT uintVal - VT_UINT

	VARIANT_BOOL boolVal - VT_BOOL
	_VARIANT_BOOL bool - VT_BOOL
	DATE date - VT_DATE
	BSTR bstrVal - VT_BSTR
*/

bool	cTable::Get( const short field, _variant_t& vtValue )
{
	if( !Assert() )	return false;
	
	try
	{
		//if(field >= GetCount())
		//	return false;

		vtValue = m_pRecordSet->Fields->GetItem(field)->GetValue();
	}
	catch( _com_error &e )
	{
		LOG.PrintTimeAndLog(0, "Table FAILED : %lu", m_queryId );

		ErrorHandler(e, _T("Get"));
		vtValue = 0;
		return true;
	}

	return true;
}

bool	cTable::Get( const short field, int8& value )
{
	_variant_t vtValue;
	if( !Get( field, vtValue ) )
		return false;

	value = (VT_NULL == vtValue.vt) ? 0 : vtValue.cVal;
	return true;
}

bool	cTable::Get( const short field, int16& value )
{
	_variant_t vtValue;
	if( !Get( field, vtValue ) )
		return false;

	value = (VT_NULL == vtValue.vt) ? 0 : vtValue.iVal;
	return true;
}

bool	cTable::Get( const short field, int32& value )
{
	_variant_t vtValue;
	if( !Get( field, vtValue ) )
		return false;

	value = (VT_NULL == vtValue.vt) ? 0 : vtValue.lVal;
	return true;
}

bool	cTable::Get( const short field, int64& value )
{
	_variant_t vtValue;
	if( !Get( field, vtValue ) )
		return false;

	value = (VT_NULL == vtValue.vt) ? 0 : vtValue.llVal;
	return true;
}

bool	cTable::Get( const short field, uint8& value )
{
	_variant_t vtValue;
	if( !Get( field, vtValue ) )
		return false;

	value = (VT_NULL == vtValue.vt) ? 0 : vtValue.bVal;
	return true;
}

bool	cTable::Get( const short field, uint16& value )
{
	_variant_t vtValue;
	if( !Get( field, vtValue ) )
		return false;

	value = (VT_NULL == vtValue.vt) ? 0 : vtValue.uiVal;
	return true;
}

bool	cTable::Get( const short field, uint32& value )
{
	_variant_t vtValue;
	if( !Get( field, vtValue ) )
		return false;

	value = (VT_NULL == vtValue.vt) ? 0 : vtValue.ulVal;
	return true;
}
bool	cTable::Get( const short field, uint64& value )
{
	_variant_t vtValue;
	if( !Get( field, vtValue ) )
		return false;

	value = (VT_NULL == vtValue.vt) ? 0 : vtValue.ullVal;
	return true;
}

bool	cTable::Get( const short field, float &value )
{
	_variant_t vtValue;
	if( !Get( field, vtValue ) )
		return false;

	value = (VT_NULL == vtValue.vt) ? 0 : vtValue.fltVal;
	return true;
}

bool	cTable::Get( const short field, double &value )
{
	_variant_t vtValue;
	if( !Get( field, vtValue ) )
		return false;

	value = (VT_NULL == vtValue.vt) ? 0 : vtValue.dblVal;
	return true;
}

bool	cTable::Get( const short field, DBTIMESTAMP &value )
{
	_variant_t  vtValue;
	if( !Get( field, vtValue ) )
		return false;

	if(VT_NULL == vtValue.vt)
	{
		ZeroMemory(&value, sizeof(DBTIMESTAMP));
	}
	else
	{
		SYSTEMTIME st;
		VariantTimeToSystemTime(vtValue.dblVal, &st);
	
		value.year		= st.wYear;
		value.month		= st.wMonth;
		value.day		= st.wDay;
 		value.hour		= st.wHour;
		value.minute	= st.wMinute;
		value.second	= st.wSecond;
		value.fraction	= st.wMilliseconds * 1000000;
	}
	return true;
}

bool	cTable::Get( const short field, char *value, const int size )
{
	try
	{
		_variant_t vtValue;
		if( !Get( field, vtValue ) )
			return false;

		if(VT_NULL == vtValue.vt)
		{
			ZeroMemory( value, size );
			return true;
		}

		//BYTE bVal - VT_UI1
		if( size == 1 )
		{
			value[0] = vtValue.bVal;
		}
		else
		{
			if (NULL != vtValue.bstrVal)
			{
				ZeroMemory(value, size);
				strncpy_s(value, size, (LPCTSTR)((_bstr_t)vtValue.bstrVal), _TRUNCATE);
			}
		}
		return true;
	}
	catch(_com_error &e)
	{
		LOG.PrintTimeAndLog(0, "Table FAILED : %lu", m_queryId );

		ErrorHandler(e, _T("Get"));
	}
	return false;
}
