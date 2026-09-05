

#include "stdafx.h"

#include "ioP2PChecker.h"

template<> ioP2PChecker* Singleton< ioP2PChecker >::ms_Singleton = 0;

ioP2PChecker::ioP2PChecker() : m_dwProcessTime(0)
{
	m_bView = false;
}

ioP2PChecker::~ioP2PChecker()
{	
	m_vPacketList.clear();
}

ioP2PChecker& ioP2PChecker::GetSingleton()
{
	return Singleton< ioP2PChecker >::GetSingleton();
}

void ioP2PChecker::Initialize()
{
	m_vPacketList.clear();
	m_vRecvLog.clear();
	m_dwProcessTime = FRAMEGETTIME();
}

void ioP2PChecker::AddSendPacket( DWORD dwID )
{
	vPacketData::iterator iter,iEnd;
	iEnd = m_vPacketList.end();
	for(iter = m_vPacketList.begin();iter != iEnd;iter++)
	{
		PacketData &kData = *iter;
		if( kData.m_dwID == dwID )
		{
			kData.m_iSendCount++;
			return;
		}
	}

	PacketData kData;
	kData.m_dwID = dwID;
	kData.m_iSendCount = 1;
	m_vPacketList.push_back( kData );
}

void ioP2PChecker::AddRecvPacket( DWORD dwID )
{
	vPacketData::iterator iter,iEnd;
	iEnd = m_vPacketList.end();
	for(iter = m_vPacketList.begin();iter != iEnd;iter++)
	{
		PacketData &kData = *iter;
		if( kData.m_dwID == dwID )
		{
			kData.m_iRecvCount++;
			return;
		}
	}

	PacketData kData;
	kData.m_dwID = dwID;
	kData.m_iRecvCount = 1;
	m_vPacketList.push_back( kData );
}

ioHashString ioP2PChecker::SendPacketLog( int iArray )
{
	static ioHashString szReturn = "none";
	
	std::sort( m_vPacketList.begin(), m_vPacketList.end(), PacketSendSort() );
	int iSize = m_vPacketList.size();
	if( !COMPARE( iArray, 0, iSize ) )
	{
		szReturn = "none";
	}
	else if( m_vPacketList[iArray].m_iSendCount == 0 )
	{
		szReturn = "none";
	}
	else
	{
		char szTemp[MAX_PATH] = "";
		sprintf( szTemp, "0x%x : %I64d", m_vPacketList[iArray].m_dwID, m_vPacketList[iArray].m_iSendCount );
		szReturn = szTemp;
	}

	return szReturn;
}

ioHashString ioP2PChecker::RecvPacketLog( int iArray )
{
	static ioHashString szReturn = "none";

	std::sort( m_vPacketList.begin(), m_vPacketList.end(), PacketRecvSort() );
	int iSize = m_vPacketList.size();
	if( !COMPARE( iArray, 0, iSize ) )
	{
		szReturn = "none";
	}
	else if( m_vPacketList[iArray].m_iRecvCount == 0 )
	{
		szReturn = "none";
	}
	else
	{
		char szTemp[MAX_PATH] = "";
		sprintf( szTemp, "0x%x : %I64d", m_vPacketList[iArray].m_dwID, m_vPacketList[iArray].m_iRecvCount );
		szReturn = szTemp;
	}

	return szReturn;
}

void ioP2PChecker::SwitchRecvLog()
{
	m_bView = !m_bView;
	if( m_bView )
		LOG.PrintTimeAndLog( 0, "Control UDP Log ON" );
	else
		LOG.PrintTimeAndLog( 0, "Control UDP Log OFF" );
}

void ioP2PChecker::SetControlLog( DWORD dwPacket, ioHashString szSendID, int iSendIndex, int iPrevIndex, bool bParsing )
{
	if( !IsRecvLog() ) return;

	if( iSendIndex - 1 == iPrevIndex )
		return;

	RecvLog kRecvLog;	
	switch( dwPacket )
	{
	case CUPK_CHAR_CONTROL:
		{
			char szLog[MAX_PATH] = "";
			sprintf( szLog, "CUPK_CHAR_CONTROL : %s ( %d, %d ) : %d", szSendID.c_str(), iPrevIndex, iSendIndex, bParsing );
			kRecvLog.m_szLog = szLog;
		}
		break;
	default:
		return;
	}	
	if( !bParsing )
		kRecvLog.m_dwColor = 0xFFFF0000;
	m_vRecvLog.push_back( kRecvLog );
	LOG.PrintTimeAndLog( 0, "%s", kRecvLog.m_szLog.c_str() );
}

void ioP2PChecker::RenderLog( int iXPos, int iYPos )
{
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	
	
	vRecvLog::reverse_iterator riter, riEnd;
	riEnd = m_vRecvLog.rend();
	int i = 0;
	for(riter = m_vRecvLog.rbegin();riter != riEnd;riter++)
	{
		RecvLog &kLog = *riter;
		g_FontMgr.SetTextColor( kLog.m_dwColor );
		g_FontMgr.PrintText( iXPos + 10, iYPos + 75 + (i*20), FONT_SIZE_13, kLog.m_szLog.c_str() );		
		if( ++i >= 10 )
			break;
	}
}
