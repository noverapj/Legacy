#include "StdAfx.h"

#include "../ioApplication.h"
#include "../ioMyInfo.h"
#include "../ioBattleRoomMgr.h"
#include "../WndID.h"
#include "../Setting.h"
#include "../NetworkWrappingFunc.h"
#include "../TextColorTable.h"
#include "../PlayEntityHelpStruct.h"
#include "../EtcHelpFunc.h"
#include "../ioDateHelp.h"

#include "ioRichLabel.h"

#include "ioSP2GUIManager.h"
#include "TimeGateSearchShuffle.h"

#include "../ioShuffleRoomMgr.h"
#include "../ioModeResourceManager.h"

#include <strsafe.h>

TimeGateSearchShuffle::TimeGateSearchShuffle()
{
	for( int i=0; i<2; ++i )
		m_pProcessCircle[i] = NULL;

	m_pModeImageBack			= NULL;
	m_dwChangeTime				= 0;
	m_dwChangeDuration			= 200;

	m_dwChangeProcessCicleTime	= 0;
	m_dwChangeModeImageTime		= 0;

	m_pCurProcessCircle			= NULL;
	m_pCurModeImage				= NULL;
	m_dwCurrSerachTime			= 0;

	for( UIRenderImageVec::iterator iter = m_pModeImageVec.begin(); iter != m_pModeImageVec.end(); ++iter )
	{
		SAFEDELETE( *iter );
	}
	m_pModeImageVec.clear();
	
}

TimeGateSearchShuffle::~TimeGateSearchShuffle()
{
	for( int i=0; i<2; ++i )
		SAFEDELETE( m_pProcessCircle[i] )

	SAFEDELETE( m_pModeImageBack );
}

void TimeGateSearchShuffle::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ProcessCircleA" )
	{
		SAFEDELETE( m_pProcessCircle[0] );
		m_pProcessCircle[0] = pImage;
	}
	else if( szType == "ProcessCircleB" )
	{
		SAFEDELETE( m_pProcessCircle[1] );
		m_pProcessCircle[1] = pImage;
	}
	else if( szType == "ModeImageBack" )
	{
		SAFEDELETE( m_pModeImageBack );
		m_pModeImageBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TimeGateSearchShuffle::SendSearch()
{
	if( g_App.IsMouseBusy() )
		return;

	m_dwCurrSerachTime = FRAMEGETTIME();
}

void TimeGateSearchShuffle::SendCancel()
{
	SP2Packet kPacket( CTPK_SHUFFLEROOM_JOIN_CANCEL );
	TCPNetwork::SendToServer( kPacket );
	m_dwCurrSerachTime = 0;
}

void TimeGateSearchShuffle::ChangeProcessCircle()
{
	if( m_dwChangeProcessCicleTime < FRAMEGETTIME() )
	{
		m_dwChangeProcessCicleTime = FRAMEGETTIME() + CHANGE_DURATION_PROCESS_CIRCLE;
		if( m_iProcessArray == 0 )
			m_iProcessArray = 1;
		else
			m_iProcessArray = 0;
	}
	m_pCurProcessCircle = m_pProcessCircle[m_iProcessArray];
}



void TimeGateSearchShuffle::ChangeModeImage()
{
	if( m_dwChangeModeImageTime < FRAMEGETTIME() )
	{
		m_dwChangeModeImageTime = FRAMEGETTIME() + CHANGE_DURATION_MODE_IMAGE;
		m_iModeArray++;
		if( m_iModeArray >= g_ShuffleRoomMgr.GetMaxModeCount() )
			m_iModeArray = 0;
	}
		
	m_pCurModeImage = g_ModeResourceMgr.GetModeIcon( g_ShuffleRoomMgr.GetTodayModeType( DateHelp::GetTodayOfWeek(), m_iModeArray ) );
}

void TimeGateSearchShuffle::iwm_create()
{
}

void TimeGateSearchShuffle::iwm_show()
{
	g_ShuffleRoomMgr.InitGlobalSearchingShuffleCount();

	m_dwChangeProcessCicleTime = FRAMEGETTIME() + CHANGE_DURATION_PROCESS_CIRCLE;
	m_dwChangeModeImageTime    = FRAMEGETTIME() + CHANGE_DURATION_MODE_IMAGE;
	m_dwChangeTime = FRAMEGETTIME() + m_dwChangeDuration;

	m_pCurProcessCircle = NULL;
	m_pCurModeImage     = NULL;
	m_iProcessArray     = 0;
	m_iModeArray        = 0;
	m_dwCurrSerachTime  = 0;

	int iDayOfWeek = DEFAULT;
#ifdef SHIPPING	
	SYSTEMTIME rkServerTime;
	g_MyInfo.GetServerTime( rkServerTime );	
	iDayOfWeek = rkServerTime.wDayOfWeek;
#else
	SYSTEMTIME systime;
	GetSystemTime(&systime);
	iDayOfWeek = systime.wDayOfWeek;
#endif

	int i = 0;
	for( int iDay = SUN_; iDay <= DEFAULT; ++iDay, ++i )
	{
		ioRichLabel* pLabel = dynamic_cast<ioRichLabel*>( FindChildWnd( iDay ) );
		if( pLabel )
		{
			if( iDayOfWeek == i )
			{
				pLabel->ShowWnd();				
			}
			else
			{
				pLabel->HideWnd();
			}
		}
	}
}

void TimeGateSearchShuffle::iwm_hide()
{
	m_pCurProcessCircle = NULL;
	m_pCurModeImage     = NULL;
	m_iProcessArray     = 0;
	m_iModeArray        = 0;
	m_dwCurrSerachTime  = 0;

	g_ShuffleRoomMgr.InitGlobalSearchingShuffleCount();
}

void TimeGateSearchShuffle::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioWnd::OnProcess( fTimePerSec );

	ChangeProcessCircle();
	ChangeModeImage();

	int iCurrTryCount = g_ShuffleRoomMgr.GetGlobalSearchingShuffleCount();
	int iMaxCount     = g_ShuffleRoomMgr.GetMatchConditionMaxCount( g_MyInfo.GetKillDeathLevel() );

	if( 0 < m_dwCurrSerachTime && m_dwCurrSerachTime <= FRAMEGETTIME() )
	{
		if( iCurrTryCount < iMaxCount )
		{
			SP2Packet kPacket( CTPK_SHUFFLEROOM_JOIN );
			kPacket << iCurrTryCount;
			TCPNetwork::SendToServer( kPacket );

			g_ShuffleRoomMgr.IncreaseGlobalSearchingShuffleCount();
			m_dwCurrSerachTime = FRAMEGETTIME() + 1500;

#ifndef SHIPPING
			LOG.PrintTimeAndLog(0, "%s Serach Start - %d : my kill-death lv : %d", __FUNCTION__, FRAMEGETTIME(), g_MyInfo.GetKillDeathLevel() );
#endif
			
		}
		else
		{
			HideWnd();
			g_GUIMgr.ShowWnd( TIMEGATE_SEARCH_SHUFFLE_RESULT );
			m_dwCurrSerachTime = 0;
			SendCancel();
			g_ShuffleRoomMgr.InitGlobalSearchingShuffleCount();

#ifndef SHIPPING
			LOG.PrintTimeAndLog(0, "%s Serach End - %d : my kill-death lv : %d", __FUNCTION__, FRAMEGETTIME(), g_MyInfo.GetKillDeathLevel() );
#endif			
		}
	}
}

void TimeGateSearchShuffle::SetSearching()
{
	ShowWnd();
	SendSearch();
}

bool TimeGateSearchShuffle::iwm_esc()
{
	if( HasWndStyle( IWS_EXIT_ESC ) )
	{
		iwm_command( FindChildWnd( ID_EXIT ), IOBN_BTNUP, 0 );
		return true;
	}
	return false;
}

void TimeGateSearchShuffle::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			SendCancel();
			HideWnd();
		}
		break;
	}
}


void TimeGateSearchShuffle::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( !m_pCurModeImage || !m_pCurProcessCircle || !m_pModeImageBack )
		return;

	m_pModeImageBack->Render( iXPos + 151, iYPos + 199, UI_RENDER_MULTIPLY );
	m_pCurModeImage->Render( iXPos + 151, iYPos + 199 );
	m_pCurProcessCircle->Render( iXPos + 71, iYPos + 119, UI_RENDER_MULTIPLY );
}

//////////////////////////////////////////////////////////////////////////
TimeGateSearchShuffleResult::TimeGateSearchShuffleResult()
{

}

TimeGateSearchShuffleResult::~TimeGateSearchShuffleResult()
{

}

bool TimeGateSearchShuffleResult::iwm_spacebar()
{
	ioWnd *pBtn = FindChildWnd( ID_RESEARCH );
	if( !pBtn ) return false;
	if( !pBtn->IsShow() ) return false;

	iwm_command( pBtn, IOBN_BTNUP, 0 );
	return true;
}

void TimeGateSearchShuffleResult::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CLOSE:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_RESEARCH:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			TimeGateSearchShuffle *pSearchWnd = dynamic_cast<TimeGateSearchShuffle*>( g_GUIMgr.FindWnd( TIMEGATE_SEARCH_SHUFFLE ) );
			if( pSearchWnd )
			{
				pSearchWnd->SetSearching();
			}
		}
		break;
	}
}

void TimeGateSearchShuffleResult::iwm_show()
{
	int iDayOfWeek = DEFAULT;
#ifdef SHIPPING	
	SYSTEMTIME rkServerTime;
	g_MyInfo.GetServerTime( rkServerTime );	
	iDayOfWeek = rkServerTime.wDayOfWeek;
#else
	SYSTEMTIME systime;
	GetSystemTime(&systime);
	iDayOfWeek = systime.wDayOfWeek;
#endif

	int i = 0;
	for( int iDay = SUN_; iDay <= DEFAULT; ++iDay, ++i )
	{
		ioRichLabel* pLabel = dynamic_cast<ioRichLabel*>( FindChildWnd( iDay ) );
		if( pLabel )
		{
			if( iDayOfWeek == i )
			{
				pLabel->ShowWnd();				
			}
			else
			{
				pLabel->HideWnd();
			}
		}
	}
}