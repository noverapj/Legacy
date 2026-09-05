#include "stdafx.h"
#include "./licensemanager.h"
//#include "../Window.h"
#include "../MainProcess.h"
#include "../Shutdown.h"
#include "../Version.h"
#include <strsafe.h>

template<> LicenseManager* Singleton< LicenseManager >::ms_Singleton = 0;

LicenseManager::LicenseManager(void)
{
	m_dwShutDownTime  = 0;
	m_dwSendTime      = 0 ;
	m_dwMinSendMinutes = 60;   // 유저가 조작할 수 있으니 INI 설정하지 않는다.
	m_dwMaxSendMinutes = 120;
}

LicenseManager::~LicenseManager(void)
{
}

LicenseManager& LicenseManager::GetSingleton()
{
	return Singleton< LicenseManager >::GetSingleton();
}

void LicenseManager::OnLicense( sockaddr_in sender_addr, SP2Packet &rkPacket )
{
	if( m_dwShutDownTime != 0 )
		return;

	if( g_App.IsReserveLogOut() )
		return;

	DWORD dwMinShutDownMinutes = 0;
	DWORD dwMaxShutDownMinutes = 0;
	ioHashString szShutDownKey;
	rkPacket >> dwMinShutDownMinutes;
	rkPacket >> dwMaxShutDownMinutes;
	rkPacket >> szShutDownKey;

	if( dwMinShutDownMinutes == 0 || dwMaxShutDownMinutes == 0 )
		return;

	if( szShutDownKey != SHUTDOWN_KEY )
		return;

	char szSenderIP[MAX_PATH]="";
	StringCbPrintf( szSenderIP, sizeof( szSenderIP ), "%d.%d.%d.%d", sender_addr.sin_addr.s_net, sender_addr.sin_addr.s_host, sender_addr.sin_addr.s_lh, sender_addr.sin_addr.s_impno);

	if( strcmp( LICENSE_SERVERIP, szSenderIP ) != 0 )
		return;

	DWORD dwGapTime = dwMaxShutDownMinutes - dwMinShutDownMinutes;
	DWORD dwRandTime = rand()%dwGapTime;
	dwRandTime += dwMinShutDownMinutes;
	dwRandTime *=60000; // min -> ms

	m_dwShutDownTime = TIMEGETTIME() + dwRandTime;
}

void LicenseManager::Pocess()
{
	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김
	ProcessShutDown();
	ProcessSend();
}

void LicenseManager::ProcessShutDown()
{
	if( m_dwShutDownTime == 0 ) return;
	if( TIMEGETTIME() < m_dwShutDownTime ) return;     

	g_App.Shutdown(SHUTDOWN_SAFE, 20);
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "-ShutDown-" );
	m_dwShutDownTime = 0;

	//ioINILoader kLoader( "config/sp2_auto_run.ini" );
	//kLoader.SaveBool( "config", "AutoRun", false );
}

void LicenseManager::ProcessSend()
{
	if( m_dwSendTime == 0 )
	{
		DWORD dwGapTime = m_dwMaxSendMinutes - m_dwMinSendMinutes;
		DWORD dwRandTime = rand()%dwGapTime;
		dwRandTime += m_dwMinSendMinutes;
		dwRandTime *=60000; // min -> ms

		m_dwSendTime = TIMEGETTIME() + dwRandTime;
		return;
	}

	if( TIMEGETTIME() < m_dwSendTime ) return;         
	m_dwSendTime = 0;
	
	SP2Packet kPacket( GUPK_LICENSE );
	kPacket << g_App.GetClientMoveIP();
	kPacket << g_App.GetCSPort();
	kPacket << FILEVER;

	g_UDPNode.SendMessage( LICENSE_SERVERIP, LICENSE_SERVERPORT, kPacket );
}
