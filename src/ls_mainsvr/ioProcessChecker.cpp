#include "stdafx.h"
#include "ioProcessChecker.h"

#include "Network/GameServer.h"
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
				ProcessLOG.PrintTimeAndLog( 0, "%s %d Line 0x%x : T[%.3fsec]", m_pFileName, m_iFileLine, m_dwPacketID, fCompleteTime / 1000000 );
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
	ioINILoader kLoader( "config/sp2_process.ini" );
	kLoader.SetTitle( "ProcessChecker" );
	m_dwLogTime = kLoader.LoadInt( "LogTime", 10000 );
}

void ioProcessChecker::Initialize()
{
	// PROCESS TIME CHECK
	m_fMainTreadMaxTime = 0.0f;
	m_MainThreadTime.m_bFrequency = false;
	m_MainThreadTime.m_start = 0;
	m_MainThreadTime.m_end   = 0;

	m_fServerATreadMaxTime = 0.0f;
	m_ServerAThreadTime.m_bFrequency = false;
	m_ServerAThreadTime.m_start = 0;
	m_ServerAThreadTime.m_end   = 0;

	m_fClientATreadMaxTime = 0.0f;
	m_ClientAThreadTime.m_bFrequency = false;
	m_ClientAThreadTime.m_start = 0;
	m_ClientAThreadTime.m_end   = 0;

	// PROCESS LOOP
	m_dwMainLoop = 0;	          
	m_dwClientAcceptLoop = m_dwServerAcceptLoop = 0;

	// PACKET
	m_iUserSend = 0;
	m_iUserSendComplete = 0;
	m_iUserRecv = 0;
	m_iServerSend = 0;
	m_iServerSendComplete = 0;
	m_iServerRecv = 0;
	m_iLogDBServerSend = 0;
	m_iLogDBServerSendComplete = 0;
	m_iLogDBServerRecv = 0;
	m_iDBServerSend = 0;
	m_iDBServerSendComplete = 0;
	m_iDBServerRecv = 0;
	m_iMainProcessMaxPacket = 0;
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
	sprintf_s( szLOG, "[M:%d(%.3fsec)][MP:%d][C:%d(%.3fsec)][S:%d(%.3fsec)]", m_dwMainLoop, m_fMainTreadMaxTime, m_iMainProcessMaxPacket,
					m_dwClientAcceptLoop, m_fClientATreadMaxTime, m_dwServerAcceptLoop, m_fServerATreadMaxTime );
	ProcessLOG.PrintTimeAndLog( 0, "%s", szLOG );
	sprintf_s( szLOG, "[UT:%I64d:%I64d:%I64d][DT:%I64d:%I64d:%I64d]", m_iUserSend, m_iUserSendComplete, m_iUserRecv, m_iDBServerSend, m_iDBServerSendComplete, m_iDBServerRecv );
	ProcessLOG.PrintTimeAndLog( 0, "%s", szLOG );
	sprintf_s( szLOG, "[ST:%I64d:%I64d:%I64d][LB:%I64d:%I64d:%I64d]", m_iServerSend, m_iServerSendComplete, m_iServerRecv , m_iLogDBServerSend, m_iLogDBServerSendComplete, m_iLogDBServerRecv );
	ProcessLOG.PrintTimeAndLog( 0, "%s", szLOG );
	ProcessLOG.PrintTimeAndLog( 0, "" );
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

void ioProcessChecker::ProcessIOCP( int iConnectType, DWORD dwFlag, DWORD dwByteTransfer )
{
	if( m_dwLogTime == 0 ) return;

	switch( iConnectType )
	{
	case CONNECT_TYPE_MANAGERTOOL:
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
	case CONNECT_TYPE_GAMESERVER:
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
	case CONNECT_TYPE_LOGDB_SERVER:
		{
			if( dwFlag == ASYNCFLAG_RECEIVE )
			{
				LogDBServerRecvMessage( dwByteTransfer );
			}
			else if( dwFlag == ASYNCFLAG_SEND )
			{
				LogDBServerSendComplete( dwByteTransfer );
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
	}
}

void ioProcessChecker::UserSendMessage( int iSize )
{
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

void ioProcessChecker::ServerSendMessage( int iSize )
{
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

void ioProcessChecker::LogDBServerSendMessage( int iSize )
{
	if( m_dwLogTime == 0 ) return;

	m_iLogDBServerSend += (__int64)iSize;
}

void ioProcessChecker::LogDBServerSendComplete( int iSize )
{
	if( m_dwLogTime == 0 ) return;

	m_iLogDBServerSendComplete += (__int64)iSize;
}

void ioProcessChecker::LogDBServerRecvMessage( int iSize )
{
	if( m_dwLogTime == 0 ) return;

	m_iLogDBServerRecv += (__int64)iSize;
}

void ioProcessChecker::DBServerSendMessage( int iSize )
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

void ioProcessChecker::MainProcessMaxPacket( int iPacketCnt )
{
	m_iMainProcessMaxPacket = max( iPacketCnt, m_iMainProcessMaxPacket );
}