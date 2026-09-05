

#include "stdafx.h"

#include "ioTCPChecker.h"

template<> ioTCPChecker* Singleton< ioTCPChecker >::ms_Singleton = 0;

ioTCPChecker::ioTCPChecker() 
{

}

ioTCPChecker::~ioTCPChecker()
{	
	m_vCheckData.clear();
}

ioTCPChecker& ioTCPChecker::GetSingleton()
{
	return Singleton< ioTCPChecker >::GetSingleton();
}

void ioTCPChecker::AddRecvPacket( DWORD dwID )
{
	CheckData kData;
	kData.m_dwType = CheckType_Packet;
	kData.m_dwDataValue = dwID;
	kData.m_dwTime = FRAMEGETTIME();

	if( m_vCheckData.empty() )
		m_vCheckData.push_back( kData );
	else
		m_vCheckData.insert( m_vCheckData.begin(), kData );

	int iSize = m_vCheckData.size() - 1;
	if( iSize >= MAX_RECV_HIGH_SIZE )
		m_vCheckData.erase( m_vCheckData.begin() + iSize );
}


void ioTCPChecker::AddPlayModeChange( DWORD dwType /* = 0  */ )
{
	CheckData kData;
	kData.m_dwType = CheckType_PlayMode;
	kData.m_dwDataValue = dwType;
	kData.m_dwTime = FRAMEGETTIME();

	if( m_vCheckData.empty() )
		m_vCheckData.push_back( kData );
	else
		m_vCheckData.insert( m_vCheckData.begin(), kData );

	int iSize = m_vCheckData.size() - 1;
	if( iSize >= MAX_RECV_HIGH_SIZE )
		m_vCheckData.erase( m_vCheckData.begin() + iSize );
}

char *ioTCPChecker::GetLOG()
{
	memset( m_szLOG, 0, sizeof( m_szLOG ) );
	
	sprintf( m_szLOG, "Packet History : " );
	char szTemp[MAX_PATH] = "";
	int iSize = min( m_vCheckData.size(), MAX_RECV_HIGH_SIZE );
	for(int i = 0;i < iSize;i++)
	{
		CheckData &kData = m_vCheckData[i];
		if( kData.m_dwType == CheckType_Packet )
			sprintf( szTemp, "%x:%d,", kData.m_dwDataValue, ( FRAMEGETTIME() - kData.m_dwTime ) / 100 );
		else
			sprintf( szTemp, "(%d:%d),", kData.m_dwDataValue, ( FRAMEGETTIME() - kData.m_dwTime ) / 100 );
        strcat( m_szLOG, szTemp );
	}

	return m_szLOG;
}