#pragma once

#include "Ado.h"

class cTable
{
public:
	cTable();
	virtual ~cTable();

public:
	void	SetRecordset(ADODB::_RecordsetPtr pRecordSet)	{	m_pRecordSet =	pRecordSet;		}
	ADODB::_RecordsetPtr	GetRecordset()					{	return m_pRecordSet;			}	
	
public:
	bool	Assert();
	int		GetCount();
	int		GetFieldCount();

	HRESULT MoveNext();
	HRESULT MovePrevious();
	HRESULT MoveFirst();
	HRESULT MoveLast();

	bool	Get( const short field, _variant_t&  vtValue );

	bool	Get( const short field, int8& value );
	bool	Get( const short field, int16& value );
	bool	Get( const short field, int32& value );
	bool	Get( const short field, int64& value );

	bool	Get( const short field, uint8& value );
	bool	Get( const short field, uint16& value );
	bool	Get( const short field, uint32& value );
	bool	Get( const short field, uint64& value );
	
	bool	Get( const short field, float& value );
	bool	Get( const short field, double& value );
	bool	Get( const short field, DBTIMESTAMP &value );
	bool	Get( const short field, char* value, const int size );
	
public:
	void	SetQueryID( const uint32 queryId ) { m_queryId = queryId; }
	uint32	m_queryId;

private:
	ADODB::_RecordsetPtr	m_pRecordSet;
};
