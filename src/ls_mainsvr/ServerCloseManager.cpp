#include "stdafx.h"

#include "ioProcessChecker.h"
#include "NodeInfo/ServerNodeManager.h"

#include "EtcHelpFunc.h"
#include "ServerCloseManager.h"


ServerCloseManager *ServerCloseManager::sg_Instance = NULL;
ServerCloseManager::ServerCloseManager() : m_dwMainTimer( 0 ), m_dwCloseState( CLOSE_NONE ), m_bServerClose( false )
{
}

ServerCloseManager::~ServerCloseManager()
{
}

ServerCloseManager &ServerCloseManager::GetInstance()
{
	if( !sg_Instance )
		sg_Instance = new ServerCloseManager;
	return *sg_Instance;
}

void ServerCloseManager::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void ServerCloseManager::LoadCloseServerInfo()
{
	ioINILoader kLoader;
	if( kLoader.ReloadFile( "config/sp2_server_close.ini" ) )
	{
		LOG.PrintTimeAndLog(0, "%s - INI file reload sucess", __FUNCTION__ );
	}
	else
	{
		LOG.PrintTimeAndLog(0, "%s - INI file reload failed!!", __FUNCTION__ );
	}

	m_dwCloseState = CLOSE_NONE;
	kLoader.SetTitle( "ServerClose" );
	SHORT iYear   = (SHORT)kLoader.LoadInt( "close_year", 0 );
	SHORT iMonth  = (SHORT)kLoader.LoadInt( "close_month", 0 );
	SHORT iDay    = (SHORT)kLoader.LoadInt( "close_day", 0 );
	SHORT iHour   = (SHORT)kLoader.LoadInt( "close_hour", 0 );
	SHORT iMinute = (SHORT)kLoader.LoadInt( "close_minute", 0 );

	if( iYear == 0 || iMonth == 0 || iDay == 0 || iHour == 0 ) return;

	m_cCloseTime = Help::GetSafeValueForCTimeConstructor( iYear, iMonth, iDay, iHour, iMinute, 0 );
	
	char szBuf[2048] = "";
	kLoader.LoadString( "close_ment", "", szBuf, 2048 );
	m_CloseAnnounce = szBuf;

	CTime cCurTime = CTime::GetCurrentTime();
	// Step1 Check
	{
		CTimeSpan cSpanGap( 0, 0, 15, 0 );
		CTime cStepTime = m_cCloseTime - cSpanGap;
		if( Help::ConvertCTimeToDate( cCurTime ) < Help::ConvertCTimeToDate( cStepTime ) )
		{
			m_dwCloseState = CLOSE_STEP1;
		}
	}
	// Step2 Check
	if( m_dwCloseState == CLOSE_NONE )
	{
		CTimeSpan cSpanGap( 0, 0, 5, 0 );
		CTime cStepTime = m_cCloseTime - cSpanGap;
		if( Help::ConvertCTimeToDate( cCurTime ) < Help::ConvertCTimeToDate( cStepTime ) )
		{
			m_dwCloseState = CLOSE_STEP2;
		}
	}
	// Step3 Check
	if( m_dwCloseState == CLOSE_NONE )
	{
		CTime cStepTime = m_cCloseTime;
		if( Help::ConvertCTimeToDate( cCurTime ) < Help::ConvertCTimeToDate( cStepTime ) )
		{
			m_dwCloseState = CLOSE_STEP3;
		}
	}

	LOG.PrintTimeAndLog( 0, "ReserveClose Load Step(%d) : %d.%d.%d.%d.%d : %s", m_dwCloseState,
						 m_cCloseTime.GetYear(), m_cCloseTime.GetMonth(), m_cCloseTime.GetDay(), m_cCloseTime.GetHour(), m_cCloseTime.GetMinute(),
						 m_CloseAnnounce.c_str() );
}

void ServerCloseManager::ProcessCloseCheck()
{
	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	if( m_dwCloseState == CLOSE_NONE )

	// 10초마다 체크
	if( TIMEGETTIME() - m_dwMainTimer < 10000 ) return;        

	m_dwMainTimer = TIMEGETTIME();

	CTime cCurTime = CTime::GetCurrentTime();
	switch( m_dwCloseState )
	{
	case CLOSE_STEP1:
		{
			// 1차 공지 발송
			if( cCurTime.GetYear() == m_cCloseTime.GetYear() )
			{
				CTimeSpan cSpanGap( 0, 0, 15, 0 );
				CTime cStepTime = m_cCloseTime - cSpanGap;
				if( Help::ConvertCTimeToDate( cCurTime ) >= Help::ConvertCTimeToDate( cStepTime ) )
				{
					// 공지 발송
					SP2Packet kPacket( MSTPK_AUTO_CLOSE_ANNOUNCE );
					kPacket << m_CloseAnnounce;
					g_ServerNodeManager.SendMessageAllNode( kPacket );

					LOG.PrintTimeAndLog( 0, "ReserveClose Step1 : %d.%d.%d.%d.%d : %s", 
											cCurTime.GetYear(), cCurTime.GetMonth(), cCurTime.GetDay(), cCurTime.GetHour(), cCurTime.GetMinute(),
											m_CloseAnnounce.c_str() );
					m_dwCloseState = CLOSE_STEP2;
				}
			}
		}
		break;
	case CLOSE_STEP2:
		{
			// 2차 공지 발송
			if( cCurTime.GetYear() == m_cCloseTime.GetYear() )
			{
				CTimeSpan cSpanGap( 0, 0, 5, 0 );
				CTime cStepTime = m_cCloseTime - cSpanGap;
				if( Help::ConvertCTimeToDate( cCurTime ) >= Help::ConvertCTimeToDate( cStepTime ) )
				{
					// 공지 발송
					SP2Packet kPacket( MSTPK_AUTO_CLOSE_ANNOUNCE );
					kPacket << m_CloseAnnounce;
					g_ServerNodeManager.SendMessageAllNode( kPacket );

					LOG.PrintTimeAndLog( 0, "ReserveClose Step2 : %d.%d.%d.%d.%d : %s", 
											cCurTime.GetYear(), cCurTime.GetMonth(), cCurTime.GetDay(), cCurTime.GetHour(), cCurTime.GetMinute(),
											m_CloseAnnounce.c_str() );
					m_dwCloseState = CLOSE_STEP3;
				}
			}
		}
		break;
	case CLOSE_STEP3:
		{
			// 3차 서버 종료
			if( cCurTime.GetYear() == m_cCloseTime.GetYear() )
			{
				if( Help::ConvertCTimeToDate( cCurTime ) >= Help::ConvertCTimeToDate( m_cCloseTime ) )
				{
					// 서버 종료
					//SP2Packet kPacket( MSTPK_ALL_SERVER_EXIT );
					//kPacket << ALL_SERVER_SAFETY_EXIT;
					SP2Packet kPacket( MSTPK_GAMESERVER_SAFETY_EXIT );

					g_ServerNodeManager.SendMessageAllNode( kPacket );

					LOG.PrintTimeAndLog( 0, "ReserveClose Step3 : %d.%d.%d.%d.%d : %s", 
											cCurTime.GetYear(), cCurTime.GetMonth(), cCurTime.GetDay(), cCurTime.GetHour(), cCurTime.GetMinute(),
											m_CloseAnnounce.c_str() );
					m_dwCloseState = CLOSE_NONE;
					m_bServerClose = true;
				}
			}
		}
		break;
	}
}