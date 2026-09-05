

#include "StdAfx.h"
#include "ioAlarmManager.h"

template<> ioAlarmManager* Singleton< ioAlarmManager >::ms_Singleton = 0;
ioAlarmManager::ioAlarmManager()
{
	m_dwAutoPrintTime   = 0;
	m_dwSlangPrintCount = 0;	
	m_dwCurGreenMentTime= 0;
	m_dwCurSlangChatCnt = 0;
}

ioAlarmManager::~ioAlarmManager()
{
}

ioAlarmManager& ioAlarmManager::GetSingleton()
{
	return Singleton< ioAlarmManager >::GetSingleton();
}

void ioAlarmManager::Initialize()
{	
	ioINILoader_e kLoader( "config/sp2_alarm_info.ini" );

	//그린슬로건
	m_vGreenMent.clear();
	kLoader.SetTitle_e( "GreenInfo" );	
	m_dwAutoPrintTime   = kLoader.LoadInt_e( "AutoPrintTime", 0 );
	m_dwSlangPrintCount = kLoader.LoadInt_e( "SlangPrintCount", 0 );
	int iMaxGreenMent   = kLoader.LoadInt_e( "MaxGreenMent", 0 );	
	for(int i = 0;i < iMaxGreenMent;i++)
	{
		char szKey[MAX_PATH] = "";
		char szBuf[MAX_PATH] = "";
		sprintf_e( szKey, "GreenMent_%d", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_vGreenMent.push_back( szBuf );
	}
}

void ioAlarmManager::EnterStage()
{
	m_dwCurGreenMentTime = 0;
	m_dwCurSlangChatCnt  = 0;
}

void ioAlarmManager::PrintGreenMent( bool bSlangCntReset /* = false  */ )
{
	if( m_vGreenMent.empty() ) return;

	int r = rand()%m_vGreenMent.size();
	g_ChatMgr.SetSystemMsg( m_vGreenMent[r].c_str() );
	m_dwCurGreenMentTime= REALGETTIME();

	if( bSlangCntReset )
	{
		m_dwCurSlangChatCnt = 0;
	}
}

void ioAlarmManager::CheckChatSlang( const ioHashString &rkChat )
{
	char szChatBuf[MAX_PATH] = "";
	StringCbCopy( szChatBuf, MAX_PATH, g_SlangUtil.ConvertStringWithPreLine( rkChat.c_str() ) );
	if( strcmp(rkChat.c_str(), szChatBuf) == 0 ) return;
	
	if( ++m_dwCurSlangChatCnt >= m_dwSlangPrintCount )
	{
		PrintGreenMent( true );		
	}
}

void ioAlarmManager::ProcessAlarm()
{
	if( m_dwCurGreenMentTime == 0 || REALGETTIME() - m_dwCurGreenMentTime > m_dwAutoPrintTime )
	{
		PrintGreenMent();
	}
}