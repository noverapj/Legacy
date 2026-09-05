#include "stdafx.h"
//#include "Window.h"
#include "MainProcess.h"
#include "ioCriticalError.h"

extern CLog CriticalLOG;

ioCriticalError *ioCriticalError::sg_Instance = NULL;
ioCriticalError::ioCriticalError()
{
	m_ProcessInfo.dwProcessId = m_ProcessInfo.dwThreadId = 0;
	m_ProcessInfo.hProcess = m_ProcessInfo.hThread = NULL;

	m_iSoldierLimitCount = 0;
	m_iDecoTableLimitCount = 0;
	m_iEtcItemTableLimitCount = 0;
	m_iProgressQuestTableLimitCount = 0;
	m_iCompleteQuestTableLimitCount = 0;
	m_iGrowthTableLimitCount = 0;
	m_iFishInvenTableLimitCount = 0;
	m_iExtraItemTableLimitCount = 0;
	m_iMedalTableLimitCount = 0;
	m_iPesoAcquiredLimitCount = 0;
	m_iPesoPossessionLimitCount = 0;
	m_iCostumeLimitCount	= 0;
	m_iAccessoryLimitCount = 0;
}

ioCriticalError::~ioCriticalError()
{

}

ioCriticalError &ioCriticalError::GetInstance()
{
	if( sg_Instance == NULL )
		sg_Instance = new ioCriticalError;
	return *sg_Instance;
}

void ioCriticalError::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void ioCriticalError::Initialize()
{
	ioINILoader kLoader( "config/critical_error.ini" );

	kLoader.SetTitle( "limit_common" );
	m_iSoldierLimitCount = kLoader.LoadInt( "soldier_count", 300 );
	m_iDecoTableLimitCount = kLoader.LoadInt( "deco_table_count", 120 );
	m_iEtcItemTableLimitCount = kLoader.LoadInt( "etc_item_table_count", 120 );
	m_iProgressQuestTableLimitCount = kLoader.LoadInt( "progress_quest_table_count", 90 );
	m_iCompleteQuestTableLimitCount = kLoader.LoadInt( "complete_quest_table_count", 180 );
	m_iGrowthTableLimitCount = kLoader.LoadInt( "growth_table_count", 150 );
	m_iFishInvenTableLimitCount = kLoader.LoadInt( "fish_inven_table_count", 280 );
	m_iExtraItemTableLimitCount = kLoader.LoadInt( "extra_item_table_count", 80 );
	m_iMedalTableLimitCount = kLoader.LoadInt( "medal_table_count", 140 );
	m_iPesoAcquiredLimitCount = kLoader.LoadInt( "peso_acquired_count", 10000000 );
	m_iPesoPossessionLimitCount = kLoader.LoadInt( "peso_possession_count", 50000000 );
	m_iCostumeLimitCount = kLoader.LoadInt( "costume_count", 1000);
	m_iAccessoryLimitCount = kLoader.LoadInt( "accessory_count", 1000);

	char szFile[MAX_PATH] = {};
	sprintf_s(szFile, sizeof(szFile), "%s/critical.log", g_App.GetLogFolder());
	::DeleteFile( szFile );
}

void ioCriticalError::ExcuteProcess( char* szFileName , char* szCmdLine )
{
/*	if( m_ProcessInfo.hProcess != NULL)
	{
		CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioCriticalError::ExcuteProcess Destroy First Process" );
		TerminateProcess( m_ProcessInfo.hProcess, 0 );
	}

	STARTUPINFO StartupInfo;
	StartupInfo.cb          = sizeof(STARTUPINFO);
	StartupInfo.lpReserved  = NULL;
	StartupInfo.lpDesktop   = NULL;
	StartupInfo.lpTitle     = NULL;
	StartupInfo.dwFlags     = STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = SW_SHOWNORMAL;
	StartupInfo.cbReserved2 = 0;
	StartupInfo.lpReserved2 = NULL;

	char szFullCmdLine[4096] = "";
	sprintf_s( szFullCmdLine, "%s \n%s", g_App.GetGameServerName().c_str(), szCmdLine );

	CreateProcess( szFileName, szFullCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &m_ProcessInfo );
*/
}

void ioCriticalError::CheckGameServerExceptionDisconnect( const ioHashString &rkDisconnectIP, DWORD dwDisconnectPort, DWORD dwLastError )
{
	if( g_App.IsWantExit() || g_App.IsReserveLogOut() )
		return;

	CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "CONNECT_TYPE_SERVER Exception Close : %s:%d:%d", rkDisconnectIP.c_str(), dwDisconnectPort, dwLastError );

	char szCmd[2048] = "";
	sprintf_s( szCmd, "Game Server Network Exception Disconnect \n%s:%d \nLast Error(%d) \nError Code(%d)", rkDisconnectIP.c_str(), dwDisconnectPort, dwLastError, 101 );
	ExcuteProcess( "CriticalError.exe", szCmd );
}

void ioCriticalError::CheckMainServerExceptionDisconnect( DWORD dwLastError )
{
	if( g_App.IsWantExit() || g_App.IsReserveLogOut() )
		return;

	CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "CONNECT_TYPE_MAIN_SERVER Exception Close :%d", dwLastError );

	char szCmd[2048] = "";
	sprintf_s( szCmd, "Main Server Network Exception Disconnect \nLast Error(%d) \nError Code(%d)", dwLastError, 102 );
	ExcuteProcess( "CriticalError.exe", szCmd );
}

void ioCriticalError::CheckDBAgentServerExceptionDisconnect( const ioHashString &rkDisconnectIP, DWORD dwDisconnectPort, DWORD dwLastError )
{
	if( g_App.IsWantExit() || g_App.IsReserveLogOut() )
		return;

	CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "CONNECT_TYPE_GAMEDB_SERVER Exception Close : %s:%d:%d", rkDisconnectIP.c_str(), dwDisconnectPort, dwLastError );

	char szCmd[2048] = "";
	sprintf_s( szCmd, "DBAgent Server Network Exception Disconnect \n%s:%d \nLast Error(%d) \nError Code(%d)", rkDisconnectIP.c_str(), dwDisconnectPort, dwLastError, 103 );
	ExcuteProcess( "CriticalError.exe", szCmd );
}

void ioCriticalError::CheckBillingServerExceptionDisconnect( DWORD dwLastError )
{
	if( g_App.IsWantExit() || g_App.IsReserveLogOut() )
		return;

	CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "CONNECT_TYPE_BILLING_RELAY_SERVER Exception Close :%d", dwLastError );

	char szCmd[2048] = "";
	sprintf_s( szCmd, "Billing Relay Server Network Exception Disconnect \nLast Error(%d) \nError Code(%d)", dwLastError, 104 );
	ExcuteProcess( "CriticalError.exe", szCmd );
}

void ioCriticalError::CheckGameServerDisconnect( const ioHashString &rkDisconnectIP, DWORD dwDisconnectPort )
{
	if( g_App.IsWantExit() || g_App.IsReserveLogOut() )
		return;

	CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Disconnect Game Server : %s:%d", rkDisconnectIP.c_str(), dwDisconnectPort );

	char szCmd[2048] = "";
	sprintf_s( szCmd, "Game Server Network Disconnect \n%s:%d \nError Code(%d)", rkDisconnectIP.c_str(), dwDisconnectPort, 106 );
	ExcuteProcess( "CriticalError.exe", szCmd );
}

void ioCriticalError::CheckMainServerDisconnect()
{
	if( g_App.IsWantExit() || g_App.IsReserveLogOut() )
		return;

	CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Disconnect Main Server" );

	char szCmd[2048] = "";
	sprintf_s( szCmd, "Main Server Network Disconnect \nError Code(%d)", 107 );
	ExcuteProcess( "CriticalError.exe", szCmd );
}

void ioCriticalError::CheckDBAgentServerDisconnect( const ioHashString &rkDisconnectIP, DWORD dwDisconnectPort )
{
	if( g_App.IsWantExit() || g_App.IsReserveLogOut() )
		return;

	CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB Agent Server : %s:%d", rkDisconnectIP.c_str(), dwDisconnectPort );

	char szCmd[2048] = "";
	sprintf_s( szCmd, "DBAgent Server Network Disconnect \n%s:%d \nError Code(%d)", rkDisconnectIP.c_str(), dwDisconnectPort, 108 );
	ExcuteProcess( "CriticalError.exe", szCmd );
}

void ioCriticalError::CheckBillingServerDisconnect()
{
	if( g_App.IsWantExit() || g_App.IsReserveLogOut() )
		return;

	CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Billing Relay Server Disconnect" );

	char szCmd[2048] = "";
	sprintf_s( szCmd, "Billing Relay Server Network Disconnect \nError Code(%d)", 109 );
	ExcuteProcess( "CriticalError.exe", szCmd );
}

void ioCriticalError::CheckServerDownMsg( int iServerCount, int iConnectCount )
{
	CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "OnLowConnectExit : %d - %d", iServerCount, iConnectCount );

	char szCmd[2048] = "";
	sprintf_s( szCmd, "Main Server Low Connect Disconnect Command \nMainServer(%d) - GameServer(%d) \nError Code(%d)", iServerCount, iConnectCount, 105 );
	ExcuteProcess( "CriticalError.exe", szCmd );
}

void ioCriticalError::CheckPacketPool( int iPacketQueueCount, DWORD dwPacketID )
{

}

void ioCriticalError::CheckUserPool( int iUserPoolCount )
{
	CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"UserNode MemPool Zero by Add Pool : %d", iUserPoolCount );

	char szCmd[2048] = "";
	sprintf_s( szCmd, "User Node Memory Pool Add \nMax User Node Pool(%d) \nError Code(%d)", iUserPoolCount, 202 );
	ExcuteProcess( "CriticalError.exe", szCmd );
}

void ioCriticalError::CheckCrashLog( const ioHashString &rkCrashLog )
{
	//
	char szCmd[2048] = "";
	sprintf_s( szCmd, "Crash Down \n%s \nError Code(%d)", rkCrashLog.c_str(), 301 );
	ExcuteProcess( "CriticalError.exe", szCmd );
}

void ioCriticalError::CheckSoldierCount( const ioHashString &rkPublicID, int iSoldierCount )
{
	if( iSoldierCount >= m_iSoldierLimitCount )
	{
		// 로그 
		CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"%s - Soldier Max Count : %d", rkPublicID.c_str(), iSoldierCount );

		char szCmd[2048] = "";
		sprintf_s( szCmd, "Soldier Count Full \n(%s) : %d \nError Code(%d)", rkPublicID.c_str(), iSoldierCount, 501 );
		ExcuteProcess( "CriticalError.exe", szCmd );
	}
}

void ioCriticalError::CheckDecoTableCount( const ioHashString &rkPublicID, int iDecoTableCount )
{
	if( iDecoTableCount >= m_iDecoTableLimitCount )
	{
		// 로그 
		CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"%s - Deco Table Max Count : %d", rkPublicID.c_str(), iDecoTableCount );

		char szCmd[2048] = "";
		sprintf_s( szCmd, "Deco DB Table Count Full \n(%s) : %d \nError Code(%d)", rkPublicID.c_str(), iDecoTableCount, 502 );
		ExcuteProcess( "CriticalError.exe", szCmd );
	}
}

void ioCriticalError::CheckEtcItemTableCount( const ioHashString &rkPublicID, int iEtcItemTableCount )
{
	if( iEtcItemTableCount >= m_iEtcItemTableLimitCount )
	{
		// 로그 
		CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"%s - EtcItem Table Max Count : %d", rkPublicID.c_str(), iEtcItemTableCount );

		char szCmd[2048] = "";
		sprintf_s( szCmd, "EtcItem DB Table Count Full \n(%s) : %d \nError Code(%d)", rkPublicID.c_str(), iEtcItemTableCount, 503 );
		ExcuteProcess( "CriticalError.exe", szCmd );
	}
}

void ioCriticalError::CheckProgressQuestTableCount( const ioHashString &rkPublicID, int iProgressQuestTableCount )
{
	if( iProgressQuestTableCount >= m_iProgressQuestTableLimitCount )
	{
		// 로그 
		CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"%s - Progress Quest Table Max Count : %d", rkPublicID.c_str(), iProgressQuestTableCount );

		char szCmd[2048] = "";
		sprintf_s( szCmd, "Progress Quest DB Table Count Full \n(%s) : %d \nError Code(%d)", rkPublicID.c_str(), iProgressQuestTableCount, 504 );
		ExcuteProcess( "CriticalError.exe", szCmd );
	}
}

void ioCriticalError::CheckCompleteQuestTableCount( const ioHashString &rkPublicID, int iCompleteQuestTableCount )
{
	if( iCompleteQuestTableCount >= m_iCompleteQuestTableLimitCount )
	{
		// 로그 
		CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"%s - Complete Quest Table Max Count : %d", rkPublicID.c_str(), iCompleteQuestTableCount );

		char szCmd[2048] = "";
		sprintf_s( szCmd, "Complete Quest DB Table Count Full \n(%s) : %d \nError Code(%d)", rkPublicID.c_str(), iCompleteQuestTableCount, 505 );
		ExcuteProcess( "CriticalError.exe", szCmd );
	}
}

void ioCriticalError::CheckGrowthTableCount( const ioHashString &rkPublicID, int iGrowthTableCount )
{
	if( iGrowthTableCount >= m_iGrowthTableLimitCount )
	{
		// 로그 
		CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"%s - Growth Table Max Count : %d", rkPublicID.c_str(), iGrowthTableCount );

		char szCmd[2048] = "";
		sprintf_s( szCmd, "Growth DB Table Count Full \n(%s) : %d \nError Code(%d)", rkPublicID.c_str(), iGrowthTableCount, 506 );
		ExcuteProcess( "CriticalError.exe", szCmd );
	}
}

void ioCriticalError::CheckFishInvenTableCount( const ioHashString &rkPublicID, int iFishInvenTableCount )
{
	if( iFishInvenTableCount >= m_iFishInvenTableLimitCount )
	{
		// 로그 
		CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"%s - FishInven Table Max Count : %d", rkPublicID.c_str(), iFishInvenTableCount );

		char szCmd[2048] = "";
		sprintf_s( szCmd, "FishInven DB Table Count Full \n(%s) : %d \nError Code(%d)", rkPublicID.c_str(), iFishInvenTableCount, 507 );
		ExcuteProcess( "CriticalError.exe", szCmd );
	}
}

void ioCriticalError::CheckExtraItemTableCount( const ioHashString &rkPublicID, int iExtraItemTableCount )
{
	if( iExtraItemTableCount >= m_iExtraItemTableLimitCount )
	{
		// 로그 
		CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"%s - ExtraItem Table Max Count : %d", rkPublicID.c_str(), iExtraItemTableCount );

		char szCmd[2048] = "";
		sprintf_s( szCmd, "ExtraItem DB Table Count Full \n(%s) : %d \nError Code(%d)", rkPublicID.c_str(), iExtraItemTableCount, 508 );
		ExcuteProcess( "CriticalError.exe", szCmd );
	}
}

void ioCriticalError::CheckMedalTableCount( const ioHashString &rkPublicID, int iMedalTableCount )
{
	if( iMedalTableCount >= m_iMedalTableLimitCount )
	{
		// 로그 
		CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"%s - Medal Table Max Count : %d", rkPublicID.c_str(), iMedalTableCount );

		char szCmd[2048] = "";
		sprintf_s( szCmd, "Medal DB Table Count Full \n(%s) : %d \nError Code(%d)", rkPublicID.c_str(), iMedalTableCount, 509 );
		ExcuteProcess( "CriticalError.exe", szCmd );
	}
}

void ioCriticalError::CheckPesoAcquiredCount( const ioHashString &rkPublicID, int iPesoAcquiredCount )
{
	if( iPesoAcquiredCount >= m_iPesoAcquiredLimitCount )
	{
		// 로그 
		CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"%s - Peso Acquired Limit : %d", rkPublicID.c_str(), iPesoAcquiredCount );

		char szCmd[2048] = "";
		sprintf_s( szCmd, "Peso Acquired Limit \n(%s) : %d \nError Code(%d)", rkPublicID.c_str(), iPesoAcquiredCount, 510 );
		ExcuteProcess( "CriticalError.exe", szCmd );
	}
}

void ioCriticalError::CheckPesoPossessionCount( const ioHashString &rkPublicID, __int64 iPesoPossessionCount )
{
	if( iPesoPossessionCount >= (__int64)m_iPesoPossessionLimitCount )
	{
		// 로그 
		CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"%s - Peso Possession Limit : %I64d", rkPublicID.c_str(), iPesoPossessionCount );

		char szCmd[2048] = "";
		sprintf_s( szCmd, "Peso Possession Limit \n(%s) : %I64d \nError Code(%d)", rkPublicID.c_str(), iPesoPossessionCount, 511 );
		ExcuteProcess( "CriticalError.exe", szCmd );
	}
}

void ioCriticalError::CheckQuestAbuse( const ioHashString &rkPublicID, DWORD dwMainIndex, DWORD dwSubIndex, DWORD dwGapTime, DWORD dwGapValue )
{
	// 로그 
	CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"%s - Quest Abuse : %d - %d - %d - %d", rkPublicID.c_str(), dwMainIndex, dwSubIndex, dwGapTime, dwGapValue );

	char szCmd[2048] = "";
	sprintf_s( szCmd, "Quest Abuse \n(%s) : %d - %d - %d - %d \nError Code(%d)", rkPublicID.c_str(), dwMainIndex, dwSubIndex, dwGapTime, dwGapValue, 601 );
	ExcuteProcess( "CriticalError.exe", szCmd );
}
