#include "stdafx.h"
#include "ioProcessChecker.h"
#include "Network/GameServer.h"
#include "Network/ioPacketQueue.h"

extern CLog ProcessLOG;

FunctionTimeChecker::FunctionTimeChecker( LPSTR pFileName, LONG iFileLine, double fCheckMicSec, DWORD dwPacketID ) : 
										m_bFrequency( false ), m_iFileLine( iFileLine ), m_fCheckMicSec( fCheckMicSec ), m_dwPacketID( dwPacketID )
{
	m_pFileName = pFileName;
	if( QueryPerformanceFrequency( &m_freq ) )
	{
		QueryPerformanceCounter( (LARGE_INTEGER *)&m_start );
		m_bFrequency = true;
	}
}

FunctionTimeChecker::~FunctionTimeChecker()
{
	if( m_bFrequency )
	{
		QueryPerformanceCounter( (LARGE_INTEGER *)&m_end );

		double fCompleteTime = (double)(m_end-m_start) / m_freq.QuadPart*1000000;

		if( fCompleteTime > m_fCheckMicSec )
		{
			if( m_pFileName )
				ProcessLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s %d Line 0x%x : T[%.3f]", m_pFileName, m_iFileLine, m_dwPacketID, fCompleteTime / 1000000 );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
ioProcessChecker *ioProcessChecker::sg_Instance = NULL;
ioProcessChecker::ioProcessChecker() : m_dwLogTime( 0 ), m_dwCurTime( 0 )
{
	Initialize();
}

ioProcessChecker::~ioProcessChecker()
{

}

ioProcessChecker &ioProcessChecker::GetInstance()
{
	if( sg_Instance == NULL )
		sg_Instance = new ioProcessChecker;
	return *sg_Instance;
}

void ioProcessChecker::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void ioProcessChecker::LoadINI()
{
	ioINILoader kLoader;
	kLoader.ReloadFile( "config/sp2_process.ini" );
	kLoader.SetTitle( "ProcessChecker" );
	m_dwLogTime = kLoader.LoadInt( "LogTime", 0 );
}

void ioProcessChecker::Initialize()
{
	// PROCESS TIME CHECK
	m_fMainTreadMaxTime = 0.0f;
	m_MainThreadTime.m_bFrequency = false;
	m_MainThreadTime.m_start = 0;
	m_MainThreadTime.m_end   = 0;

	m_fBroadcastTreadMaxTime = 0.0f;
	m_BroadcastThreadTime.m_bFrequency = false;
	m_BroadcastThreadTime.m_start = 0;
	m_BroadcastThreadTime.m_end   = 0;

	m_fWorkTreadMaxTime = 0.0f;
	m_WorkThreadTime.m_bFrequency = false;
	m_WorkThreadTime.m_start = 0;
	m_WorkThreadTime.m_end   = 0;

	m_fServerATreadMaxTime = 0.0f;
	m_ServerAThreadTime.m_bFrequency = false;
	m_ServerAThreadTime.m_start = 0;
	m_ServerAThreadTime.m_end   = 0;

	m_fClientATreadMaxTime = 0.0f;
	m_ClientAThreadTime.m_bFrequency = false;
	m_ClientAThreadTime.m_start = 0;
	m_ClientAThreadTime.m_end   = 0;

	m_fMonitoringATreadMaxTime = 0.0f;
	m_MonitoringAThreadTime.m_bFrequency = false;
	m_MonitoringAThreadTime.m_start = 0;
	m_MonitoringAThreadTime.m_end   = 0;

	// PROCESS LOOP
	m_dwMainLoop = m_dwLogDBLoop = m_dwUDPLoop = 0;	          
	m_dwClientAcceptLoop = m_dwServerAcceptLoop = 0;
	m_dwMonitoringAcceptLoop = 0;

	// PACKET
	m_iUserSend = 0;
	m_iUserSendComplete = 0;
	m_iUserRecv = 0;
	m_iServerSend = 0;
	m_iServerSendComplete = 0;
	m_iServerRecv = 0;
	m_iMServerSend = 0;
	m_iMServerSendComplete = 0;
	m_iMServerRecv = 0;
	m_iDBServerSend = 0;
	m_iDBServerSendComplete = 0;
	m_iDBServerRecv = 0;
	m_iLogDBSend = 0;		
	m_iLogDBRecv = 0;		
	m_iUDPSend   = 0;			
	m_iUDPRecv   = 0;		
	m_iBillingRelayServerSend         = 0;
	m_iBillingRelayServerSendComplete = 0;
	m_iBillingRelayServerRecv         = 0;
	m_iMainProcessMaxPacket           = 0;
	m_iMonitoringSend         = 0;
	m_iMonitoringSendComplete = 0;
	m_iMonitoringRecv         = 0;
}

void ioProcessChecker::Process()
{
	if( m_dwLogTime == 0 ) return;

	if( TIMEGETTIME() - m_dwCurTime > m_dwLogTime )
	{
		m_dwCurTime = TIMEGETTIME();
		WriteLOG();
	}
}

void ioProcessChecker::WriteLOG()
{
	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	//WorkThread가 1개뿐이라서 그냥 남기지만 늘어난다면 나머지도 남겨줘야한다.
	static char szLOG[2000] = "";
	sprintf_s( szLOG, "[M:%d(%.3fsec)][MP:%d][L:%d][U:%d(%.3fsec)][C:%d(%.3fsec)][S:%d(%.3fsec)][MN:%d(%.3fsec)]", 
		m_dwMainLoop, m_fMainTreadMaxTime, m_iMainProcessMaxPacket, m_dwLogDBLoop,
        m_dwUDPLoop, m_fBroadcastTreadMaxTime, m_dwClientAcceptLoop, m_fClientATreadMaxTime, 
		m_dwServerAcceptLoop, m_fServerATreadMaxTime, 
		m_dwMonitoringAcceptLoop, m_fMonitoringATreadMaxTime );
	ProcessLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s", szLOG );
	sprintf_s( szLOG, "[UT:%I64d:%I64d:%I64d][LB:%I64d:%I64d]", m_iUserSend, m_iUserSendComplete, m_iUserRecv, m_iLogDBSend, m_iLogDBRecv );
	ProcessLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s", szLOG );
	sprintf_s( szLOG, "[DT:%I64d:%I64d:%I64d][UP:%I64d:%I64d]", m_iDBServerSend, m_iDBServerSendComplete, m_iDBServerRecv, m_iUDPSend, m_iUDPRecv  );
	ProcessLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s", szLOG );
	sprintf_s( szLOG, "[ST:%I64d:%I64d:%I64d][MT:%I64d:%I64d:%I64d]", m_iServerSend, m_iServerSendComplete, m_iServerRecv , m_iMServerSend, m_iMServerSendComplete, m_iMServerRecv );
	ProcessLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s", szLOG );
	sprintf_s( szLOG, "[BT:%I64d:%I64d:%I64d]", m_iBillingRelayServerSend, m_iBillingRelayServerSendComplete, m_iBillingRelayServerRecv );
	ProcessLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s", szLOG );
	sprintf_s( szLOG, "[MNT:%I64d:%I64d:%I64d]", m_iMonitoringSend, m_iMonitoringSendComplete, m_iMonitoringRecv );
	ProcessLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s", szLOG );

	ProcessLOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,
		"[RQ:%03d] [UM:%03d] [RI:%03d/%03d] "
		"[RM:%03d:%03d:%03d:%03d:%03d::%03d:%03d:%03d:%03d:%03d] "
		"[UR:%03d] "
		"[UM:%03d:%03d:%03d:%03d:%03d::%03d:%03d:%03d:%03d:%03d] "
		"[RD:%03d:%03d:%03d:%03d:%03d] "
		"[UD:%03d:%03d:%03d:%03d:%03d] ",
		g_Relay.GetNodeSize(),g_UDPNode.Maxsize(),g_Relay.InputCount(),g_Relay.ProcessCount(),

		g_Relay.GetBuffer64PoolCount(),g_Relay.GetBuffer128PoolCount(),g_Relay.GetBuffer256PoolCount(),g_Relay.GetBuffer1024PoolCount(),g_Relay.GetBuffer2048PoolCount(),g_Relay.GetMax64PopCount(),g_Relay.GetMax128PopCount(),g_Relay.GetMax256PopCount(),g_Relay.GetMax1024PopCount(),g_Relay.GetMax2048PopCount(),
		g_UDPNode.ProcessCount(),
		g_UDPNode.GetBuffer64PoolCount(),g_UDPNode.GetBuffer128PoolCount(),g_UDPNode.GetBuffer256PoolCount(), g_UDPNode.GetBuffer1024PoolCount(), g_UDPNode.GetBuffer2048PoolCount(),
		g_UDPNode.GetMax64PopCount(),g_UDPNode.GetMax128PopCount(),g_UDPNode.GetMax256PopCount(),g_UDPNode.GetMax1024PopCount(), g_UDPNode.GetMax2048PopCount(),
		g_Relay.Get64DropCount(),g_Relay.Get128DropCount(),g_Relay.Get256DropCount(),g_Relay.Get1024DropCount(), g_Relay.Get2048DropCount(),
		g_UDPNode.Get64DropCount(),g_UDPNode.Get128DropCount(),g_UDPNode.Get256DropCount(),g_UDPNode.Get1024DropCount(),g_UDPNode.Get2048DropCount()); 
	ProcessLOG.PrintTimeAndLog(LOG_DEBUG_LEVEL,
		"[UCR:%04d:%04d:%04d:%04d:%04d] "
		"[RCR:%04d:%04d:%04d:%04d:%04d]",
		g_UDPNode.Get64RemainderCount(),g_UDPNode.Get128RemainderCount(),g_UDPNode.Get256RemainderCount(),g_UDPNode.Get1024RemainderCount(),g_UDPNode.Get2048RemainderCount(),
		g_Relay.Get64RemainderCount(),g_Relay.Get128RemainderCount(),g_Relay.Get256RemainderCount(),g_Relay.Get1024RemainderCount(),g_Relay.Get2048RemainderCount());
	ProcessLOG.PrintTimeAndLog(0,"CD:%d",g_UDPNode.GetContextpoolCount());

	static std::vector<int> queueProcess;
	queueProcess.clear();
	g_Relay.GetProcessCounts(queueProcess);
	for(DWORD i=0; i < queueProcess.size(); i++)
	{
		ProcessLOG.PrintNoEnterLog(0,"RQ[%d]:%04d ",i,queueProcess[i]);
		
	}
	ProcessLOG.PrintNoEnterLog(0,"\r\n");
	ProcessLOG.PrintTimeAndLog(0,"");
	//m_ServerStatistics.Extract();
	//m_ServerStatistics.Statistics( szLOG, sizeof(szLOG) );
	ProcessLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s", szLOG );
	
	ProcessLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "" );
	Initialize();
}

void ioProcessChecker::MainThreadCheckTimeStart()
{
	if( QueryPerformanceFrequency( &m_MainThreadTime.m_freq ) )
	{
		QueryPerformanceCounter( (LARGE_INTEGER *)&m_MainThreadTime.m_start );
		m_MainThreadTime.m_bFrequency = true;
	}
}

void ioProcessChecker::MainThreadCheckTimeEnd()
{
	if( m_MainThreadTime.m_bFrequency )
	{
		QueryPerformanceCounter( (LARGE_INTEGER *)&m_MainThreadTime.m_end );

		double fCompleteTime = (double)(m_MainThreadTime.m_end - m_MainThreadTime.m_start) / m_MainThreadTime.m_freq.QuadPart*1000000;
		fCompleteTime /= 1000000;
		if( fCompleteTime > m_fMainTreadMaxTime )
			m_fMainTreadMaxTime = fCompleteTime;
	}
}

void ioProcessChecker::BroadcastThreadCheckTimeStart()
{
	if( QueryPerformanceFrequency( &m_BroadcastThreadTime.m_freq ) )
	{
		QueryPerformanceCounter( (LARGE_INTEGER *)&m_BroadcastThreadTime.m_start );
		m_BroadcastThreadTime.m_bFrequency = true;
	}
}

void ioProcessChecker::BroadcastThreadCheckTimeEnd()
{
	if( m_BroadcastThreadTime.m_bFrequency )
	{
		QueryPerformanceCounter( (LARGE_INTEGER *)&m_BroadcastThreadTime.m_end );

		double fCompleteTime = (double)(m_BroadcastThreadTime.m_end - m_BroadcastThreadTime.m_start) / m_BroadcastThreadTime.m_freq.QuadPart*1000000;
		fCompleteTime /= 1000000;
		if( fCompleteTime > m_fBroadcastTreadMaxTime )
			m_fBroadcastTreadMaxTime = fCompleteTime;
	}
}

void ioProcessChecker::WorkThreadCheckTimeStart()
{
	if( QueryPerformanceFrequency( &m_WorkThreadTime.m_freq ) )
	{
		QueryPerformanceCounter( (LARGE_INTEGER *)&m_WorkThreadTime.m_start );
		m_WorkThreadTime.m_bFrequency = true;
	}
}

void ioProcessChecker::WorkThreadCheckTimeEnd()
{
	if( m_WorkThreadTime.m_bFrequency )
	{
		QueryPerformanceCounter( (LARGE_INTEGER *)&m_WorkThreadTime.m_end );

		double fCompleteTime = (double)(m_WorkThreadTime.m_end - m_WorkThreadTime.m_start) / m_WorkThreadTime.m_freq.QuadPart*1000000;
		fCompleteTime /= 1000000;
		if( fCompleteTime > m_fWorkTreadMaxTime )
			m_fWorkTreadMaxTime = fCompleteTime;
	}
}

void ioProcessChecker::ServerAThreadCheckTimeStart()
{
	if( QueryPerformanceFrequency( &m_ServerAThreadTime.m_freq ) )
	{
		QueryPerformanceCounter( (LARGE_INTEGER *)&m_ServerAThreadTime.m_start );
		m_ServerAThreadTime.m_bFrequency = true;
	}
}

void ioProcessChecker::ServerAThreadCheckTimeEnd()
{
	if( m_ServerAThreadTime.m_bFrequency )
	{
		QueryPerformanceCounter( (LARGE_INTEGER *)&m_ServerAThreadTime.m_end );

		double fCompleteTime = (double)(m_ServerAThreadTime.m_end - m_ServerAThreadTime.m_start) / m_ServerAThreadTime.m_freq.QuadPart*1000000;
		fCompleteTime /= 1000000;
		if( fCompleteTime > m_fServerATreadMaxTime )
			m_fServerATreadMaxTime = fCompleteTime;
	}
}

void ioProcessChecker::ClientAThreadCheckTimeStart()
{
	if( QueryPerformanceFrequency( &m_ClientAThreadTime.m_freq ) )
	{
		QueryPerformanceCounter( (LARGE_INTEGER *)&m_ClientAThreadTime.m_start );
		m_ClientAThreadTime.m_bFrequency = true;
	}
}

void ioProcessChecker::ClientAThreadCheckTimeEnd()
{
	if( m_ClientAThreadTime.m_bFrequency )
	{
		QueryPerformanceCounter( (LARGE_INTEGER *)&m_ClientAThreadTime.m_end );

		double fCompleteTime = (double)(m_ClientAThreadTime.m_end - m_ClientAThreadTime.m_start) / m_ClientAThreadTime.m_freq.QuadPart*1000000;
		fCompleteTime /= 1000000;
		if( fCompleteTime > m_fClientATreadMaxTime )
			m_fClientATreadMaxTime = fCompleteTime;
	}
}

void ioProcessChecker::MonitoringAThreadCheckTimeStart()
{
	if( QueryPerformanceFrequency( &m_MonitoringAThreadTime.m_freq ) )
	{
		QueryPerformanceCounter( (LARGE_INTEGER *)&m_MonitoringAThreadTime.m_start );
		m_MonitoringAThreadTime.m_bFrequency = true;
	}
}

void ioProcessChecker::MonitoringAThreadCheckTimeEnd()
{
	if( m_MonitoringAThreadTime.m_bFrequency )
	{
		QueryPerformanceCounter( (LARGE_INTEGER *)&m_MonitoringAThreadTime.m_end );

		double fCompleteTime = (double)(m_MonitoringAThreadTime.m_end - m_MonitoringAThreadTime.m_start) / m_MonitoringAThreadTime.m_freq.QuadPart*1000000;
		fCompleteTime /= 1000000;
		if( fCompleteTime > m_fMonitoringATreadMaxTime )
			m_fMonitoringATreadMaxTime = fCompleteTime;
	}
}

void ioProcessChecker::ProcessIOCP( int iConnectType, DWORD dwFlag, DWORD dwByteTransfer )
{
	if( m_dwLogTime == 0 ) return;

	switch( iConnectType )
	{
	case CONNECT_TYPE_USER:
		{
			if( dwFlag == ASYNCFLAG_RECEIVE )
			{
				UserRecvMessage( dwByteTransfer );
			}
			else if( dwFlag == ASYNCFLAG_SEND )
			{
				UserSendComplete( dwByteTransfer );
			}
		}
		break;
	case CONNECT_TYPE_SERVER:
		{
			if( dwFlag == ASYNCFLAG_RECEIVE )
			{
				ServerRecvMessage( dwByteTransfer );
			}
			else if( dwFlag == ASYNCFLAG_SEND )
			{
				ServerSendComplete( dwByteTransfer );
			}
		}
		break;
	case CONNECT_TYPE_MAIN_SERVER:
		{
			if( dwFlag == ASYNCFLAG_RECEIVE )
			{
				MainServerRecvMessage( dwByteTransfer );
			}
			else if( dwFlag == ASYNCFLAG_SEND )
			{
				MainServerSendComplete( dwByteTransfer );
			}
		}
		break;
	case CONNECT_TYPE_GAMEDB_SERVER:
		{
			if( dwFlag == ASYNCFLAG_RECEIVE )
			{
				DBServerRecvMessage( dwByteTransfer );
			}
			else if( dwFlag == ASYNCFLAG_SEND )
			{
				DBServerSendComplete( dwByteTransfer );
			}
		}
		break;
	case CONNECT_TYPE_BILLING_RELAY_SERVER:
		{
			if( dwFlag == ASYNCFLAG_RECEIVE )
			{
				BillingRelayServerRecvMessage( dwByteTransfer );
			}
			else if( dwFlag == ASYNCFLAG_SEND )
			{
				BillingRelayServerSendComplete( dwByteTransfer );
			}
		}		
		break;
	case CONNECT_TYPE_MONITORING:
		{
			if( dwFlag == ASYNCFLAG_RECEIVE )
			{
				MonitoringRecvMessage( dwByteTransfer );
			}
			else if( dwFlag == ASYNCFLAG_SEND )
			{
				MonitoringSendComplete( dwByteTransfer );
			}
		}		
		break;
	}
}

void ioProcessChecker::UserSendMessage( DWORD dwID, int iSize )
{
	g_PacketChecker.PacketSizeCheck( dwID, iSize );

	if( m_dwLogTime == 0 ) return;
	
	m_iUserSend += (__int64)iSize;
}

void ioProcessChecker::UserSendComplete( int iSize )
{
	if( m_dwLogTime == 0 ) return;

	m_iUserSendComplete += (__int64)iSize;
}

void ioProcessChecker::UserRecvMessage( int iSize )
{
	if( m_dwLogTime == 0 ) return;

	m_iUserRecv += (__int64)iSize;
}

void ioProcessChecker::ServerPacketProcess( DWORD dwID )
{
	//m_ServerStatistics.Hit( dwID );
}

void ioProcessChecker::ServerSendMessage( DWORD dwID, int iSize )
{
	g_PacketChecker.PacketSizeCheck( dwID, iSize );

	if( m_dwLogTime == 0 ) return;

	m_iServerSend += (__int64)iSize;
}

void ioProcessChecker::ServerSendComplete( int iSize )
{
	if( m_dwLogTime == 0 ) return;

	m_iServerSendComplete += (__int64)iSize;
}

void ioProcessChecker::ServerRecvMessage( int iSize )
{
	if( m_dwLogTime == 0 ) return;

	m_iServerRecv += (__int64)iSize;
}

void ioProcessChecker::MainServerSendMessage( DWORD dwID, int iSize )
{
	g_PacketChecker.PacketSizeCheck( dwID, iSize );

	if( m_dwLogTime == 0 ) return;

	m_iMServerSend += (__int64)iSize;
}

void ioProcessChecker::MainServerSendComplete( int iSize )
{
	if( m_dwLogTime == 0 ) return;

	m_iMServerSendComplete += (__int64)iSize;
}

void ioProcessChecker::MainServerRecvMessage( int iSize )
{
	if( m_dwLogTime == 0 ) return;

	m_iMServerRecv += (__int64)iSize;
}

void ioProcessChecker::DBServerSendMessage( DWORD dwID, int iSize )
{
	if( m_dwLogTime == 0 ) return;

	m_iDBServerSend += (__int64)iSize;
}

void ioProcessChecker::DBServerSendComplete( int iSize )
{
	if( m_dwLogTime == 0 ) return;

	m_iDBServerSendComplete += (__int64)iSize;
}

void ioProcessChecker::DBServerRecvMessage( int iSize )
{
	if( m_dwLogTime == 0 ) return;

	m_iDBServerRecv += (__int64)iSize;
}

void ioProcessChecker::LogDBSendMessage( DWORD dwID, int iSize )
{
	if( m_dwLogTime == 0 ) return;

	m_iLogDBSend += (__int64)iSize;
}

void ioProcessChecker::LogDBRecvMessage( int iSize )
{
	if( m_dwLogTime == 0 ) return;

	m_iLogDBRecv += (__int64)iSize;
}

void ioProcessChecker::UDPSendMessage( DWORD dwID, int iSize )
{
	if( m_dwLogTime == 0 ) return;

	m_iUDPSend += (__int64)iSize;
}

void ioProcessChecker::UDPRecvMessage( int iSize )
{
	if( m_dwLogTime == 0 ) return;

	m_iUDPRecv += (__int64)iSize;
}

void ioProcessChecker::BillingRelayServerSendMessage( DWORD dwID, int iSize )
{
	if( m_dwLogTime == 0 ) return;

	m_iBillingRelayServerSend += (__int64)iSize;
}

void ioProcessChecker::BillingRelayServerSendComplete( int iSize )
{
	if( m_dwLogTime == 0 ) return;

	m_iBillingRelayServerSendComplete += (__int64)iSize;
}

void ioProcessChecker::BillingRelayServerRecvMessage( int iSize )
{
	if( m_dwLogTime == 0 ) return;

	m_iBillingRelayServerRecv += (__int64)iSize;
}

void ioProcessChecker::MainProcessMaxPacket( int iPacketCnt )
{
	m_iMainProcessMaxPacket = max( iPacketCnt, m_iMainProcessMaxPacket );
}

void ioProcessChecker::MonitoringSendMessage( DWORD dwID, int iSize )
{
	if( m_dwLogTime == 0 ) return;

	m_iMonitoringSend += (__int64)iSize;
}

void ioProcessChecker::MonitoringSendComplete( int iSize )
{
	if( m_dwLogTime == 0 ) return;

	m_iMonitoringSendComplete += (__int64)iSize;
}

void ioProcessChecker::MonitoringRecvMessage( int iSize )
{
	if( m_dwLogTime == 0 ) return;

	m_iMonitoringRecv += (__int64)iSize;
}