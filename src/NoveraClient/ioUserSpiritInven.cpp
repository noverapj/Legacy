
#include "stdafx.h"
#include "ioUserSpiritInven.h"
#include "ioUserSpiritManager.h"

ioUserSpiritInven::ioUserSpiritInven()
{
	Initialize();
}

ioUserSpiritInven::~ioUserSpiritInven()
{
	m_vSpiritList.clear();
}

void ioUserSpiritInven::Initialize()
{
	m_vSpiritList.clear();
}

void ioUserSpiritInven::ApplySpiritData( SP2Packet &rkPacket )
{
	m_vSpiritList.clear();

	int nSize;
	rkPacket >> nSize;

	for( int i=0; i<nSize; ++i )
	{
		SpiritInvenInfo cInfo;
		rkPacket >> cInfo.m_iCode;
		rkPacket >> cInfo.m_iCount;

		m_vSpiritList.insert( SpiritInvenInfoMap::value_type( cInfo.m_iCode, cInfo ) );
	}
}

void ioUserSpiritInven::ApplySpiritSync( SP2Packet &rkPacket )
{
	SpiritInvenInfo cInfo;
	rkPacket >> cInfo.m_iCode;
	rkPacket >> cInfo.m_iCount;

	if( m_vSpiritList.find( cInfo.m_iCode ) == m_vSpiritList.end() )
		m_vSpiritList.insert( SpiritInvenInfoMap::value_type( cInfo.m_iCode, cInfo ) );
	else
		m_vSpiritList.find( cInfo.m_iCode )->second.m_iCount += cInfo.m_iCount;
}

void ioUserSpiritInven::FindSpiritInfo( int iCode, SpiritInvenInfo &rkItem )
{
	if( m_vSpiritList.find( iCode ) != m_vSpiritList.end() )
		rkItem = m_vSpiritList.find( iCode )->second;
}

int ioUserSpiritInven::GetSpiritCount( int iCode )
{
	if( m_vSpiritList.find( iCode ) != m_vSpiritList.end() )
		return m_vSpiritList.find( iCode )->second.m_iCount;

	return 0;
}

int ioUserSpiritInven::SetSpiritItem( int iCode, int iCount )
{
	if( m_vSpiritList.find( iCode ) != m_vSpiritList.end() )
	{
		int iPrevCount = m_vSpiritList.find( iCode )->second.m_iCount;
		m_vSpiritList.find( iCode )->second.m_iCount = iCount;

		return iCount - iPrevCount;
	}
	else
	{
		SpiritInvenInfo cInfo;
		cInfo.m_iCode = iCode;
		cInfo.m_iCount = iCount;
		m_vSpiritList.insert( SpiritInvenInfoMap::value_type( iCode, cInfo ) );

		return iCount;
	}
	
}